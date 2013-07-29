/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef Nix_UserMediaRequest_h
#define Nix_UserMediaRequest_h

#include "SecurityOrigin.h"
#include "PrivatePtr.h"

namespace WebCore {
class UserMediaRequest;
}

namespace Nix {
// class Document;
class MediaConstraints;
class MediaStream;

class UserMediaRequest {
public:
    UserMediaRequest() { }
    UserMediaRequest(const UserMediaRequest& request) { assign(request); }
    ~UserMediaRequest() { reset(); }

    UserMediaRequest& operator=(const UserMediaRequest& other)
    {
        assign(other);
        return *this;
    }

    WEBKIT_EXPORT void reset();
    WEBKIT_EXPORT bool isNull() const { return m_private.isNull(); }
    WEBKIT_EXPORT bool equals(const UserMediaRequest&) const;
    WEBKIT_EXPORT void assign(const UserMediaRequest&);
    WEBKIT_EXPORT bool audio() const;
    WEBKIT_EXPORT bool video() const;
    WEBKIT_EXPORT MediaConstraints audioConstraints() const;
    WEBKIT_EXPORT MediaConstraints videoConstraints() const;
    WEBKIT_EXPORT SecurityOrigin securityOrigin() const;
    //FIXME: add support later, commented for now to minimize dependencies
    //       and keep focus on MediaStream API
    // WEBKIT_EXPORT Document ownerDocument() const;
    WEBKIT_EXPORT void requestSucceeded(const MediaStream&);
    WEBKIT_EXPORT void requestFailed();

#if BUILDING_NIX__
    UserMediaRequest(const PassRefPtr<WebCore::UserMediaRequest>&);
    UserMediaRequest(WebCore::UserMediaRequest*);
    operator WebCore::UserMediaRequest*() const;
#endif

private:
    PrivatePtr<WebCore::UserMediaRequest> m_private;
};

inline bool operator==(const UserMediaRequest& a, const UserMediaRequest& b)
{
    return a.equals(b);
}

} // namespace Nix

#endif // Nix_UserMediaRequest_h
