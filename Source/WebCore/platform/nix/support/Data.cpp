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
#include <public/Data.h>

#include "SharedBuffer.h"

namespace Nix {

class Data::DataPrivate : public WebCore::SharedBuffer {
};

void Data::reset()
{
    if (m_private) {
        m_private->deref();
        m_private = 0;
    }
}

void Data::assign(const Data& other)
{
    DataPrivate* p = const_cast<DataPrivate*>(other.m_private);
    if (p)
        p->ref();
    assign(p);
}

void Data::assign(const char* data, size_t size)
{
    assign(static_cast<DataPrivate*>(
        WebCore::SharedBuffer::create(data, size).leakRef()));
}

size_t Data::size() const
{
    if (!m_private)
        return 0;
    return const_cast<DataPrivate*>(m_private)->size();
}

const char* Data::data() const
{
    if (!m_private)
        return 0;
    return const_cast<DataPrivate*>(m_private)->data();
}

Data::Data(const PassRefPtr<WebCore::SharedBuffer>& buffer)
    : m_private(static_cast<DataPrivate*>(buffer.leakRef()))
{
}

Data& Data::operator=(const PassRefPtr<WebCore::SharedBuffer>& buffer)
{
    assign(static_cast<DataPrivate*>(buffer.leakRef()));
    return *this;
}

Data::operator PassRefPtr<WebCore::SharedBuffer>() const
{
    return PassRefPtr<WebCore::SharedBuffer>(const_cast<DataPrivate*>(m_private));
}

void Data::assign(DataPrivate* p)
{
    // p is already ref'd for us by the caller
    if (m_private)
        m_private->deref();
    m_private = p;
}

} // namespace Nix
