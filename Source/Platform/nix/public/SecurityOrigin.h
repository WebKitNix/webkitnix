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

#ifndef Nix_SecurityOrigin_h
#define Nix_SecurityOrigin_h

#include "Common.h"

#if BUILDING_NIX__
namespace WebCore { class SecurityOrigin; }
namespace WTF { template <typename T> class PassRefPtr; }
#endif

namespace Nix {

class SecurityOriginPrivate;
class String;
class URL;

class SecurityOrigin {
public:
    ~SecurityOrigin() { reset(); }

    SecurityOrigin() : m_private(0) { }
    SecurityOrigin(const SecurityOrigin& s) : m_private(0) { assign(s); }
    SecurityOrigin& operator=(const SecurityOrigin& s)
    {
        assign(s);
        return *this;
    }

    WEBKIT_EXPORT static SecurityOrigin createFromDatabaseIdentifier(const String& databaseIdentifier);
    WEBKIT_EXPORT static SecurityOrigin createFromString(const String&);
    WEBKIT_EXPORT static SecurityOrigin create(const URL&);

    WEBKIT_EXPORT void reset();
    WEBKIT_EXPORT void assign(const SecurityOrigin&);

    bool isNull() const { return !m_private; }

    WEBKIT_EXPORT String protocol() const;
    WEBKIT_EXPORT String host() const;
    WEBKIT_EXPORT unsigned short port() const;

    // A unique Nix::SecurityOrigin is the least privileged Nix::SecurityOrigin.
    WEBKIT_EXPORT bool isUnique() const;

    // Returns true if this Nix::SecurityOrigin can script objects in the given
    // SecurityOrigin. For example, call this function before allowing
    // script from one security origin to read or write objects from
    // another SecurityOrigin.
    WEBKIT_EXPORT bool canAccess(const SecurityOrigin&) const;

    // Returns true if this Nix::SecurityOrigin can read content retrieved from
    // the given URL. For example, call this function before allowing script
    // from a given security origin to receive contents from a given URL.
    WEBKIT_EXPORT bool canRequest(const URL&) const;

    // Returns a string representation of the Nix::SecurityOrigin.  The empty
    // Nix::SecurityOrigin is represented by "null".  The representation of a
    // non-empty Nix::SecurityOrigin resembles a standard URL.
    WEBKIT_EXPORT String toString() const;

    // Returns a string representation of this Nix::SecurityOrigin that can
    // be used as a file.  Should be used in storage APIs only.
    WEBKIT_EXPORT String databaseIdentifier() const;

    // Returns true if this Nix::SecurityOrigin can access usernames and 
    // passwords stored in password manager.
    WEBKIT_EXPORT bool canAccessPasswordManager() const;

    // Allows this Nix::SecurityOrigin access to local resources.
    WEBKIT_EXPORT void grantLoadLocalResources() const;

#if BUILDING_NIX__
    SecurityOrigin(const WTF::PassRefPtr<WebCore::SecurityOrigin>&);
    SecurityOrigin& operator=(const WTF::PassRefPtr<WebCore::SecurityOrigin>&);
    operator WTF::PassRefPtr<WebCore::SecurityOrigin>() const;
    WebCore::SecurityOrigin* get() const;
#endif

private:
    void assign(SecurityOriginPrivate*);
    SecurityOriginPrivate* m_private;
};

} // namespace Nix

#endif
