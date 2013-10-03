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

#ifndef RTCPeerConnectionObserver_h
#define RTCPeerConnectionObserver_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "WebRTCDefinitions.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include <wtf/PassRefPtr.h>

namespace WebCore {

class MediaStreamDescriptor;
class RTCPeerConnectionHandlerClient;

class RTCPeerConnectionObserver : public webrtc::PeerConnectionObserver {
public:
    RTCPeerConnectionObserver(RTCPeerConnectionHandlerClient*);

    virtual void OnError();
    virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState);
    virtual void OnStateChange(webrtc::PeerConnectionObserver::StateType);
    virtual void OnAddStream(webrtc::MediaStreamInterface*);
    virtual void OnRemoveStream(webrtc::MediaStreamInterface*);
    virtual void OnRenegotiationNeeded();
    virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState);
    virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState);
    virtual void OnIceChange();
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface*);
    virtual void OnDataChannel(webrtc::DataChannelInterface*);

private:
    PassRefPtr<MediaStreamDescriptor> mediaStreamDescriptorFromMediaStreamInterface(webrtc::MediaStreamInterface*);
    RTCPeerConnectionHandlerClient* m_client;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // RTCPeerConnectionObserver_h
