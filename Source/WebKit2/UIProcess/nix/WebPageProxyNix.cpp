/*
 * Copyright (C) 2011 Samsung Electronics
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebPageProxy.h"

#include "PageClient.h"
#include "NotImplemented.h"
#include "WebBackForwardList.h"
#include "WebBackForwardListItem.h"
#include "APIData.h"
#include "WebPageMessages.h"
#include "WebProcessProxy.h"
#include "WebKitVersion.h"

#include <sys/utsname.h>
#include <wtf/text/StringBuilder.h>

#define STRINGFY(VALUE) #VALUE
#define GET_WEBKIT_VERSION(MAJOR, MINOR) STRINGFY(MAJOR) "." STRINGFY(MINOR)
#define UA_WEBKIT_VERSION GET_WEBKIT_VERSION(WEBKIT_MAJOR_VERSION, WEBKIT_MINOR_VERSION)

#if PLATFORM(X11)
#define UA_PLATFORM "X11"
#else
#define UA_PLATFORM "Unknown"
#endif

namespace WebKit {

String WebPageProxy::standardUserAgent(const String& applicationName)
{
    WTF::String osVersion;

    struct utsname name;
    if (uname(&name) != -1)
        osVersion = WTF::String(name.sysname) + " " + WTF::String(name.machine);
    else
        osVersion = "Unknown";

    WTF::StringBuilder userAgent;
    userAgent.appendLiteral("Mozilla/5.0 (" UA_PLATFORM "; ");
    userAgent.append(osVersion);
    userAgent.appendLiteral(") AppleWebKit/" UA_WEBKIT_VERSION " (KHTML, like Gecko)");
    if (!applicationName.isEmpty()) {
        userAgent.appendLiteral(" ");
        userAgent.append(applicationName);
    }
    userAgent.appendLiteral(" Safari/" UA_WEBKIT_VERSION);

    return userAgent.toString();
}

void WebPageProxy::platformInitialize()
{
}

void WebPageProxy::getEditorCommandsForKeyEvent(Vector<WTF::String>&)
{
    notImplemented();
}

void WebPageProxy::saveRecentSearches(const String&, const Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::loadRecentSearches(const String&, Vector<String>&)
{
    notImplemented();
}

void WebPageProxy::initializeUIPopupMenuClient(const WKPageUIPopupMenuClientBase* client)
{
    m_uiPopupMenuClient.initialize(client);
}

PassRefPtr<API::Data> WebPageProxy::sessionStateData(WebPageProxySessionStateFilterCallback filter, void* context) const
{
    auto encoder = std::make_unique<CoreIPC::ArgumentEncoder>();
    unsigned index = m_backForwardList->currentIndex();
    const BackForwardListItemVector& entries = m_backForwardList->entries();
    BackForwardListItemVector filtered;
    WKPageRef pageRef = toAPI(const_cast<WebPageProxy*>(this));
    for (unsigned i = 0; i < entries.size(); ++i) {
        if (filter && !filter(pageRef, WKPageGetSessionHistoryURLValueType(), toURLRef(entries[i]->originalURL().impl()), context)) {
            if (i < index)
                --index;
            continue;
        }
        filtered.append(entries[i]);
    }
    SessionState state(filtered, index);
    state.encode(*encoder);
    return API::Data::create(encoder->buffer(), encoder->bufferSize());
}

static uint64_t generateNewItemID()
{
    static uint64_t next = 2;
    return next += 2;
}

void WebPageProxy::restoreFromSessionStateData(API::Data* data)
{
    // Clear the back/forward list even if the list is empty.
    m_backForwardList->clear();
    if (!data)
        return;

    auto decoder = std::make_unique<CoreIPC::ArgumentDecoder>(data->bytes(), data->size());

    SessionState state;
    if (!SessionState::decode(*(decoder.get()), state))
        return;

    const BackForwardListItemVector& entries = state.list();
    if (state.isEmpty())
        return;

    // Currently our representation retrieves the same itemID from the previous
    // session state items. We need to update these to avoid clashes in the WebProcessProxy
    // history item map.
    for (size_t i = 0; i < entries.size(); ++i) {
        WebBackForwardListItem* originalItem = entries[i].get();

        WebBackForwardListItem* item = WebBackForwardListItem::create(originalItem->originalURL(),
            originalItem->url(), originalItem->title(), originalItem->backForwardData().data(),
            originalItem->backForwardData().size(), generateNewItemID()).leakRef();

        m_backForwardList->addItem(item);
        process().registerNewWebBackForwardListItem(item);
        if (i == state.currentIndex()) {
            auto transaction = m_pageLoadState.transaction();
            m_pageLoadState.setPendingAPIRequestURL(transaction, item->url());
        }
    }

    process().send(Messages::WebPage::RestoreSessionAndNavigateToCurrentItem(state), m_pageID);
}

} // namespace WebKit
