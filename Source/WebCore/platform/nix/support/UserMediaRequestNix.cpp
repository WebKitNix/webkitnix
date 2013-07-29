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

#include "config.h"

#include <public/MediaConstraints.h>
#include <public/MediaStream.h>
#include <public/SecurityOrigin.h>
#include <public/UserMediaRequest.h>

#include "MediaConstraints.h"
#include "MediaStreamDescriptor.h"
#include "ScriptExecutionContext.h"
#include "SecurityOrigin.h"
#include "UserMediaRequest.h"

namespace Nix {

UserMediaRequest::UserMediaRequest(const PassRefPtr<WebCore::UserMediaRequest>& request)
    : m_private(request)
{
}

UserMediaRequest::UserMediaRequest(WebCore::UserMediaRequest* request)
    : m_private(request)
{
}

void UserMediaRequest::reset()
{
    m_private.reset();
}

bool UserMediaRequest::audio() const
{
    ASSERT(!isNull());
    return m_private->audio();
}

bool UserMediaRequest::video() const
{
    ASSERT(!isNull());
    return m_private->video();
}

MediaConstraints UserMediaRequest::audioConstraints() const
{
    ASSERT(!isNull());
    return m_private->audioConstraints();
}

MediaConstraints UserMediaRequest::videoConstraints() const
{
    ASSERT(!isNull());
    return m_private->videoConstraints();
}

SecurityOrigin UserMediaRequest::securityOrigin() const
{
    ASSERT(!isNull() && m_private->scriptExecutionContext());
    return SecurityOrigin(m_private->scriptExecutionContext()->securityOrigin());
}

// WebDocument UserMediaRequest::ownerDocument() const
// {
//     ASSERT(!isNull());
//     return WebDocument(m_private->ownerDocument());
// }

void UserMediaRequest::requestSucceeded(const MediaStream& streamDescriptor)
{
    ASSERT(!isNull() && !streamDescriptor.isNull());
    m_private->succeed(streamDescriptor);
}

void UserMediaRequest::requestFailed()
{
    ASSERT(!isNull());
    m_private->fail();
}


bool UserMediaRequest::equals(const UserMediaRequest& other) const
{
    if (isNull() || other.isNull())
        return false;
    return m_private.get() == other.m_private.get();
}

void UserMediaRequest::assign(const UserMediaRequest& other)
{
    m_private = other.m_private;
}

UserMediaRequest::operator WebCore::UserMediaRequest*() const
{
    return m_private.get();
}

} // namespace Nix
