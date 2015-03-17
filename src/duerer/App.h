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

#ifndef DUERER_APP_H
#define DUERER_APP_H

#include "Server.h"

#include <cbang/Application.h>
#include <cbang/net/IPAddress.h>
#include <cbang/security/KeyPair.h>

#include <cbang/event/Base.h>
#include <cbang/event/DNSBase.h>
#include <cbang/event/Client.h>

namespace cb {
  namespace Event {class Event;}
  namespace MariaDB {class EventDB;}
}


namespace duerer {
  class App : public cb::Application {
    cb::Event::Base base;
    cb::Event::DNSBase dns;
    cb::Event::Client client;

    Server server;
    cb::IPAddress outboundIP;

    std::string awsID;
    std::string awsSecret;
    std::string awsBucket;
    std::string awsRegion;
    uint32_t awsUploadExpires;

  public:
    App();

    cb::Event::Base &getEventBase() {return base;}
    cb::Event::DNSBase &getEventDNS() {return dns;}
    cb::Event::Client &getEventClient() {return client;}

    const cb::IPAddress &getOutboundIP() const {return outboundIP;}

    const std::string &getAWSID() const {return awsID;}
    const std::string &getAWSSecret() const {return awsSecret;}
    const std::string &getAWSBucket() const {return awsBucket;}
    const std::string &getAWSRegion() const {return awsRegion;}
    uint32_t getAWSUploadExpires() const {return awsUploadExpires;}

    // From cb::Application
    int init(int argc, char *argv[]);
    void run();

    void signalEvent(cb::Event::Event &e, int signal, unsigned flags);
  };
}

#endif // DUERER_APP_H

