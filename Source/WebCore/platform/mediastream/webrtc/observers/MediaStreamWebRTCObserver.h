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

#ifndef MediaStreamWebRTCObserver_h
#define MediaStreamWebRTCObserver_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "MediaStreamTrackWebRTCObserver.h"
#include "libwebrtc.h"
#include <wtf/RefCounted.h>

namespace WebCore {

class MediaStreamPrivate;

class MediaStreamWebRTCObserver : public RefCounted<MediaStreamWebRTCObserver>, public webrtc::ObserverInterface {
public:
    MediaStreamWebRTCObserver(webrtc::MediaStreamInterface*, MediaStreamPrivate*, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>&, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>&);

    virtual ~MediaStreamWebRTCObserver() { }

    void OnChanged();
    MediaStreamPrivate* privateStream() { return m_private; }
    webrtc::MediaStreamInterface* webRTCStream() { return m_stream.get(); }

private:
    template<typename T>
    void findAndAddTrack(const T&, Vector<RefPtr<MediaStreamTrackWebRTCObserver>>&);

    void findAndRemoveTrack(Vector<RefPtr<MediaStreamTrackWebRTCObserver>>&);

    bool haveTrackObserver(const std::string&, const Vector<RefPtr<MediaStreamTrackWebRTCObserver>>&);
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> m_stream;
    MediaStreamPrivate* m_private;
    Vector<RefPtr<MediaStreamTrackWebRTCObserver>> m_audioTrackObservers;
    Vector<RefPtr<MediaStreamTrackWebRTCObserver>> m_videoTrackObservers;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // MediaStreamWebRTCObserver_h
