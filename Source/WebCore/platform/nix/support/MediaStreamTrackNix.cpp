/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#if ENABLE(MEDIA_STREAM)

#include <public/MediaStream.h>
#include <public/MediaStreamSource.h>
#include <public/MediaStreamTrack.h>
#include <public/String.h>

#include <wtf/Vector.h>

#include "MediaStreamComponent.h"

namespace Nix {

MediaStreamTrack::MediaStreamTrack(PassRefPtr<WebCore::MediaStreamComponent> mediaStreamComponent)
    : m_private(mediaStreamComponent)
{
}

MediaStreamTrack::MediaStreamTrack(WebCore::MediaStreamComponent* mediaStreamComponent)
    : m_private(mediaStreamComponent)
{
}

MediaStreamTrack& MediaStreamTrack::operator=(WebCore::MediaStreamComponent* mediaStreamComponent)
{
    m_private = mediaStreamComponent;
    return *this;
}

void MediaStreamTrack::initialize(const MediaStreamSource& source)
{
    m_private = WebCore::MediaStreamComponent::create(source);
}

void MediaStreamTrack::initialize(const String& id, const MediaStreamSource& source)
{
    m_private = WebCore::MediaStreamComponent::create(id, source);
}

void MediaStreamTrack::reset()
{
    m_private.reset();
}

MediaStreamTrack::operator PassRefPtr<WebCore::MediaStreamComponent>() const
{
    return m_private.get();
}

MediaStreamTrack::operator WebCore::MediaStreamComponent*() const
{
    return m_private.get();
}

bool MediaStreamTrack::isEnabled() const
{
    ASSERT(!m_private.isNull());
    return m_private->enabled();
}

String MediaStreamTrack::id() const
{
    ASSERT(!m_private.isNull());
    return m_private->id();
}

MediaStream MediaStreamTrack::stream() const
{
    ASSERT(!m_private.isNull());
    return MediaStream(m_private->stream());
}

MediaStreamSource MediaStreamTrack::source() const
{
    ASSERT(!m_private.isNull());
    return MediaStreamSource(m_private->source());
}

void MediaStreamTrack::assign(const MediaStreamTrack& other)
{
    m_private = other.m_private;
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)
