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

#if ENABLE(MEDIA_STREAM)

#include <public/AudioDestinationConsumer.h>
#include <public/MediaStreamSource.h>
#include <public/String.h>

#include <wtf/MainThread.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/Vector.h>

#include "AudioBus.h"
#include "MediaStreamSource.h"

namespace Nix {

namespace {

class ExtraDataContainer : public WebCore::MediaStreamSource::ExtraData {
public:
    ExtraDataContainer(Nix::MediaStreamSource::ExtraData* extraData) : m_extraData(adoptPtr(extraData)) { }

    Nix::MediaStreamSource::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<Nix::MediaStreamSource::ExtraData> m_extraData;
};

} // namespace

MediaStreamSource MediaStreamSource::ExtraData::owner()
{
    ASSERT(m_owner);
    return MediaStreamSource(m_owner);
}

void MediaStreamSource::ExtraData::setOwner(WebCore::MediaStreamSource* owner)
{
    ASSERT(!m_owner);
    m_owner = owner;
}

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

void MediaStreamSource::initialize(const String& id, Type type, const String& name)
{
    m_private = WebCore::MediaStreamSource::create(id, static_cast<WebCore::MediaStreamSource::Type>(type), name);
}

String MediaStreamSource::id() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->id();
}

MediaStreamSource::Type MediaStreamSource::type() const
{
    ASSERT(!m_private.isNull());
    return static_cast<Type>(m_private.get()->type());
}

String MediaStreamSource::name() const
{
    ASSERT(!m_private.isNull());
    return m_private.get()->name();
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

MediaStreamSource::ExtraData* MediaStreamSource::extraData() const
{
    ASSERT(!m_private.isNull());
    WebCore::MediaStreamSource::ExtraData* data = m_private->extraData().get();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data)->extraData();
}

void MediaStreamSource::setExtraData(ExtraData* extraData)
{
    ASSERT(!m_private.isNull());

    if (extraData)
        extraData->setOwner(m_private.get());

    m_private->setExtraData(new ExtraDataContainer(extraData));
}

String MediaStreamSource::deviceId() const
{
    ASSERT(!m_private.isNull());
    return m_private->deviceId();
}

void MediaStreamSource::setDeviceId(const String& deviceId)
{
    ASSERT(!m_private.isNull());
    m_private->setDeviceId(deviceId);
}

bool MediaStreamSource::requiresAudioConsumer() const
{
    ASSERT(!m_private.isNull());
    return m_private->requiresAudioConsumer();
}

class ConsumerWrapper : public WebCore::AudioDestinationConsumer {
public:
    static PassRefPtr<ConsumerWrapper> create(Nix::AudioDestinationConsumer* consumer)
    {
        return adoptRef(new ConsumerWrapper(consumer));
    }

    virtual void setFormat(size_t numberOfChannels, float sampleRate) OVERRIDE;
    virtual void consumeAudio(WebCore::AudioBus*, size_t numberOfFrames) OVERRIDE;

    Nix::AudioDestinationConsumer* consumer() { return m_consumer; }

private:
    explicit ConsumerWrapper(Nix::AudioDestinationConsumer* consumer) : m_consumer(consumer) { }

    // m_consumer is not owned by this class.
    Nix::AudioDestinationConsumer* m_consumer;
};

void ConsumerWrapper::setFormat(size_t numberOfChannels, float sampleRate)
{
    m_consumer->setFormat(numberOfChannels, sampleRate);
}

void ConsumerWrapper::consumeAudio(WebCore::AudioBus* bus, size_t numberOfFrames)
{
    if (!bus)
        return;

    // Wrap AudioBus.
    size_t numberOfChannels = bus->numberOfChannels();
    Vector<const float*> busVector(numberOfChannels);
    for (size_t i = 0; i < numberOfChannels; ++i)
        busVector[i] = bus->channel(i)->data();

    m_consumer->consumeAudio(busVector, numberOfFrames);
}

void MediaStreamSource::addAudioConsumer(AudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    m_private->addAudioConsumer(ConsumerWrapper::create(consumer));
}

bool MediaStreamSource::removeAudioConsumer(AudioDestinationConsumer* consumer)
{
    ASSERT(isMainThread());
    ASSERT(!m_private.isNull() && consumer);

    const WTF::Vector<RefPtr<WebCore::AudioDestinationConsumer> >& consumers = m_private->audioConsumers();
    for (WTF::Vector<RefPtr<WebCore::AudioDestinationConsumer> >::const_iterator it = consumers.begin(); it != consumers.end(); ++it) {
        ConsumerWrapper* wrapper = static_cast<ConsumerWrapper*>((*it).get());
        if (wrapper->consumer() == consumer) {
            m_private->removeAudioConsumer(wrapper);
            return true;
        }
    }
    return false;
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)

