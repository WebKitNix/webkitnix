/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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
#include <public/CString.h>
#include <public/String.h>
#include <wtf/text/CString.h>

namespace Nix {

class CStringPrivate : public WTF::CStringBuffer {
};

int CString::compare(const Nix::CString& other) const
{
    // A null string is always less than a non null one.
    if (isNull() != other.isNull())
        return isNull() ? -1 : 1;

    if (isNull())
        return 0; // Both Nix::Strings are null.

    return strcmp(m_private->data(), other.m_private->data());
}

void CString::reset()
{
    if (m_private) {
        m_private->deref();
        m_private = 0;
    }
}

void CString::assign(const Nix::CString& other)
{
    assign(const_cast<CStringPrivate*>(other.m_private));
}

void CString::assign(const char* data, size_t length)
{
    char* newData;
    RefPtr<WTF::CStringBuffer> buffer =
        WTF::CString::newUninitialized(length, newData).buffer();
    memcpy(newData, data, length);
    assign(static_cast<CStringPrivate*>(buffer.get()));
}

size_t CString::length() const
{
    if (!m_private)
        return 0;
    return const_cast<CStringPrivate*>(m_private)->length();
}

const char* CString::data() const
{
    if (!m_private)
        return 0;
    return const_cast<CStringPrivate*>(m_private)->data();
}

Nix::String CString::utf16() const
{
    return Nix::String::fromUTF8(data(), length());
}

CString::CString(const WTF::CString& s)
    : m_private(static_cast<CStringPrivate*>(s.buffer()))
{
    if (m_private)
        m_private->ref();
}

Nix::CString& CString::operator=(const WTF::CString& s)
{
    assign(static_cast<CStringPrivate*>(s.buffer()));
    return *this;
}

CString::operator WTF::CString() const
{
    return m_private;
}

void CString::assign(CStringPrivate* p)
{
    // Take care to handle the case where m_private == p
    if (p)
        p->ref();
    if (m_private)
        m_private->deref();
    m_private = p;
}

} // namespace Nix
