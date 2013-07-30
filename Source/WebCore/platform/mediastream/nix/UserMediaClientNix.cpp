/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * FIXME ?
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
#if ENABLE(MEDIA_STREAM)

#include "UserMediaClientNix.h"

#include <wtf/RefPtr.h>
#include <public/MediaStreamSource.h>
#include <public/UserMediaClient.h>
#include <public/UserMediaRequest.h>

#include <public/Platform.h>

namespace WebKit {

UserMediaClientNix::UserMediaClientNix()
    : m_client(Nix::Platform::current()->createUserMediaClient())
{
}

void UserMediaClientNix::pageDestroyed()
{
    if (m_client)
        m_client->pageDestroyed();
    delete this;
}

static Nix::Vector<Nix::MediaStreamSource> toNixMediaStreamSourceVector(const WebCore::MediaStreamSourceVector& sourceVector)
{
    Nix::Vector<Nix::MediaStreamSource> sources(sourceVector.size());
    for (size_t i = 0; i < sourceVector.size(); ++i)
        sources[i] = Nix::MediaStreamSource(sourceVector[i]);
    return sources;
}

void UserMediaClientNix::requestUserMedia(PassRefPtr<WebCore::UserMediaRequest> prpRequest, const WebCore::MediaStreamSourceVector& audioSources, const WebCore::MediaStreamSourceVector& videoSources)
{
    RefPtr<WebCore::UserMediaRequest> request = prpRequest;
    if (m_client) {
        const Nix::Vector<Nix::MediaStreamSource> nixAudioSources = toNixMediaStreamSourceVector(audioSources);
        const Nix::Vector<Nix::MediaStreamSource> nixVideoSources = toNixMediaStreamSourceVector(videoSources);
        Nix::UserMediaRequest nixUserMediaRequest = Nix::UserMediaRequest(request.get());
        m_client->requestUserMedia(nixUserMediaRequest, nixAudioSources, nixVideoSources);
    }
    request->succeed(audioSources, videoSources);
}


void UserMediaClientNix::cancelUserMediaRequest(WebCore::UserMediaRequest* request)
{
    if (m_client) {
        Nix::UserMediaRequest nixUserMediaRequest = Nix::UserMediaRequest(request);
        m_client->cancelUserMediaRequest(nixUserMediaRequest);
    }
}

} // namespace WebKit

#endif // ENABLE(MEDIA_STREAM)
