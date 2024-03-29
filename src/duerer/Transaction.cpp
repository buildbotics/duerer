/******************************************************************************\

                 This file is part of the Duerer Webserver.

                Copyright (c) 2014-2015, Cauldron Development LLC
                               All rights reserved.

        The Duerer Webserver is free software: you can redistribute
        it and/or modify it under the terms of the GNU General Public
        License as published by the Free Software Foundation, either
        version 2 of the License, or (at your option) any later version.

        The Duerer Webserver is distributed in the hope that it will
        be useful, but WITHOUT ANY WARRANTY; without even the implied
        warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
        PURPOSE.  See the GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this software.  If not, see
        <http://www.gnu.org/licenses/>.

        In addition, BSD licensing may be granted on a case by case basis
        by written permission from at least one of the copyright holders.
        You may request written permission by emailing the authors.

                For information regarding this software email:
                               Joseph Coffland
                        joseph@cauldrondevelopment.com

\******************************************************************************/

#include "Transaction.h"
#include "App.h"

#include <cbang/log/Logger.h>
#include <cbang/Catch.h>
#include <cbang/os/SystemUtilities.h>
#include <cbang/os/Subprocess.h>

#include <cbang/event/Buffer.h>
#include <cbang/event/Headers.h>


using namespace std;
using namespace cb;
using namespace duerer;


Transaction::Transaction(App &app, Event::RequestMethod method, const URI &uri,
                         const Version &version) :
  Request(method, uri, version), app(app), locked(false) {}


Transaction::~Transaction() {unlock();}


string Transaction::getBase() const {
  return app.getCacheDir() + getURI().getPath();
}


string Transaction::getPath() const {return getBase() + "/" + size;}


void Transaction::lock() {
  locked = true;
  app.lock(getBase());
}


void Transaction::unlock() {if (locked) app.unlock(getBase());}


void Transaction::sendFile(const string &path) {
  Event::Buffer buf;
  buf.addFile(path);

  reply(buf);
}


void Transaction::sendFile() {sendFile(getPath());}


void Transaction::convertImage() {
  string path = getBase();

  string cmd = app.getCmd(size);
  cmd = String::replace(cmd, "\\$src", "orig");
  cmd = String::replace(cmd, "\\$dst", size);
  cmd = String::replace(cmd, "\\$common", app.getCommon());

  Subprocess proc;
  proc.setWorkingDirectory(path);
  proc.exec(cmd);

  app.registerSubprocess(proc.getPID(), this);
}


bool Transaction::processRequest() {
  // Sanity check path
  string base = getBase();
  if (base.find("..") != string::npos) {
    reply(HTTP_UNAUTHORIZED);
    return true;
  }

  // Make sure the file is not locked
  if (app.wait(base, this)) return true;

  // Get type
  auto &args = *parseArgs();
  size = String::toLower(args.getString("size", "orig"));

  // Validate size
  if (size != "orig" && !app.hasCmd(size)) THROW("Invalid size " << size);

  // Check if file exists
  string path = base + "/" + size;
  if (SystemUtilities::exists(path)) {
    sendFile(path);
    return true;
  }

  // Lock the file
  lock();

  // Download the original file
  string orig = base + "/orig";
  if (size == "orig" || !SystemUtilities::exists(orig)) {
    string url = "http://" + app.getAWSBucket() + ".s3.amazonaws.com" +
      URI::encode(getURI().getPath());

    pending = app.getEventClient()
      .call(url, HTTP_GET, this, &Transaction::storeImage);
    pending->send();

  } else convertImage();

  return true;
}


void Transaction::storeImage(Request &req) {
  if (!req.isOk()) return;

  if (req.getResponseCode() != HTTP_OK) {
    reply(req.getResponseCode());
    return;
  }

  Event::Buffer buf = req.getInputBuffer();

  // Ensure the cache directory exists
  string path = getBase();
  SystemUtilities::ensureDirectory(path);

  // Save the original file
  path += "/orig";
  SystemUtilities::oopen(path)->write(buf.toCString(), buf.getLength());

  if (size == "orig") sendFile(path);
  else convertImage();
}
