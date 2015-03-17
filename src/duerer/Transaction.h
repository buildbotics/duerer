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

#ifndef DUERER_TRANSACTION_H
#define DUERER_TRANSACTION_H

#include <cbang/event/Request.h>
#include <cbang/event/RequestMethod.h>
#include <cbang/event/PendingRequest.h>


namespace cb {
  namespace JSON {
    class Sync;
    class Writer;
    class Value;
  }
}


namespace duerer {
  class App;
  class User;
  class AWS4Post;

  class Transaction : public cb::Event::Request {
    App &app;

  public:
    Transaction(App &app, evhttp_request *req);
    ~Transaction();

    cb::SmartPointer<cb::JSON::Dict> parseArgsPtr();

    // Event::WebServer request callbacks
    bool processRequest();
  };
}

#endif // DUERER_TRANSACTION_H

