/*
 * Copyright (C) 2013 University of Szeged
 * All rights reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef WebCurlRequestManagerProxy_h
#define WebCurlRequestManagerProxy_h

#include "WebContextSupplement.h"
#include <wtf/PassRefPtr.h>
#include <wtf/text/WTFString.h>

namespace WebKit {

class WebContext;

class WebCurlRequestManagerProxy : public WebContextSupplement  {
public:
    static const char* supplementName();
    static PassRefPtr<WebCurlRequestManagerProxy> create(WebContext*);
    virtual ~WebCurlRequestManagerProxy();

    void setHostAllowsAnyHTTPSCertificate(const String&);
    void setClientCertificateInfo(const String&, const String&, const String&);

private:
    explicit WebCurlRequestManagerProxy(WebContext*);

    // WebContextSupplement
    virtual void refWebContextSupplement() OVERRIDE;
    virtual void derefWebContextSupplement() OVERRIDE;
};

} // namespace WebKit

#endif // WebCurlRequestManagerProxy_h
