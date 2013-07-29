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

#include "config.h"
#include "CookieManager.h"

#include "Cookie.h"
#include "CookieDatabaseBackingStore.h"
#include "CookieNode.h"
#include "KURL.h"
#include "Logging.h"
#include "ParsedCookie.h"
#include <arpa/inet.h>
#include <curl/curl.h>
#include <wtf/CurrentTime.h>
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

#define LOG_ERROR_AND_RETURN(format, ...) \
    { \
        LOG_ERROR(format, ## __VA_ARGS__); \
        return 0; \
    }

static inline bool isLightweightSpace(UChar c)
{
    return (c == ' ' || c == '\t');
}

static bool isValidIPAddress(const String& ipString)
{
    const CString ipaddr = ipString.ascii();
    const char* ip = ipaddr.data();
    struct sockaddr_in dst;

    if (inet_pton(AF_INET,  ip, &(dst.sin_addr)) == 1)
        return true;

    struct sockaddr_in6 dst6;

    if (inet_pton(AF_INET6, ip, &(dst6.sin6_addr)) == 1)
        return true;

    return false;
}

typedef Vector<RefPtr<ParsedCookie> >::iterator CookieIterator;

static String getCookieString(const Vector<RefPtr<ParsedCookie> >& cookies)
{
    double curTime = currentTime();
    StringBuilder cookieBuilder;

    size_t count = cookies.size();
    for (size_t i = 0; i < count; ++i) {
        RefPtr<ParsedCookie> current = cookies.at(i);
        if (i > 0)
            cookieBuilder.append("; ");
        current->setLastAccessed(curTime);
        cookieBuilder.append(current->toNameValuePair());
    }

    return cookieBuilder.toString();
}

CookieManager::CookieManager()
    : m_tree(WTF::adoptPtr(new CookieNode()))
    , m_privateCookiesTree(WTF::adoptPtr(new CookieNode()))
    , m_backingStore(WTF::adoptPtr(new CookieDatabaseBackingStore()))
    , m_acceptPolicy(OnlyFromMainDocumentDomain)
{
    m_backingStore->init(m_tree.get());
}

CookieManager::~CookieManager()
{
    removeAllCookies(OnlyInMemory);
    m_backingStore.clear();
}

CookieManager& CookieManager::getInstance()
{
    static CookieManager cookieManager;
    return cookieManager;
}

void CookieManager::setCookie(const KURL& url, const String& value, CookieFilter filter, CookieStoragePolicy policy)
{
    if (m_acceptPolicy == Never)
        return;

    RefPtr<ParsedCookie> cookie = parseOneCookie(url, value);

    if (!cookie || cookie->name().isEmpty())
        return;

    if (filter == NoHttpOnlyCookies && cookie->isHttpOnly())
        return;

    CookieStoragePolicy selectedPolicy = cookie->isSession() ? OnlyInMemory : policy;
    Vector<String> domain;
    cookie->domain().split(".", false, domain);

    m_tree->insert(cookie, domain, m_backingStore.get(), filter, selectedPolicy);

    if (m_cookieChangeCallback)
        m_cookieChangeCallback();
}

void CookieManager::setCookieInPrivateMode(const KURL& url, const String& value, CookieFilter filter)
{
    if (m_acceptPolicy == Never)
        return;

    RefPtr<ParsedCookie> cookie = parseOneCookie(url, value);

    if (!cookie || cookie->name().isEmpty())
        return;

    if (filter == NoHttpOnlyCookies && cookie->isHttpOnly())
        return;

    Vector<String> domain;
    cookie->domain().split(".", false, domain);

    m_privateCookiesTree->insert(cookie, domain, m_backingStore.get(), filter, OnlyInMemory);

    if (m_cookieChangeCallback)
        m_cookieChangeCallback();
}

void CookieManager::getRawCookies(const KURL& url, Vector<RefPtr<ParsedCookie> >& cookies, CookieFilter filter, bool privateMode)
{
    Vector<String> domain;
    url.host().split(".", false, domain);

    if (privateMode)
        m_privateCookiesTree->get(domain, cookies);
    else
        m_tree->get(domain, cookies);

    for (unsigned i = 0; i < cookies.size(); ++i) {
        RefPtr<ParsedCookie> currentCookie = cookies.at(i);

        if ((currentCookie->isHttpOnly() && filter == NoHttpOnlyCookies)
            || (currentCookie->isSecure() && url.protocolIs("http"))
            || !(url.path().startsWith(currentCookie->path(), false))
            || currentCookie->hasExpired()) {
            cookies.remove(i);
            --i;
        }
    }
}

void CookieManager::getHostnames(HashSet<String>& hostnames)
{
    m_tree->getDomains("", hostnames);
}

String CookieManager::cookiesForSession(const KURL& url, CookieFilter filter)
{
    Vector<RefPtr<ParsedCookie> > cookies;
    getRawCookies(url, cookies, filter);

    return getCookieString(cookies);
}

String CookieManager::cookiesForSessionWithPrivateModeCookies(const KURL& url, CookieFilter filter)
{
    Vector<RefPtr<ParsedCookie> > cookies;
    getRawCookies(url, cookies, filter, false);
    getRawCookies(url, cookies, filter, true); // cookies for private browsing mode

    return getCookieString(cookies);
}


void CookieManager::removeAllCookies(CookieStoragePolicy policy)
{
    m_tree->removeAll();
    if (policy == InMemoryAndBackingStore)
        m_backingStore->removeAll();
}

void CookieManager::removeCookieWithName(const KURL& url, const String& name, CookieStoragePolicy policy)
{
    Vector<String> domain;
    url.host().split(".", false, domain);
    m_tree->removeWithName(domain, name, m_backingStore.get(), policy);
}

void CookieManager::removeCookiesWithHostname(const String& hostname, CookieStoragePolicy policy)
{
    Vector<String> domain;
    hostname.split(".", false, domain);
    m_tree->removeWithHostName(domain, m_backingStore.get(), policy);
}

PassRefPtr<ParsedCookie> CookieManager::parseOneCookie(const KURL& url, const String& cookie)
{
    unsigned start = 0;
    unsigned end = cookie.length();

    double curTime = currentTime();
    RefPtr<ParsedCookie> res = ParsedCookie::create(curTime);

    res->setProtocol(url.protocol());

    unsigned tokenEnd = start; // Token end contains the position of the '=' or the end of a token
    unsigned pairEnd = start; // Pair end contains always the position of the ';'

    // Find the first ';' which is not double-quoted and the '=' (if they exist).
    bool foundEqual = false;
    while (pairEnd < end && cookie[pairEnd] != ';') {
        if (cookie[pairEnd] == '=') {
            if (tokenEnd == start) {
                tokenEnd = pairEnd;
                foundEqual = true;
            }
        } else if (cookie[pairEnd] == '"') {
            size_t secondQuotePosition = cookie.find('"', pairEnd + 1);
            if (secondQuotePosition != notFound && secondQuotePosition <= end) {
                pairEnd = secondQuotePosition + 1;
                continue;
            }
        }
        pairEnd++;
    }

    unsigned tokenStart = start;

    bool hasName = false; // This is a hack to avoid changing too much in this brutally brittle code.
    if (tokenEnd != start) {
        // There is a '=' so parse the NAME
        unsigned nameEnd = tokenEnd;

        // The tokenEnd is the position of the '=' so the nameEnd is one less
        nameEnd--;

        // Remove lightweight spaces.
        while (nameEnd && isLightweightSpace(cookie[nameEnd]))
            nameEnd--;

        while (tokenStart < nameEnd && isLightweightSpace(cookie[tokenStart]))
            tokenStart++;

        if (nameEnd + 1 <= tokenStart)
            LOG_ERROR_AND_RETURN("Empty name. Rejecting the cookie");

        String name = cookie.substring(tokenStart, nameEnd + 1 - start);
        res->setName(name);
        hasName = true;
    }

    // Now parse the VALUE
    tokenStart = tokenEnd + 1;
    if (!hasName)
        --tokenStart;

    // Skip lightweight spaces in our token
    while (tokenStart < pairEnd && isLightweightSpace(cookie[tokenStart]))
        tokenStart++;

    tokenEnd = pairEnd;
    while (tokenEnd > tokenStart && isLightweightSpace(cookie[tokenEnd - 1]))
        tokenEnd--;

    String value;
    if (tokenEnd == tokenStart) {
        // Firefox accepts empty value so we will do the same
        value = String();
    } else
        value = cookie.substring(tokenStart, tokenEnd - tokenStart);

    if (hasName)
        res->setValue(value);
    else if (foundEqual)
        return ParsedCookie::create(curTime);
    else
        res->setName(value); // No NAME=VALUE, only NAME

    while (pairEnd < end) {
        // Switch to the next pair as pairEnd is on the ';' and fast-forward any lightweight spaces.
        pairEnd++;
        while (pairEnd < end && isLightweightSpace(cookie[pairEnd]))
            pairEnd++;

        tokenStart = pairEnd;
        tokenEnd = tokenStart; // initialize token end to catch first '='

        while (pairEnd < end && cookie[pairEnd] != ';') {
            if (tokenEnd == tokenStart && cookie[pairEnd] == '=')
                tokenEnd = pairEnd;
            pairEnd++;
        }

        // FIXME : should we skip lightweight spaces here ?

        unsigned length = tokenEnd - tokenStart;
        unsigned tokenStartSvg = tokenStart;

        String parsedValue;
        if (tokenStart != tokenEnd) {
            // There is an equal sign so remove lightweight spaces in VALUE
            tokenStart = tokenEnd + 1;
            while (tokenStart < pairEnd && isLightweightSpace(cookie[tokenStart]))
                tokenStart++;

            tokenEnd = pairEnd;
            while (tokenEnd > tokenStart && isLightweightSpace(cookie[tokenEnd - 1]))
                tokenEnd--;

            parsedValue = cookie.substring(tokenStart, tokenEnd - tokenStart);
        } else {
            // If the parsedValue is empty, initialise it in case we need it
            parsedValue = String();
            // Handle a token without value.
            length = pairEnd - tokenStart;
        }

        // Detect which "cookie-av" is parsed
        // Look at the first char then parse the whole for performance issue
        switch (cookie[tokenStartSvg]) {
        case 'P':
        case 'p' : {
            if (length >= 4 && ((cookie.find("ath", tokenStartSvg + 1, false) - tokenStartSvg) == 1)) {
                // We need the path to be decoded to match those returned from KURL::path().
                // The path attribute may or may not include percent-encoded characters. Fortunately
                // if there are no percent-encoded characters, decoding the url is a no-op.
                res->setPath(curl_unescape(parsedValue.utf8().data(), parsedValue.length()));

                // We have to disable the following check because sites like Facebook and
                // Gmail currently do not follow the spec.

                // Check if path attribute is a prefix of the request URI.
                if (!url.path().startsWith(res->path()))
                    LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (path): it does not math the URL", cookie.ascii().data());
            } else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (path)", cookie.ascii().data());
            break;
        }

        case 'D':
        case 'd' : {
            if (length >= 6 && ((cookie.find("omain", tokenStartSvg + 1, false) - tokenStartSvg) == 1)) {
                if (parsedValue.length() > 1 && parsedValue[0] == '"' && parsedValue[parsedValue.length() - 1] == '"')
                    parsedValue = parsedValue.substring(1, parsedValue.length() - 2);

                String host = url.host();

                // Check if the domain contains an embedded dot.
                size_t dotPosition = parsedValue.find(".", 1);
                if (dotPosition == notFound || dotPosition == parsedValue.length())
                    LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (domain): it does not contain an embedded dot", cookie.ascii().data());

                if (isValidIPAddress(host)) {
                    if (parsedValue != host)
                        LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (domain)", cookie.ascii().data());

                } else {
                    // If the domain does not start with a dot, add one for security checks and to distinguish it from host-only domains
                    // For example: ab.c.com dose not domain match b.c.com;
                    if (parsedValue[0] != '.')
                        parsedValue = "." + parsedValue;

                    host = "." + host;

                    if (!host.endsWith(parsedValue, false))
                        LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (domain): it does not domain match the host (%s)", cookie.ascii().data(), host.ascii().data());
                }

                res->setDomain(parsedValue);
            } else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (domain)", cookie.ascii().data());
            break;
        }

        case 'E' :
        case 'e' : {
            if (length >= 7 && ((cookie.find("xpires", tokenStartSvg + 1, false) - tokenStartSvg) == 1))
                res->setExpiry(parsedValue);
            else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (expires)", cookie.ascii().data());
            break;
        }

        case 'M' :
        case 'm' : {
            if (length >= 7 && ((cookie.find("ax-age", tokenStartSvg + 1, false) - tokenStartSvg) == 1))
                res->setMaxAge(parsedValue);
            else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (max-age)", cookie.ascii().data());
            break;
        }

        case 'C' :
        case 'c' : {
            if (length >= 7 && ((cookie.find("omment", tokenStartSvg + 1, false) - tokenStartSvg) == 1))
                // We do not have room for the comment part (and so do Mozilla) so just log the comment.
                LOG(Network, "Comment %s for ParsedCookie : %s\n", parsedValue.ascii().data(), cookie.ascii().data());
            else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (comment)", cookie.ascii().data());
            break;
        }

        case 'V' :
        case 'v' : {
            if (length >= 7 && ((cookie.find("ersion", tokenStartSvg + 1, false) - tokenStartSvg) == 1)) {
                // Although the out-of-dated Cookie Spec(RFC2965, http://tools.ietf.org/html/rfc2965) defined
                // the value of version can only contain DIGIT, some random sites, e.g. https://devforums.apple.com
                // would use double quotation marks to quote the digit. So we need to get rid of them for compliance.
                if (parsedValue.length() > 1 && parsedValue[0] == '"' && parsedValue[parsedValue.length() - 1] == '"')
                    parsedValue = parsedValue.substring(1, parsedValue.length() - 2);

                if (parsedValue.toInt() != 1)
                    LOG_ERROR_AND_RETURN("ParsedCookie version %d not supported (only support version=1)", parsedValue.toInt());
            } else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (version)", cookie.ascii().data());
            break;
        }

        case 'S' :
        case 's' : {
            // Secure is a standalone token ("Secure;")
            if (length >= 6 && ((cookie.find("ecure", tokenStartSvg + 1, false) - tokenStartSvg) == 1))
                res->setSecureFlag(true);
            else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (secure)", cookie.ascii().data());
            break;
        }
        case 'H':
        case 'h': {
            // HttpOnly is a standalone token ("HttpOnly;")
            if (length >= 8 && ((cookie.find("ttpOnly", tokenStartSvg + 1, false) - tokenStartSvg) == 1))
                res->setIsHttpOnly(true);
            else
                LOG_ERROR_AND_RETURN("Invalid cookie attribute %s (HttpOnly)", cookie.ascii().data());
            break;
        }

        default : {
            // If length == 0, we should be at the end of the cookie (case : ";\r") so ignore it
            if (length)
                LOG_ERROR_AND_RETURN("Invalid token for cookie %s", cookie.ascii().data());
            break;
        }
        }
    }

    // Check if the cookie is valid with respect to the size limit.
    if (res->isOverSizeLimit())
        LOG_ERROR_AND_RETURN("ParsedCookie %s is above the 4kb in length : REJECTED", cookie.ascii().data());

    // If no domain was provided, set it to the host
    if (!res->domain())
        res->setDomain(url.host());

    // According to the Cookie Specificaiton (RFC6265, section 4.1.2.4 and 5.2.4, http://tools.ietf.org/html/rfc6265),
    // If no path was provided or the first character of the path value is not '/', set it to the host's path
    //
    // REFERENCE
    // 4.1.2.4. The Path Attribute
    //
    // The scope of each cookie is limited to a set of paths, controlled by
    // the Path attribute. If the server omits the Path attribute, the user
    // agent will use the "directory" of the request-uri's path component as
    // the default value. (See Section 5.1.4 for more details.)
    // ...........
    // 5.2.4. The Path Attribute
    //
    // If the attribute-name case-insensitively matches the string "Path",
    // the user agent MUST process the cookie-av as follows.
    //
    // If the attribute-value is empty or if the first character of the
    // attribute-value is not %x2F ("/"):
    //
    // Let cookie-path be the default-path.
    //
    // Otherwise:
    //
    // Let cookie-path be the attribute-value.
    //
    // Append an attribute to the cookie-attribute-list with an attribute-
    // name of Path and an attribute-value of cookie-path.
    if (!res->path() || !res->path().length() || !res->path().startsWith("/", false)) {
        String path = url.string().substring(url.pathStart(), url.pathAfterLastSlash() - url.pathStart() - 1);
        if (path.isEmpty())
            path = "/";
        // Since this is reading the raw url string, it could contain percent-encoded sequences. We
        // want it to be comparable to the return value of url.path(), which is not percent-encoded,
        // so we must remove the escape sequences.
        res->setPath(curl_unescape(path.utf8().data(), path.length()));
    }

    return res;
}

} // namespace WebCore
