/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef Nix_CString_h
#define Nix_CString_h

#include "Common.h"

#if BUILDING_NIX__
#include <wtf/Forward.h>
#else
#include <string>
#endif

namespace WTF {
class CString;
}

namespace Nix {

class CStringPrivate;
class String;

// A single-byte string container with unspecified encoding.  It is
// inexpensive to copy a Nix::CString object.
//
// WARNING: It is not safe to pass a Nix::CString across threads!!!
//
class CString {
public:
    ~CString() { reset(); }

    CString() : m_private(0) { }

    CString(const char* data, size_t len) : m_private(0)
    {
        assign(data, len);
    }

    CString(const Nix::CString& s) : m_private(0) { assign(s); }

    Nix::CString& operator=(const Nix::CString& s)
    {
        assign(s);
        return *this;
    }

    // Returns 0 if both strings are equals, a value greater than zero if the
    // first character that does not match has a greater value in this string
    // than in |other|, or a value less than zero to indicate the opposite.
    WEBKIT_EXPORT int compare(const Nix::CString& other) const;

    WEBKIT_EXPORT void reset();
    WEBKIT_EXPORT void assign(const Nix::CString&);
    WEBKIT_EXPORT void assign(const char* data, size_t len);

    WEBKIT_EXPORT size_t length() const;
    WEBKIT_EXPORT const char* data() const;

    bool isEmpty() const { return !length(); }
    bool isNull() const { return !m_private; }

    WEBKIT_EXPORT Nix::String utf16() const;

#if BUILDING_NIX__
    CString(const WTF::CString&);
    Nix::CString& operator=(const WTF::CString&);
    operator WTF::CString() const;
#else
    CString(const std::string& s) : m_private(0)
    {
        assign(s.data(), s.length());
    }

    Nix::CString& operator=(const std::string& s)
    {
        assign(s.data(), s.length());
        return *this;
    }

    operator std::string() const
    {
        size_t len = length();
        return len ? std::string(data(), len) : std::string();
    }

    template <class UTF16String>
    static Nix::CString fromUTF16(const UTF16String& s)
    {
        return fromUTF16(s.data(), s.length());
    }
#endif

private:
    void assign(CStringPrivate*);
    CStringPrivate* m_private;
};

inline bool operator<(const Nix::CString& a, const Nix::CString& b)
{
    return a.compare(b) < 0;
}

} // namespace Nix

#endif
