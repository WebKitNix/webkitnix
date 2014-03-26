/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <public/MediaStream.h>

#include "MediaStreamAudioSource.h"
#include "MediaStreamPrivate.h"
#include "MediaStreamSource.h"

#include <public/MediaStreamAudioSource.h>
#include <public/MediaStreamSource.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>


using namespace WebCore;

namespace Nix {

#if ENABLE(MEDIA_STREAM)
MediaStream::MediaStream(const PassRefPtr<WebCore::MediaStreamPrivate>& mediaStreamDescriptor)
    : m_private(mediaStreamDescriptor)
{
}

MediaStream::MediaStream(WebCore::MediaStreamPrivate* mediaStreamDescriptor)
    : m_private(mediaStreamDescriptor)
{
}

MediaStream& MediaStream::operator=(const PassRefPtr<WebCore::MediaStreamPrivate>& mediaStreamDescriptor)
{
    m_private = mediaStreamDescriptor;
    return *this;
}

MediaStream::operator PassRefPtr<WebCore::MediaStreamPrivate>() const
{
    return m_private.get();
}

MediaStream::operator WebCore::MediaStreamPrivate*() const
{
    return m_private.get();
}
#endif // ENABLE(MEDIA_STREAM)

MediaStream::MediaStream()
{
}

MediaStream::~MediaStream()
{
#if ENABLE(MEDIA_STREAM)
    m_private.clear();
#endif
}

const char* MediaStream::id() const
{
#if ENABLE(MEDIA_STREAM)
    return m_private->id().utf8().data();
#else
    return nullptr;
#endif
}

std::vector<MediaStreamSource*> MediaStream::audioSources() const
{
#if ENABLE(MEDIA_STREAM)
    size_t numberOfSources = m_private->numberOfAudioSources();
    std::vector<MediaStreamSource*> result(numberOfSources);
    for (size_t i = 0; i < numberOfSources; ++i)
        result[i] = new MediaStreamAudioSource(dynamic_cast<WebCore::MediaStreamAudioSource*>(m_private->audioSources(i)));
    return result;
#else
    return std::vector<MediaStreamSource*>();
#endif
}

std::vector<MediaStreamSource*> MediaStream::videoSources() const
{
    // FIXME Add support for video sources.
    return std::vector<MediaStreamSource*>();
}

void MediaStream::addSource(const MediaStreamSource&)
{
    ASSERT(!isNull());
    // FIXME: Set a MediaStreamPrivateClient when initialize.
}

void MediaStream::removeSource(const MediaStreamSource&)
{
    ASSERT(!isNull());
    // FIXME: Set a MediaStreamPrivateClient when initialize.
}

void MediaStream::initialize(std::vector<MediaStreamSource*>& audioSources, std::vector<MediaStreamSource*>& videoSources)
{
#if ENABLE(MEDIA_STREAM)
    Vector<RefPtr<WebCore::MediaStreamSource>> audio, video;
    for (size_t i = 0; i < audioSources.size(); ++i) {
        WebCore::MediaStreamAudioSource* source = *(dynamic_cast<MediaStreamAudioSource*>(audioSources[i]));
        audio.append(source);
        delete audioSources[i];
    }
    // FIXME: Add support for MediaStreamVideoSource when it's available.
    for (size_t i = 0; i < videoSources.size(); ++i) {
        WebCore::MediaStreamSource* source = *(videoSources[i]);
        video.append(source);
        delete videoSources[i];
    }
    m_private = WebCore::MediaStreamPrivate::create(audio, video);
#else
    UNUSED_PARAM(audioSources);
    UNUSED_PARAM(videoSources);
#endif
}

bool MediaStream::isNull() const
{
#if ENABLE(MEDIA_STREAM)
    return !m_private;
#else
    return true;
#endif
}

} // namespace Nix
