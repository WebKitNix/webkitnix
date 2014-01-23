/*
 * Copyright (C) 2013 University of Szeged
 * All rights reserved.
 *
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

#include "config.h"
#include "WebCurlRequestManagerProxy.h"

#include "WebContext.h"
#include "WebCurlRequestManagerMessages.h"

namespace WebKit {

const char* WebCurlRequestManagerProxy::supplementName()
{
	return "WebCurlRequestManagerProxy";
}

PassRefPtr<WebCurlRequestManagerProxy> WebCurlRequestManagerProxy::create(WebContext* context)
{
    return adoptRef(new WebCurlRequestManagerProxy(context));
}

WebCurlRequestManagerProxy::WebCurlRequestManagerProxy(WebContext* context)
    : WebContextSupplement(context)
{
}

WebCurlRequestManagerProxy::~WebCurlRequestManagerProxy()
{
}

// WebContextSupplement
void WebCurlRequestManagerProxy::refWebContextSupplement()
{
}

void WebCurlRequestManagerProxy::derefWebContextSupplement()
{
}

void WebCurlRequestManagerProxy::setHostAllowsAnyHTTPSCertificate(const String& host)
{
    if (!context())
        return;

    context()->sendToAllProcesses(Messages::WebCurlRequestManager::SetHostAllowsAnyHTTPSCertificate(host));
}

void WebCurlRequestManagerProxy::setClientCertificateInfo(const String& host, const String& cert, const String& key)
{
    if (!context())
        return;
    context()->sendToAllProcesses(Messages::WebCurlRequestManager::SetClientCertificateInfo(host, cert, key));
}

} // namespace WebKit
