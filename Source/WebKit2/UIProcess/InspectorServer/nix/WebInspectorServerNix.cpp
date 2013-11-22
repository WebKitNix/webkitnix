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

#if ENABLE(INSPECTOR_SERVER)
#include "WebInspectorServer.h"

#include "WebInspectorProxy.h"
#include "WebPageProxy.h"
#include <WebCore/MIMETypeRegistry.h>
#include <sys/stat.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>

namespace WebKit {

static String inspectorBaseURL()
{
    String inspectorFilesPath = WK2_WEB_INSPECTOR_INSTALL_DIR;
    if (access(inspectorFilesPath.utf8().data(), R_OK))
        inspectorFilesPath = WK2_WEB_INSPECTOR_DIR; // TODO: Perhaps we should check which folder was the last one modified?

    return inspectorFilesPath;
}

bool WebInspectorServer::platformResourceForPath(const String& path, Vector<char>& data, String& contentType)
{
    // The page list contains an unformated list of pages that can be inspected with a link to open a session.
    if (path == "/pagelist.json") {
        buildPageList(data, contentType);
        return true;
    }

    // Point the default path to a formatted page that queries the page list and display them.
    String localPath = inspectorBaseURL() + ((path == "/") ? ASCIILiteral("/inspectorPageIndex.html") : path);

    FILE* fileHandle = fopen(localPath.utf8().data(), "r");
    if (!fileHandle)
        return false;

    struct stat fileStat;
    if (fstat(fileno(fileHandle), &fileStat)) {
        fclose(fileHandle);
        return false;
    }

    data.grow(fileStat.st_size);
    int bytesRead = fread(data.data(), 1, fileStat.st_size, fileHandle);
    fclose(fileHandle);

    if (bytesRead < fileStat.st_size)
        return false;

    size_t extStart = localPath.reverseFind('.');
    if (extStart == notFound)
        return false;

    String ext = localPath.substring(extStart + 1);
    if (ext.isEmpty())
        return false;

    contentType = WebCore::MIMETypeRegistry::getMIMETypeForExtension(ext);

    return true;
}

void WebInspectorServer::buildPageList(Vector<char>& data, String& contentType)
{
    StringBuilder builder;
    builder.appendLiteral("[ ");
    ClientMap::iterator end = m_clientMap.end();
    for (ClientMap::iterator it = m_clientMap.begin(); it != end; ++it) {
        WebPageProxy* webPage = it->value->page();
        if (it != m_clientMap.begin())
            builder.appendLiteral(", ");
        builder.appendLiteral("{ \"id\": ");
        builder.appendNumber(it->key);
        builder.appendLiteral(", \"title\": \"");
        builder.append(webPage->pageLoadState().title());
        builder.appendLiteral("\", \"url\": \"");
        builder.append(webPage->pageLoadState().activeURL());
        builder.appendLiteral("\", \"inspectorUrl\": \"");
        builder.appendLiteral("/Main.html?page=");
        builder.appendNumber(it->key);
        builder.appendLiteral("\" }");
    }
    builder.appendLiteral(" ]");
    CString cstr = builder.toString().utf8();
    data.append(cstr.data(), cstr.length());
    contentType = "application/json; charset=utf-8";
}

}

#endif // ENABLE(INSPECTOR_SERVER)
