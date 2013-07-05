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

#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/Vector.h>

#include "MediaStreamComponent.h"
#include "MediaStreamDescriptor.h"
#include "MediaStreamSource.h"
#include "UUID.h"

namespace Nix {

namespace {

class ExtraDataContainer : public WebCore::MediaStreamDescriptor::ExtraData {
public:
    ExtraDataContainer(Nix::MediaStream::ExtraData* extraData) : m_extraData(adoptPtr(extraData)) { }

    Nix::MediaStream::ExtraData* extraData() { return m_extraData.get(); }

private:
    OwnPtr<Nix::MediaStream::ExtraData> m_extraData;
};

} // namespace

MediaStream::MediaStream(const PassRefPtr<WebCore::MediaStreamDescriptor>& mediaStreamDescriptor)
    : m_private(mediaStreamDescriptor)
{
}

MediaStream::MediaStream(WebCore::MediaStreamDescriptor* mediaStreamDescriptor)
    : m_private(mediaStreamDescriptor)
{
}

void MediaStream::reset()
{
    m_private.reset();
}

String MediaStream::label() const
{
    return m_private->id();
}

String MediaStream::id() const
{
    return m_private->id();
}

MediaStream::ExtraData* MediaStream::extraData() const
{
    RefPtr<WebCore::MediaStreamDescriptor::ExtraData> data = m_private->extraData();
    if (!data)
        return 0;
    return static_cast<ExtraDataContainer*>(data.get())->extraData();
}

void MediaStream::setExtraData(ExtraData* extraData)
{
    m_private->setExtraData(adoptRef(new ExtraDataContainer(extraData)));
}

void MediaStream::audioTracks(Vector<MediaStreamTrack>& webTracks) const
{
    size_t numberOfTracks = m_private->numberOfAudioComponents();
    Vector<MediaStreamTrack> result(numberOfTracks);
    for (size_t i = 0; i < numberOfTracks; ++i)
        result[i] = m_private->audioComponent(i);
    webTracks.swap(result);
}

void MediaStream::videoTracks(Vector<MediaStreamTrack>& webTracks) const
{
    size_t numberOfTracks = m_private->numberOfVideoComponents();
    Vector<MediaStreamTrack> result(numberOfTracks);
    for (size_t i = 0; i < numberOfTracks; ++i)
        result[i] = m_private->videoComponent(i);
    webTracks.swap(result);
}

void MediaStream::addTrack(const MediaStreamTrack& track)
{
    ASSERT(!isNull());
    //FIXME
    //m_private->addRemoteTrack(track);
}

void MediaStream::removeTrack(const MediaStreamTrack& track)
{
    ASSERT(!isNull());
    //FIXME
    //m_private->removeRemoteTrack(track);
}

MediaStream& MediaStream::operator=(const PassRefPtr<WebCore::MediaStreamDescriptor>& mediaStreamDescriptor)
{
    m_private = mediaStreamDescriptor;
    return *this;
}

MediaStream::operator PassRefPtr<WebCore::MediaStreamDescriptor>() const
{
    return m_private.get();
}

MediaStream::operator WebCore::MediaStreamDescriptor*() const
{
    return m_private.get();
}

void MediaStream::initialize(const String& label, const Vector<MediaStreamSource>& audioSources, const Vector<MediaStreamSource>& videoSources)
{
    WebCore::MediaStreamComponentVector audio, video;
    for (size_t i = 0; i < audioSources.size(); ++i) {
        WebCore::MediaStreamSource* source = audioSources[i];
        audio.append(WebCore::MediaStreamComponent::create(source->id(), source));
    }
    for (size_t i = 0; i < videoSources.size(); ++i) {
        WebCore::MediaStreamSource* source = videoSources[i];
        video.append(WebCore::MediaStreamComponent::create(source->id(), source));
    }
    m_private = WebCore::MediaStreamDescriptor::create(label, audio, video);
}

void MediaStream::initialize(const Vector<MediaStreamTrack>& audioTracks, const Vector<MediaStreamTrack>& videoTracks)
{
    initialize(WebCore::createCanonicalUUIDString(), audioTracks, videoTracks);
}

void MediaStream::initialize(const String& label, const Vector<MediaStreamTrack>& audioTracks, const Vector<MediaStreamTrack>& videoTracks)
{
    WebCore::MediaStreamComponentVector audio, video;
    for (size_t i = 0; i < audioTracks.size(); ++i) {
        WebCore::MediaStreamComponent* component = audioTracks[i];
        audio.append(component);
    }
    for (size_t i = 0; i < videoTracks.size(); ++i) {
        WebCore::MediaStreamComponent* component = videoTracks[i];
        video.append(component);
    }
    m_private = WebCore::MediaStreamDescriptor::create(label, audio, video);
}

void MediaStream::assign(const MediaStream& other)
{
    m_private = other.m_private;
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)
