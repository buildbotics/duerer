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

#pragma once

#include <cbang/event/Request.h>
#include <cbang/event/PendingRequest.h>


namespace duerer {
  class App;

  class Transaction : public cb::Event::Request {
    App &app;

    std::string size;
    cb::SmartPointer<cb::Event::PendingRequest> pending;
    bool locked;

  public:
    Transaction(App &app, evhttp_request *req);
    ~Transaction();

    std::string getBase() const;
    std::string getPath() const;

    void lock();
    void unlock();

    void sendFile(const std::string &path);
    void sendFile();
    void convertImage();

    // Event::WebServer request callbacks
    bool processRequest();

    // Client callback
    void storeImage(Request *req, int err);
  };
}
