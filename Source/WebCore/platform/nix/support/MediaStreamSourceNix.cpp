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

#if ENABLE(MEDIA_STREAM)

#include <public/MediaStreamSource.h>

#include "MediaStreamSource.h"

#include <wtf/text/CString.h>

namespace Nix {

MediaStreamSource::MediaStreamSource(const PassRefPtr<WebCore::MediaStreamSource>& mediaStreamSource)
    : m_private(mediaStreamSource)
{
}

MediaStreamSource& MediaStreamSource::operator=(WebCore::MediaStreamSource* mediaStreamSource)
{
    m_private = mediaStreamSource;
    return *this;
}

void MediaStreamSource::assign(const MediaStreamSource& other)
{
    m_private = other.m_private;
}

void MediaStreamSource::reset()
{
    m_private.reset();
}

MediaStreamSource::operator PassRefPtr<WebCore::MediaStreamSource>() const
{
    return m_private.get();
}

MediaStreamSource::operator WebCore::MediaStreamSource*() const
{
    return m_private.get();
}

const char* MediaStreamSource::id() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->id().utf8().data();
}

MediaStreamSource::Type MediaStreamSource::type() const
{
    ASSERT(!m_private.isNull());
    return static_cast<Type>(m_private.get()->type());
}

const char* MediaStreamSource::name() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->name().utf8().data();
}

void MediaStreamSource::setReadyState(ReadyState state)
{
    ASSERT(!m_private.isNull());
    m_private->setReadyState(static_cast<WebCore::MediaStreamSource::ReadyState>(state));
}

MediaStreamSource::ReadyState MediaStreamSource::readyState() const
{
    ASSERT(!m_private.isNull());
    return static_cast<ReadyState>(m_private->readyState());
}

void MediaStreamSource::setEnabled(bool enabled)
{
    ASSERT(!m_private.isNull());
    m_private->setEnabled(enabled);
}

bool MediaStreamSource::enabled() const
{
    ASSERT(!m_private.isNull());
    return m_private->enabled();
}

void MediaStreamSource::setMuted(bool muted)
{
    ASSERT(!m_private.isNull());
    m_private->setMuted(muted);
}

bool MediaStreamSource::muted() const
{
    ASSERT(!m_private.isNull());
    return m_private->muted();
}

void MediaStreamSource::setReadonly(bool readonly)
{
    ASSERT(!m_private.isNull());
    m_private->setReadonly(readonly);
}

bool MediaStreamSource::readonly() const
{
    ASSERT(!m_private.isNull());
    return m_private->readonly();
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)

