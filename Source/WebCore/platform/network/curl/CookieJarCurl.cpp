/*
 *  Copyright (C) 2013 University of Szeged
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"

#include "CookieManager.h"
#include "PlatformCookieJar.h"
#include "ResourceHandleManager.h"
#include "URL.h"

namespace WebCore {

static inline bool thirdParty(const URL& firstParty, const URL& url)
{
    // FIXME: Should we check the path equality here?
    return firstParty.host() != url.host();
}

void setCookiesFromDOM(const NetworkStorageSession&, const URL& firstParty, const URL& url, const String& value)
{
    CookieManager& manager = CookieManager::getInstance();
    if (!manager.cookiesAllowed() || (!manager.thirdPartyCookiesAllowed() && thirdParty(firstParty, url)))
        return;

    manager.setCookie(url, value, NoHttpOnlyCookies);
}

String cookiesForDOM(const NetworkStorageSession& /*session*/, const URL& /*firstParty*/, const URL& url)
{
    return CookieManager::getInstance().cookiesForSession(url, NoHttpOnlyCookies);
}

String cookieRequestHeaderFieldValue(const NetworkStorageSession& /*session*/, const URL& /*firstParty*/, const URL& url)
{
    return CookieManager::getInstance().cookiesForSession(url, WithHttpOnlyCookies);
}

bool cookiesEnabled(const NetworkStorageSession&, const URL& /*firstParty*/, const URL& /*url*/)
{
    return CookieManager::getInstance().cookiesAllowed();
}

bool getRawCookies(const NetworkStorageSession&, const URL& /*firstParty*/, const URL& url, Vector<Cookie>& rawCookies)
{
    Vector<RefPtr<ParsedCookie> > cookiesVector;
    CookieManager::getInstance().getRawCookies(url, cookiesVector, WithHttpOnlyCookies);

    Vector<RefPtr<ParsedCookie> >::iterator end = cookiesVector.end();
    for (Vector<RefPtr<ParsedCookie> >::iterator it = cookiesVector.begin(); it != end; ++it)
        (*it)->appendWebCoreCookie(rawCookies);
    return true;
}

void deleteCookie(const NetworkStorageSession&, const URL& url, const String& name)
{
    CookieManager::getInstance().removeCookieWithName(url, name);
}

void getHostnamesWithCookies(const NetworkStorageSession&, HashSet<String>& hostnames)
{
    CookieManager::getInstance().getHostnames(hostnames);
}

void deleteCookiesForHostname(const NetworkStorageSession&, const String& hostname)
{
    CookieManager::getInstance().removeCookiesWithHostname(hostname);
}

void deleteAllCookies(const NetworkStorageSession&)
{
    CookieManager::getInstance().removeAllCookies(InMemoryAndBackingStore);
}

void deleteAllCookiesModifiedAfterDate(const NetworkStorageSession&, double)
{
    // FIXME: Not yet implemented
}

}
