/******************************************************************************\

                 This file is part of the Duerer image server.

                Copyright (c) 2014-2015, Cauldron Development LLC
                               All rights reserved.

        The Duerer image server is free software: you can redistribute
        it and/or modify it under the terms of the GNU General Public
        License as published by the Free Software Foundation, either
        version 2 of the License, or (at your option) any later version.

        The Duerer image server is distributed in the hope that it will
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

#include "App.h"
#include "Transaction.h"

#include <cbang/util/DefaultCatch.h>

#include <cbang/os/SystemUtilities.h>

#include <cbang/time/Time.h>
#include <cbang/log/Logger.h>
#include <cbang/event/Event.h>

#include <sys/types.h>
#include <sys/wait.h>

using namespace duerer;
using namespace cb;
using namespace std;


App::App() :
  cb::Application("Duerer"), dns(base), client(base, dns),
  server(*this), cacheDir("cache") {

  options.pushCategory("Duerer Server");
  options.addTarget("cache", cacheDir,
                    "Cache and serve files from this directory.");
  options.popCategory();

  options.pushCategory("Debugging");
  options.add("debug-libevent", "Enable verbose libevent debugging"
              )->setDefault(false);
  options.popCategory();

  options.pushCategory("Amazon Web Services");
  options.addTarget("aws-bucket", awsBucket, "AWS bucket name");
  options.popCategory();

  // Enable libevent logging
  Event::Event::enableLogging(3);
}


void App::registerSubprocess(uint64_t pid, Transaction *tran) {
  subprocesses[pid] = tran;
}


bool App::wait(const string &path, Transaction *tran) {
  waiters_t::iterator it = waiters.find(path);
  if (it == waiters.end()) return false;

  it->second.push_back(tran);

  return true;
}


void App::lock(const string &path) {
  waiters.insert(waiters_t::value_type(path, vector<Transaction *>()));
}


void App::unlock(const string &path) {
  waiters_t::iterator it = waiters.find(path);
  if (it == waiters.end()) return;

  vector<Transaction *> trans = it->second;
  waiters.erase(it);

  for (unsigned i = 0; i < trans.size(); i++)
    trans[i]->processRequest();
}


int App::init(int argc, char *argv[]) {
  int i = Application::init(argc, argv);
  if (i == -1) return -1;

  // Libevent debugging
  if (options["debug-libevent"].toBoolean()) Event::Event::enableDebugLogging();

  server.init();

  // Handle exit signal
  base.newSignal(SIGINT, this, &App::interruptSignal).add();
  base.newSignal(SIGTERM, this, &App::interruptSignal).add();
  base.newSignal(SIGCHLD, this, &App::childSignal).add();

  // Make sure cache dir exists
  SystemUtilities::ensureDirectory(getCacheDir());

  return 0;
}


void App::run() {
  try {
    base.dispatch();
    LOG_INFO(1, "Clean exit");
  } CATCH_ERROR;
}


void App::interruptSignal(Event::Event &e, int signal, unsigned flags) {
  base.loopExit();
}


void App::childSignal(Event::Event &e, int signal, unsigned flags) {
  pid_t pid;

  while (0 < (pid = waitpid(-1, 0, WNOHANG))) {
    subprocesses_t::iterator it = subprocesses.find(pid);
    if (it == subprocesses.end()) continue;
    it->second->sendFile();
    subprocesses.erase(it);
  }

  e.renew(signal);
  e.add();
}
