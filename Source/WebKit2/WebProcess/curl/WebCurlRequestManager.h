/*
 * Copyright (C) 2013 University of Szeged
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

#ifndef WebCurlRequestManager_h
#define WebCurlRequestManager_h

#include "MessageReceiver.h"
#include "WebProcessSupplement.h"
#include <wtf/text/WTFString.h>

namespace WebKit {

class WebProcess;

class WebCurlRequestManager : public WebProcessSupplement, private IPC::MessageReceiver {
    WTF_MAKE_NONCOPYABLE(WebCurlRequestManager);
public:
    explicit WebCurlRequestManager(WebProcess*);
    ~WebCurlRequestManager();

    static const char* supplementName();

private:
    // IPC::MessageReceiver
    void didReceiveMessage(IPC::Connection*, IPC::MessageDecoder&) override;

    void setHostAllowsAnyHTTPSCertificate(const String&);
    void setClientCertificateInfo(const String&, const String&, const String&);

    WebProcess* m_process;
};

} // namespace WebKit

#endif
