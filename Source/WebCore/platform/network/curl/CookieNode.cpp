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

#include "config.h"
#include "CookieNode.h"

#include "Cookie.h"
#include "CookieDatabaseBackingStore.h"
#include "CookieManager.h"
#include "ParsedCookie.h"
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

CookieNode::CookieNode() { }

CookieNode::CookieNode(PassRefPtr<ParsedCookie> cookie, Vector<String>& splittedHost, CookieDatabaseBackingStore* backingStore, CookieFilter filter, CookieStoragePolicy policy)
{
    insert(cookie, splittedHost, backingStore, filter, policy);
}

CookieNode::~CookieNode() { }

static void addCookieToVector(PassRefPtr<ParsedCookie> cookie, Vector<RefPtr<ParsedCookie> >& cookies)
{
    unsigned size = cookies.size();
    for (unsigned i = 0; i < size; i++) {
        RefPtr<ParsedCookie> current = cookies.at(i);
        if (cookie->name() == current->name() && cookie->path() == current->path()) {
            cookies[i] = getPtr(cookie);
            return;
        }
    }
    cookies.append(cookie);
}

void CookieNode::insert(PassRefPtr<ParsedCookie> cookie, Vector<String>& splittedHost, CookieDatabaseBackingStore* backingStore, CookieFilter filter, CookieStoragePolicy policy)
{
    if (splittedHost.size()) {
        String subdomain = splittedHost.last();
        splittedHost.removeLast();

        HashMap<String, CookieNode>::iterator it = m_subdomains.find(subdomain);
        if (it != m_subdomains.end())
            it->value.insert(cookie, splittedHost, backingStore, filter, policy);
        else
            m_subdomains.add(subdomain, CookieNode(cookie, splittedHost, backingStore, filter, policy));

    } else {
        unsigned oldestIndex = 0;
        unsigned size = m_cookies.size();
        for (unsigned i = 0; i < size; ++i) {
            RefPtr<ParsedCookie> currentCookie = m_cookies.at(i);
            if (cookie->name() == currentCookie->name() && cookie->path() == currentCookie->path()) {
                if (!(currentCookie->isHttpOnly() && filter == NoHttpOnlyCookies)) {
                    m_cookies[i] = getPtr(cookie);
                    if (policy == InMemoryAndBackingStore)
                        backingStore->update(cookie);
                }
                return;
            }

            if (currentCookie->lastAccessed() < m_cookies.at(oldestIndex)->lastAccessed())
                oldestIndex = i;
        }

        if (size == s_maxCookieCountPerHost) {
            m_cookies[oldestIndex] = getPtr(cookie);
            if (policy == InMemoryAndBackingStore)
                backingStore->update(cookie);
        } else {
            m_cookies.append(getPtr(cookie));
            if (policy == InMemoryAndBackingStore)
                backingStore->insert(cookie);
        }
    }
}

void CookieNode::get(Vector<String>& splittedHost, Vector<RefPtr<ParsedCookie> >& cookies)
{
    unsigned size = m_cookies.size();
    if (splittedHost.size()) {
        for (unsigned i = 0; i < size; ++i)
            if (m_cookies.at(i)->isAllowedForSubdomains())
                addCookieToVector(m_cookies.at(i), cookies);

        String subdomain = splittedHost.last();
        splittedHost.removeLast();

        HashMap<String, CookieNode>::iterator it = m_subdomains.find(subdomain);
        if (it != m_subdomains.end())
            it->value.get(splittedHost, cookies);
    } else {
        for (unsigned i = 0; i < size; ++i)
            addCookieToVector(m_cookies.at(i), cookies);
    }
}

void CookieNode::getDomains(String current, HashSet<String>& domains)
{
    if (m_cookies.size())
        domains.add(current);

    HashMap<String, CookieNode>::iterator end = m_subdomains.end();
    for (HashMap<String, CookieNode>::iterator it = m_subdomains.begin(); it != end; ++it)
        it->value.getDomains(it->key + "." + current, domains);
}

void CookieNode::removeAll()
{
    m_subdomains.clear();
}

void CookieNode::removeWithName(Vector<String>& splittedHost, const String& name, CookieDatabaseBackingStore* backingStore, CookieStoragePolicy policy)
{
    if (splittedHost.size()) {
        String subdomain = splittedHost.last();
        splittedHost.removeLast();

        HashMap<String, CookieNode>::iterator it = m_subdomains.find(subdomain);
        if (it != m_subdomains.end())
            it->value.removeWithName(splittedHost, name, backingStore, policy);
    } else {
        unsigned size = m_cookies.size();
        for (unsigned i = 0; i < size; ++i) {
            RefPtr<ParsedCookie> current = m_cookies.at(i);
            if (current->name() == name) {
                if (policy == InMemoryAndBackingStore)
                    backingStore->remove(current);
                m_cookies.remove(i);
                return;
            }
        }
    }
}

void CookieNode::removeWithHostName(Vector<String>& splittedHost, CookieDatabaseBackingStore* backingStore, CookieStoragePolicy policy)
{
    if (splittedHost.size()) {
        String subdomain = splittedHost.last();
        splittedHost.removeLast();

        HashMap<String, CookieNode>::iterator it = m_subdomains.find(subdomain);
        if (it != m_subdomains.end())
            it->value.removeWithHostName(splittedHost, backingStore, policy);
    } else {
        unsigned size = m_cookies.size();
        for (unsigned i = 0; i < size; ++i)
            if (policy == InMemoryAndBackingStore)
                backingStore->remove(m_cookies.at(i));

        m_cookies.clear();
    }
}

} // namespace WebCore
