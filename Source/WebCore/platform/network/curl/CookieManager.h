/*
 * Copyright (C) 2009 Julien Chaffraix <jchaffraix@pleyo.com>
 * Copyright (C) 2010, 2011, 2012 Research In Motion Limited. All rights reserved.
 * Copyright (C) 2013 University of Szeged
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CookieManager_h
#define CookieManager_h

#include "Cookie.h"
#include "KURL.h"
#include "ParsedCookie.h"
#include <wtf/HashSet.h>
#include <wtf/OwnPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class CookieDatabaseBackingStore;
class CookieNode;

enum CookieFilter {
    NoHttpOnlyCookies,
    WithHttpOnlyCookies,
};

enum CookieStoragePolicy {
    InMemoryAndBackingStore,
    OnlyInMemory
};

enum CookieAcceptPolicy {
    Always,
    Never,
    OnlyFromMainDocumentDomain
};

static const unsigned s_globalMaxCookieCount = 6000;
static const unsigned s_cookiesToDeleteWhenLimitReached = 60;
static const unsigned s_maxCookieCountPerHost = 60;
static const unsigned s_maxCookieLength = 4096;

class CookieManager {
public:
    static CookieManager& getInstance();

    String cookiesForSession(const KURL&, CookieFilter);
    String cookiesForSessionWithPrivateModeCookies(const KURL&, CookieFilter);

    void setCookie(const KURL&, const String&, CookieFilter, CookieStoragePolicy = InMemoryAndBackingStore);
    void setCookieInPrivateMode(const KURL&, const String&, CookieFilter);

    void getRawCookies(const KURL&, Vector<RefPtr<ParsedCookie> >&, CookieFilter = WithHttpOnlyCookies, bool = false);
    void getHostnames(HashSet<String>&);

    void removeAllCookies(CookieStoragePolicy = InMemoryAndBackingStore);
    void removeCookieWithName(const KURL&, const String&, CookieStoragePolicy = InMemoryAndBackingStore);
    void removeCookiesWithHostname(const String&, CookieStoragePolicy = InMemoryAndBackingStore);

    PassRefPtr<ParsedCookie> parseOneCookie(const KURL&, const String&);

    void setCookieAcceptPolicy(CookieAcceptPolicy acceptPolicy) { m_acceptPolicy = acceptPolicy; }
    CookieAcceptPolicy cookieAcceptPolicy() { return m_acceptPolicy; }

    bool thirdPartyCookiesAllowed() { return m_acceptPolicy == Always || m_acceptPolicy == OnlyFromMainDocumentDomain; }

    bool cookiesAllowed() { return m_acceptPolicy != Never; }
    void setCookieChangeCallback(void (*pointer)()) { m_cookieChangeCallback = pointer; }

private:
    CookieManager();
    CookieManager(CookieManager const&);
    CookieManager& operator=(CookieManager const&);
    virtual ~CookieManager();

    OwnPtr<CookieNode> m_tree;
    OwnPtr<CookieNode> m_privateCookiesTree;
    OwnPtr<CookieDatabaseBackingStore> m_backingStore;

    CookieAcceptPolicy m_acceptPolicy;
    void (*m_cookieChangeCallback)();
};

} // namespace WebCore

#endif // CookieManager_h
