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



using namespace std;
using namespace cb;
using namespace duerer;


Transaction::Transaction(App &app, evhttp_request *req) :
  Request(req), app(app) {
  LOG_DEBUG(5, "Transaction()");
}


Transaction::~Transaction() {
  LOG_DEBUG(5, "~Transaction()");
}


SmartPointer<JSON::Dict> Transaction::parseArgsPtr() {
  return SmartPointer<JSON::Dict>::Null(&parseArgs());
}


bool Transaction::processRequest() {
  return true;
}
