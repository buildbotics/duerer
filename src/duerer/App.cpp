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

#include <cbang/util/DefaultCatch.h>

#include <cbang/os/SystemUtilities.h>

#include <cbang/security/SSLContext.h>
#include <cbang/time/Time.h>
#include <cbang/log/Logger.h>
#include <cbang/event/Event.h>

#include <stdlib.h>
#include <unistd.h>

using namespace duerer;
using namespace cb;
using namespace std;


App::App() :
  cb::Application("Duerer"), dns(base), client(base, dns, new SSLContext),
  server(*this), awsRegion("us-east-1"),
  awsUploadExpires(Time::SEC_PER_HOUR * 2) {

  options.pushCategory("Duerer Server");
  options.add("outbound-ip", "IP address for outbound connections.  Defaults "
              "to first http-address.");
  options.add("document-root", "Serve files from this directory.");
  options.popCategory();

  options.pushCategory("Debugging");
  options.add("debug-libevent", "Enable verbose libevent debugging"
              )->setDefault(false);
  options.popCategory();

  options.pushCategory("Amazon Web Services");
  options.addTarget("aws-access-key-id", awsID, "AWS access key ID");
  options.addTarget("aws-secret-access-key", awsSecret,
                    "AWS secret access key")->setObscured();;
  options.addTarget("aws-bucket", awsBucket, "AWS bucket name");
  options.addTarget("aws-region", awsRegion, "AWS region code");
  options.addTarget("aws-upload-expires", awsUploadExpires,
                    "Lifetime in seconds of an AWS upload token");
  options.popCategory();

  // Enable libevent logging
  Event::Event::enableLogging(3);
}


int App::init(int argc, char *argv[]) {
  int i = Application::init(argc, argv);
  if (i == -1) return -1;

  // Libevent debugging
  if (options["debug-libevent"].toBoolean()) Event::Event::enableDebugLogging();

  server.init();

  // Initialized outbound IP
  if (options["outbound-ip"].hasValue())
    outboundIP = IPAddress(options["outbound-ip"]);
  outboundIP.setPort(0);

  // Handle exit signal
  base.newSignal(SIGINT, this, &App::signalEvent).add();
  base.newSignal(SIGTERM, this, &App::signalEvent).add();

  return 0;
}


void App::run() {
  try {
    base.dispatch();
    LOG_INFO(1, "Clean exit");
  } CATCH_ERROR;
}


void App::signalEvent(Event::Event &e, int signal, unsigned flags) {
  base.loopExit();
}
