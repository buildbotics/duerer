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

#include "Server.h"
#include "App.h"
#include "Transaction.h"

#include <cbang/openssl/SSLContext.h>

using namespace cb;
using namespace std;
using namespace duerer;


Server::Server(App &app) :
  Event::WebServer(app.getOptions(), app.getEventBase()), app(app) {}


void Server::init() {
  Event::WebServer::init();

  // Event::HTTP request callbacks
  addMember<Transaction>(HTTP_ANY, "", &Transaction::processRequest);
}


Event::Request *Server::createRequest(evhttp_request *req) {
  return new Transaction(app, req);
}
