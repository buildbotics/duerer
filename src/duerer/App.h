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

#include <cbang/event/Base.h>
#include <cbang/event/DNSBase.h>
#include <cbang/event/Client.h>

#include <map>


namespace cb {
  namespace Event {class Event;}
  namespace MariaDB {class EventDB;}
}


namespace duerer {
  class Transaction;

  class App : public cb::Application {
    cb::Event::Base base;
    cb::Event::DNSBase dns;
    cb::Event::Client client;

    Server server;
    std::string cacheDir;
    std::string awsBucket;

    typedef std::map<uint64_t, Transaction *> subprocesses_t;
    subprocesses_t subprocesses;

    typedef std::map<std::string, std::vector<Transaction *> > waiters_t;
    waiters_t waiters;

  public:
    App();

    cb::Event::Base &getEventBase() {return base;}
    cb::Event::DNSBase &getEventDNS() {return dns;}
    cb::Event::Client &getEventClient() {return client;}

    const std::string &getCacheDir() const {return cacheDir;}
    const std::string &getAWSBucket() const {return awsBucket;}

    bool hasCmd(const std::string &size) const;
    const std::string &getCmd(const std::string &size) const;

    void registerSubprocess(uint64_t pid, Transaction *tran);

    bool wait(const std::string &path, Transaction *tran);
    void lock(const std::string &path);
    void unlock(const std::string &path);

    // From cb::Application
    int init(int argc, char *argv[]);
    void run();

    void interruptSignal(cb::Event::Event &e, int signal, unsigned flags);
    void childSignal(cb::Event::Event &e, int signal, unsigned flags);
  };
}

#endif // DUERER_APP_H

