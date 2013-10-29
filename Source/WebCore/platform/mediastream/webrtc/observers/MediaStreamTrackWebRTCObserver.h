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

#ifndef MediaStreamTrackWebRTCObserver_h
#define MediaStreamTrackWebRTCObserver_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "MediaStreamTrackPrivate.h"
#include "WebRTCUtils.h"
#include "libwebrtc.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class MediaStreamTrackWebRTCObserver : public RefCounted<MediaStreamTrackWebRTCObserver>, public webrtc::ObserverInterface {
public:
    MediaStreamTrackWebRTCObserver(webrtc::MediaStreamTrackInterface* webRTCTrack, PassRefPtr<MediaStreamTrackPrivate> track)
        : m_webRTCTrack(webRTCTrack)
        , m_track(track)
    { }

    virtual ~MediaStreamTrackWebRTCObserver() { }

    void OnChanged()
    {
        // The only properties that can change in a track in webrtc library are those below.
        m_track->setEnabled(m_webRTCTrack->enabled());
        m_track->setReadyState(WebRTCUtils::toWebKitReadyState(m_webRTCTrack->state()));
    };

    webrtc::MediaStreamTrackInterface* webRTCTrack() { return m_webRTCTrack.get(); }
    MediaStreamTrackPrivate* track() { return m_track.get(); }

private:
    talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> m_webRTCTrack;
    RefPtr<MediaStreamTrackPrivate> m_track;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // MediaStreamTrackWebRTCObserver_h
