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

#include "config.h"
#include <public/String.h>
#include <public/CString.h>
#include <wtf/text/AtomicString.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

namespace Nix {

class StringPrivate : public WTF::StringImpl {
};

void String::reset()
{
    if (m_private) {
        m_private->deref();
        m_private = 0;
    }
}

void String::assign(const Nix::String& other)
{
    assign(const_cast<StringPrivate*>(other.m_private));
}

void String::assign(const UChar* data, size_t length)
{
    assign(static_cast<StringPrivate*>(
        WTF::StringImpl::create(data, length).get()));
}

size_t String::length() const
{
    return m_private ? const_cast<StringPrivate*>(m_private)->length() : 0;
}

const UChar* String::data() const
{
    return m_private ? const_cast<StringPrivate*>(m_private)->characters() : 0;
}

CString String::utf8() const
{
    return WTF::String(m_private).utf8();
}

Nix::String String::fromUTF8(const char* data, size_t length)
{
    return WTF::String::fromUTF8(data, length);
}

Nix::String String::fromUTF8(const char* data)
{
    return WTF::String::fromUTF8(data);
}

bool String::equals(const Nix::String& s) const
{
    return equal(m_private, s.m_private);
}

String::String(const WTF::String& s)
    : m_private(static_cast<StringPrivate*>(s.impl()))
{
    if (m_private)
        m_private->ref();
}

Nix::String& String::operator=(const WTF::String& s)
{
    assign(static_cast<StringPrivate*>(s.impl()));
    return *this;
}

String::operator WTF::String() const
{
    return m_private;
}

String::String(const WTF::AtomicString& s)
    : m_private(0)
{
    assign(s.string());
}

Nix::String& String::operator=(const WTF::AtomicString& s)
{
    assign(s.string());
    return *this;
}

String::operator WTF::AtomicString() const
{
    return WTF::AtomicString(static_cast<WTF::StringImpl *>(m_private));
}

void String::assign(StringPrivate* p)
{
    // Take care to handle the case where m_private == p
    if (p)
        p->ref();
    if (m_private)
        m_private->deref();
    m_private = p;
}

} // namespace Nix
