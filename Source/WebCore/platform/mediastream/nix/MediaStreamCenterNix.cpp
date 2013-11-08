/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include "MediaStreamCenterNix.h"

#include "MediaConstraints.h"
#include "MediaStreamCreationClient.h"
#include "MediaStreamPrivate.h"

#include <public/MediaConstraints.h>
#include <public/MediaStream.h>
#include <public/MediaStreamCenter.h>
#include <public/Platform.h>

#include <wtf/MainThread.h>

namespace WebCore {

MediaStreamCenter& MediaStreamCenter::platformCenter()
{
    ASSERT(isMainThread());
    DEFINE_STATIC_LOCAL(MediaStreamCenterNix, center, ());
    return center;
}

MediaStreamCenterNix::MediaStreamCenterNix()
    : m_client(Nix::Platform::current()->createMediaStreamCenter())
{
}

MediaStreamCenterNix::~MediaStreamCenterNix()
{
}

void MediaStreamCenterNix::validateRequestConstraints(PassRefPtr<MediaStreamCreationClient> creationClient, PassRefPtr<MediaConstraints> audioConstraints, PassRefPtr<MediaConstraints> videoConstraints)
{
    ASSERT(m_client);

    Nix::MediaConstraints nixAudioConstraints(audioConstraints);
    Nix::MediaConstraints nixVideoConstraints(videoConstraints);
    const char *failedConstraints = m_client->validateRequestConstraints(nixAudioConstraints, nixVideoConstraints);

    if (!failedConstraints)
        creationClient->constraintsValidated();
    else
        creationClient->constraintsInvalid(failedConstraints);
}

void MediaStreamCenterNix::createMediaStream(PassRefPtr<MediaStreamCreationClient> creationClient, PassRefPtr<MediaConstraints> audioConstraints, PassRefPtr<MediaConstraints> videoConstraints)
{
    Nix::MediaConstraints nixAudioConstraints(audioConstraints);
    Nix::MediaConstraints nixVideoConstraints(videoConstraints);
    Nix::MediaStream stream = m_client->createMediaStream(nixAudioConstraints, nixVideoConstraints);
    creationClient->didCreateStream(stream);
}

bool MediaStreamCenterNix::getMediaStreamTrackSources(PassRefPtr<MediaStreamTrackSourcesRequestClient>)
{
    return false;
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
