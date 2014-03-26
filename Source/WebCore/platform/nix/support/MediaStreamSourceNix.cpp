/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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

#include <public/MediaStreamSource.h>

#include "MediaStreamSource.h"

#include <wtf/text/CString.h>

namespace Nix {

#if ENABLE(MEDIA_STREAM)
MediaStreamSource::MediaStreamSource(const PassRefPtr<WebCore::MediaStreamSource>& mediaStreamSource)
    : m_private(mediaStreamSource)
{
}

MediaStreamSource& MediaStreamSource::operator=(WebCore::MediaStreamSource* mediaStreamSource)
{
    m_private = mediaStreamSource;
    return *this;
}

MediaStreamSource::operator PassRefPtr<WebCore::MediaStreamSource>() const
{
    return m_private.get();
}

MediaStreamSource::operator WebCore::MediaStreamSource*() const
{
    return m_private.get();
}
#endif // ENABLE(MEDIA_STREAM)

MediaStreamSource::~MediaStreamSource()
{
#if ENABLE(MEDIA_STREAM)
    m_private.clear();
#endif
}

const char* MediaStreamSource::id() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return m_private->id().utf8().data();
#else
    return nullptr;
#endif
}

MediaStreamSource::Type MediaStreamSource::type() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return static_cast<Type>(m_private->type());
#else
    return Audio;
#endif
}

const char* MediaStreamSource::name() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return m_private->name().utf8().data();
#else
    return nullptr;
#endif
}

void MediaStreamSource::setReadyState(ReadyState state)
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    m_private->setReadyState(static_cast<WebCore::MediaStreamSource::ReadyState>(state));
#else
    UNUSED_PARAM(state);
#endif
}

MediaStreamSource::ReadyState MediaStreamSource::readyState() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return static_cast<ReadyState>(m_private->readyState());
#else
    return ReadyStateEnded;
#endif
}

void MediaStreamSource::setEnabled(bool enabled)
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    m_private->setEnabled(enabled);
#else
    UNUSED_PARAM(enabled);
#endif
}

bool MediaStreamSource::enabled() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return m_private->enabled();
#else
    return false;
#endif
}

void MediaStreamSource::setMuted(bool muted)
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    m_private->setMuted(muted);
#else
    UNUSED_PARAM(muted);
#endif
}

bool MediaStreamSource::muted() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return m_private->muted();
#else
    return true;
#endif
}

void MediaStreamSource::setReadonly(bool readonly)
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    m_private->setReadonly(readonly);
#else
    UNUSED_PARAM(readonly);
#endif
}

bool MediaStreamSource::readonly() const
{
#if ENABLE(MEDIA_STREAM)
    ASSERT(!isNull());
    return m_private->readonly();
#else
    return true;
#endif
}

bool MediaStreamSource::isNull() const
{
    return !m_private;
}

} // namespace Nix
