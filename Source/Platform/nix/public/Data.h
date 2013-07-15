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

#ifndef Nix_Data_h
#define Nix_Data_h

#include "Common.h"

#ifdef BUILDING_NIX__
namespace WebCore { class SharedBuffer; }
namespace WTF { template <typename T> class PassRefPtr; }
#endif

namespace Nix {

// A container for raw bytes. It is inexpensive to copy a Nix::Data object.
//
// WARNING: It is not safe to pass a Nix::Data across threads!!!
//
class Data {
public:
    ~Data() { reset(); }

    Data() : m_private(0) { }

    Data(const char* data, size_t size) : m_private(0)
    {
        assign(data, size);
    }

    template <int N>
    Data(const char (&data)[N]) : m_private(0)
    {
        assign(data, N - 1);
    }

    Data(const Data& d) : m_private(0) { assign(d); }

    Data& operator=(const Data& d)
    {
        assign(d);
        return *this;
    }

    WEBKIT_EXPORT void reset();
    WEBKIT_EXPORT void assign(const Data&);
    WEBKIT_EXPORT void assign(const char* data, size_t);

    WEBKIT_EXPORT size_t size() const;
    WEBKIT_EXPORT const char* data() const;

    bool isEmpty() const { return !size(); }
    bool isNull() const { return !m_private; }

#ifdef BUILDING_NIX__
    Data(const WTF::PassRefPtr<WebCore::SharedBuffer>&);
    Data& operator=(const WTF::PassRefPtr<WebCore::SharedBuffer>&);
    operator WTF::PassRefPtr<WebCore::SharedBuffer>() const;
#else
    template <class C>
    Data(const C& c) : m_private(0)
    {
        assign(c.data(), c.size());
    }

    template <class C>
    Data& operator=(const C& c)
    {
        assign(c.data(), c.size());
        return *this;
    }
#endif

private:
    class DataPrivate;
    void assign(DataPrivate*);
    DataPrivate* m_private;
};

} // namespace Nix

#endif
