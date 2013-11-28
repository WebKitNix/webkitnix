/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "MediaStreamWebRTCObserver.h"

#include "MediaStreamAudioSource.h"
#include "MediaStreamPrivate.h"
#include "MediaStreamTrackWebRTCObserver.h"

namespace WebCore {

MediaStreamWebRTCObserver::MediaStreamWebRTCObserver(webrtc::MediaStreamInterface* stream, MediaStreamPrivate* privateStream, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>& audioTracks, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>& videoTracks)
    : m_stream(stream)
    , m_private(privateStream)
    , m_audioTrackObservers(audioTracks)
    , m_videoTrackObservers(videoTracks)
{
}

void MediaStreamWebRTCObserver::OnChanged()
{
    // NOTE: webrtc only notifies that something with the number of tracks has changed,
    // it does not tell whether a track has been removed or added, neither tell the id of
    // the track. So we need to iterate through every tracks we have and check if a addition
    // or removal has take place.

    // The only changes in MediaStreamInterface that can fire events are in adding or removing tracks.
    const webrtc::AudioTrackVector& audioTracks = m_stream->GetAudioTracks();
    if (m_private->numberOfAudioTracks() < audioTracks.size())
        findAndAddTrack(audioTracks, m_audioTrackObservers);
    else if (m_private->numberOfAudioTracks() > audioTracks.size())
        findAndRemoveTrack(m_audioTrackObservers);

    // FIXME: Handle video.
}

template<typename T>
void MediaStreamWebRTCObserver::findAndAddTrack(const T& tracks, Vector<RefPtr<MediaStreamTrackWebRTCObserver>>& observers)
{
    for (const auto& webRTCTrack : tracks) {
        // FIXME: Handle video.
        if (webRTCTrack->kind() != "audio" || haveTrackObserver(webRTCTrack->id(), observers))
            continue;

        RefPtr<MediaStreamTrackPrivate> track = MediaStreamTrackPrivate::create(MediaStreamAudioSource::create());

        RefPtr<MediaStreamTrackWebRTCObserver> trackObserver = adoptRef(new MediaStreamTrackWebRTCObserver(webRTCTrack, track));
        webRTCTrack->RegisterObserver(trackObserver.get());
        observers.append(trackObserver);
        m_private->addRemoteTrack(track.get());
        return;
    }
}

void MediaStreamWebRTCObserver::findAndRemoveTrack(Vector<RefPtr<MediaStreamTrackWebRTCObserver>>& observers)
{
    for (unsigned i = 0; i < observers.size(); i++) {
        const std::string id = observers[i]->webRTCTrack()->id();
        MediaStreamTrackPrivate* track = observers[i]->track();
        if (track->type() == MediaStreamSource::Audio && m_stream->FindAudioTrack(id))
            continue;
        if (track->type() == MediaStreamSource::Video && m_stream->FindVideoTrack(id))
            continue;

        observers.remove(i);
        m_private->removeRemoteTrack(track);
        return;
    }
}

bool MediaStreamWebRTCObserver::haveTrackObserver(const std::string& id, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>& observers)
{
    for (const RefPtr<MediaStreamTrackWebRTCObserver>& observer : observers) {
        if (observer->webRTCTrack()->id() == id)
            return true;
    }
    return false;
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
