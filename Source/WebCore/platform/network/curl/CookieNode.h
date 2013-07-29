/*
 * Copyright (C) Julien Chaffraix <julien.chaffraix@gmail.com>
 * Copyright (C) 2011, 2012 Research In Motion Limited. All rights reserved.
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

#ifndef CookieNode_h
#define CookieNode_h

#include "Cookie.h"
#include "CookieDatabaseBackingStore.h"
#include "CookieManager.h"
#include "ParsedCookie.h"
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class CookieNode {

public:
    CookieNode();
    CookieNode(PassRefPtr<ParsedCookie>, Vector<String>&, CookieDatabaseBackingStore*, CookieFilter = WithHttpOnlyCookies, CookieStoragePolicy = InMemoryAndBackingStore);
    ~CookieNode();

    void insert(PassRefPtr<ParsedCookie>, Vector<String>&, CookieDatabaseBackingStore*, CookieFilter = WithHttpOnlyCookies, CookieStoragePolicy = InMemoryAndBackingStore);
    void get(Vector<String>&, Vector<RefPtr<ParsedCookie> >&);
    void getDomains(String, HashSet<String>&);
    void removeAll();
    void removeWithName(Vector<String>&, const String&, CookieDatabaseBackingStore*, CookieStoragePolicy = InMemoryAndBackingStore);
    void removeWithHostName(Vector<String>&, CookieDatabaseBackingStore*, CookieStoragePolicy = InMemoryAndBackingStore);

private:
    HashMap<String, CookieNode> m_subdomains;
    Vector<RefPtr<ParsedCookie> > m_cookies;
};

} // namespace WebCore

#endif // CookieNode_h
