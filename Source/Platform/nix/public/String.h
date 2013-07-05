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

#ifndef Nix_String_h_x
#define Nix_String_h_x

#include "Common.h"

#if BUILDING_NIX__
#include <wtf/Forward.h>
#else
#include <base/nullable_string16.h>
#include <base/string16.h>
#endif

namespace Nix {

class CString;
class StringPrivate;

// A UTF-16 string container.  It is inexpensive to copy a Nix::String
// object.
//
// WARNING: It is not safe to pass a Nix::String across threads!!!
//
class String {
public:
    ~String() { reset(); }

    String() : m_private(0) { }

    String(const UChar* data, size_t len) : m_private(0)
    {
        assign(data, len);
    }

    String(const Nix::String& s) : m_private(0) { assign(s); }

    String& operator=(const Nix::String& s)
    {
        assign(s);
        return *this;
    }

    WEBKIT_EXPORT void reset();
    WEBKIT_EXPORT void assign(const Nix::String&);
    WEBKIT_EXPORT void assign(const UChar* data, size_t len);

    WEBKIT_EXPORT bool equals(const Nix::String& s) const;

    WEBKIT_EXPORT size_t length() const;
    WEBKIT_EXPORT const UChar* data() const;

    bool isEmpty() const { return !length(); }
    bool isNull() const { return !m_private; }

    WEBKIT_EXPORT CString utf8() const;

    WEBKIT_EXPORT static Nix::String fromUTF8(const char* data, size_t length);
    WEBKIT_EXPORT static Nix::String fromUTF8(const char* data);

    template <int N> String(const char (&data)[N])
        : m_private(0)
    {
        assign(fromUTF8(data, N - 1));
    }

    template <int N> Nix::String& operator=(const char (&data)[N])
    {
        assign(fromUTF8(data, N - 1));
        return *this;
    }

#if BUILDING_NIX__
    String(const WTF::String&);
    Nix::String& operator=(const WTF::String&);
    operator WTF::String() const;

    String(const WTF::AtomicString&);
    Nix::String& operator=(const WTF::AtomicString&);
    operator WTF::AtomicString() const;
#else

    String(const string16& s) : m_private(0)
    {
        assign(s.data(), s.length());
    }

    Nix::String& operator=(const string16& s)
    {
        assign(s.data(), s.length());
        return *this;
    }

    operator string16() const
    {
        size_t len = length();
        return len ? string16(data(), len) : string16();
    }

    String(const NullableString16& s) : m_private(0)
    {
        if (s.is_null())
            reset();
        else
            assign(s.string().data(), s.string().length());
    }

    Nix::String& operator=(const NullableString16& s)
    {
        if (s.is_null())
            reset();
        else
            assign(s.string().data(), s.string().length());
        return *this;
    }

    operator NullableString16() const
    {
        if (!m_private)
            return NullableString16(string16(), true);
        size_t len = length();
        return NullableString16(len ? string16(data(), len) : string16(), false);
    }

    template <class UTF8String>
    static Nix::String fromUTF8(const UTF8String& s)
    {
        return fromUTF8(s.data(), s.length());
    }
#endif

private:
    void assign(StringPrivate*);
    StringPrivate* m_private;
};

inline bool operator==(const Nix::String& a, const Nix::String& b)
{
    return a.equals(b);
}

inline bool operator!=(const Nix::String& a, const Nix::String& b)
{
    return !(a == b);
}

} // namespace Nix

#endif
