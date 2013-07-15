/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
#include <public/AudioBus.h>

#if ENABLE(WEB_AUDIO)
#include "AudioBus.h"
#else
namespace WebCore {
class AudioBus {
};
} // namespace WebCore
#endif

#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>

using namespace WebCore;

namespace Nix {

class AudioBusPrivate {
public:
    AudioBusPrivate(PassRefPtr<WebCore::AudioBus> audioBus)
        : m_audioBus(audioBus)
    { }

    RefPtr<WebCore::AudioBus> m_audioBus;
};

void AudioBus::initialize(unsigned numberOfChannels, size_t length, double sampleRate)
{
#if ENABLE(WEB_AUDIO)
    if (m_d)
        delete m_d;
    m_d = new AudioBusPrivate(WebCore::AudioBus::create(numberOfChannels, length));
    m_d->m_audioBus->setSampleRate(sampleRate);
#else
    ASSERT_NOT_REACHED();
#endif
}

void AudioBus::resizeSmaller(size_t newLength)
{
#if ENABLE(WEB_AUDIO)
    ASSERT(m_d);
    if (m_d) {
        ASSERT(newLength <= length());
        m_d->m_audioBus->resizeSmaller(newLength);
    }
#else
    ASSERT_NOT_REACHED();
#endif
}

void AudioBus::reset()
{
#if ENABLE(WEB_AUDIO)
    delete m_d;
    m_d = 0;
#else
    ASSERT_NOT_REACHED();
#endif
}

unsigned AudioBus::numberOfChannels() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_d)
        return 0;
    return m_d->m_audioBus->numberOfChannels();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

size_t AudioBus::length() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_d)
        return 0;
    return m_d->m_audioBus->length();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

double AudioBus::sampleRate() const
{
#if ENABLE(WEB_AUDIO)
    if (!m_d)
        return 0;
    return m_d->m_audioBus->sampleRate();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

float* AudioBus::channelData(unsigned channelIndex)
{
#if ENABLE(WEB_AUDIO)
    if (!m_d)
        return 0;
    ASSERT(channelIndex < numberOfChannels());
    return m_d->m_audioBus->channel(channelIndex)->mutableData();
#else
    ASSERT_NOT_REACHED();
    return 0;
#endif
}

PassRefPtr<WebCore::AudioBus> AudioBus::audioBus()
{
#if ENABLE(WEB_AUDIO)
    return m_d->m_audioBus;
#else
    ASSERT_NOT_REACHED();
    return nullptr;
#endif
}

} // namespace WebKit
