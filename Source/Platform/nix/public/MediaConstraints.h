/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef Nix_MediaConstraints_h
#define Nix_MediaConstraints_h

#include "Common.h"
#include "PrivatePtr.h"
#include "String.h"
#include "Vector.h"

namespace WebCore {
struct MediaConstraint;
class MediaConstraints;
}

namespace Nix {

struct MediaConstraint {
    MediaConstraint()
    {
    }

    MediaConstraint(String name, String value)
        : m_name(name)
        , m_value(value)
    {
    }

#ifdef BUILDING_NIX__
    MediaConstraint(const WebCore::MediaConstraint&);
#endif

    String m_name;
    String m_value;
};

class MediaConstraints {
public:
    MediaConstraints() { }
    MediaConstraints(const MediaConstraints& other) { assign(other); }
    ~MediaConstraints() { reset(); }

    MediaConstraints& operator=(const MediaConstraints& other)
    {
        assign(other);
        return *this;
    }

    WEBKIT_EXPORT void assign(const MediaConstraints&);

    WEBKIT_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }

    WEBKIT_EXPORT void getMandatoryConstraints(Vector<MediaConstraint>&) const;
    WEBKIT_EXPORT void getOptionalConstraints(Vector<MediaConstraint>&) const;

    WEBKIT_EXPORT bool getMandatoryConstraintValue(const String& name, String& value) const;
    WEBKIT_EXPORT bool getOptionalConstraintValue(const String& name, String& value) const;

#ifdef BUILDING_NIX__
    MediaConstraints(const WTF::PassRefPtr<WebCore::MediaConstraints>&);
    MediaConstraints(WebCore::MediaConstraints*);
#endif

private:
    PrivatePtr<WebCore::MediaConstraints> m_private;
};

} // namespace Nix

#endif // Nix_MediaConstraints_h
