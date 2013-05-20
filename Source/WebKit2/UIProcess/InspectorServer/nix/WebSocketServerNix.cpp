/*
 * Copyright (C) 2012 Samsung Electronics Ltd. All Rights Reserved.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */


#include "config.h"
#include "WebSocketServer.h"

#if ENABLE(INSPECTOR_SERVER)
#include <gio/gio.h>
#include <glib.h>

#include "WebSocketServerConnection.h"
#include <WebCore/SocketStreamHandle.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/gobject/GOwnPtr.h>
#include <wtf/text/CString.h>


using namespace WebCore;

namespace WebKit {


static gboolean connectionCallback(GSocketService* service, GSocketConnection* connection, GObject* sourceObject, WebSocketServer* server)
{
    UNUSED_PARAM(service);
    UNUSED_PARAM(sourceObject);
    OwnPtr<WebSocketServerConnection> webSocketConnection = adoptPtr(new WebSocketServerConnection(server->client(), server));
    webSocketConnection->setSocketHandle(SocketStreamHandle::create(connection, webSocketConnection.get()));
    server->didAcceptConnection(webSocketConnection.release());

    return TRUE;
}

void WebSocketServer::platformInitialize()
{
    m_socketService = adoptGRef(g_socket_service_new());
    g_signal_connect(m_socketService.get(), "incoming", G_CALLBACK(connectionCallback), this);
    g_socket_service_start(m_socketService.get());
}

bool WebSocketServer::platformListen(const String& bindAddress, unsigned short port)
{
    GRefPtr<GInetAddress> address = adoptGRef(g_inet_address_new_from_string(bindAddress.utf8().data()));
    GRefPtr<GSocketAddress> socketAddress = adoptGRef(g_inet_socket_address_new(address.get(), port));
    return g_socket_listener_add_address(G_SOCKET_LISTENER(m_socketService.get()), socketAddress.get(), G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, 0, 0, 0);
}

void WebSocketServer::platformClose()
{
    g_socket_service_stop(m_socketService.get());
}

}

#endif // ENABLE(INSPECTOR_SERVER)
