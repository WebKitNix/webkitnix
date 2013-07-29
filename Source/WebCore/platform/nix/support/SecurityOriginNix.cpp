/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <public/SecurityOrigin.h>
#include <public/String.h>
#include <public/URL.h>

#include <wtf/PassRefPtr.h>
#include "SecurityOrigin.h"
#include "KURL.h"

namespace Nix {

class SecurityOriginPrivate : public WebCore::SecurityOrigin {
};

SecurityOrigin SecurityOrigin::createFromDatabaseIdentifier(const String& databaseIdentifier)
{
    return SecurityOrigin(WebCore::SecurityOrigin::createFromDatabaseIdentifier(databaseIdentifier));
}

SecurityOrigin SecurityOrigin::createFromString(const String& origin)
{
    return SecurityOrigin(WebCore::SecurityOrigin::createFromString(origin));
}

SecurityOrigin SecurityOrigin::create(const URL& url)
{
    return SecurityOrigin(WebCore::SecurityOrigin::create(url));
}

void SecurityOrigin::reset()
{
    assign(0);
}

void SecurityOrigin::assign(const SecurityOrigin& other)
{
    SecurityOriginPrivate* p = const_cast<SecurityOriginPrivate*>(other.m_private);
    if (p)
        p->ref();
    assign(p);
}

String SecurityOrigin::protocol() const
{
    ASSERT(m_private);
    return m_private->protocol();
}

String SecurityOrigin::host() const
{
    ASSERT(m_private);
    return m_private->host();
}

unsigned short SecurityOrigin::port() const
{
    ASSERT(m_private);
    return m_private->port();
}

bool SecurityOrigin::isUnique() const
{
    ASSERT(m_private);
    return m_private->isUnique();
}

bool SecurityOrigin::canAccess(const SecurityOrigin& other) const
{
    ASSERT(m_private);
    ASSERT(other.m_private);
    return m_private->canAccess(other.m_private);
}

bool SecurityOrigin::canRequest(const URL& url) const
{
    ASSERT(m_private);
    return m_private->canRequest(url);
}

String SecurityOrigin::toString() const
{
    ASSERT(m_private);
    return m_private->toString();
}

String SecurityOrigin::databaseIdentifier() const
{
    ASSERT(m_private);
    return m_private->databaseIdentifier();
}

bool SecurityOrigin::canAccessPasswordManager() const
{
    ASSERT(m_private);
    return m_private->canAccessPasswordManager();
}

//FIXME Do we really need this?? 
SecurityOrigin::SecurityOrigin(const WTF::PassRefPtr<WebCore::SecurityOrigin>& origin)
    : m_private(static_cast<SecurityOriginPrivate*>(origin.leakRef()))
{
}

SecurityOrigin& SecurityOrigin::operator=(const WTF::PassRefPtr<WebCore::SecurityOrigin>& origin)
{
    assign(static_cast<SecurityOriginPrivate*>(origin.leakRef()));
    return *this;
}

SecurityOrigin::operator WTF::PassRefPtr<WebCore::SecurityOrigin>() const
{
    return PassRefPtr<WebCore::SecurityOrigin>(const_cast<SecurityOriginPrivate*>(m_private));
}

WebCore::SecurityOrigin* SecurityOrigin::get() const
{
    return m_private;
}

void SecurityOrigin::assign(SecurityOriginPrivate* p)
{
    // p is already ref'd for us by the caller
    if (m_private)
        m_private->deref();
    m_private = p;
}

void SecurityOrigin::grantLoadLocalResources() const
{
    get()->grantLoadLocalResources();
}

} // namespace Nix
