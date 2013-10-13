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

#include "RTCPeerConnectionObserver.h"

#include "MediaStreamAudioSource.h"
#include "MediaStreamDescriptor.h"
#include "NotImplemented.h"
#include "RTCDataChannelHandlerWebRTC.h"
#include "RTCIceCandidateDescriptor.h"
#include "WebRTCUtils.h"
#include <wtf/Functional.h>
#include <wtf/MainThread.h>
#include <wtf/PassOwnPtr.h>

namespace WebCore {

RTCPeerConnectionObserver::RTCPeerConnectionObserver(RTCPeerConnectionHandlerClient* client)
    : m_client(client)
{
}

void RTCPeerConnectionObserver::OnError()
{
    notImplemented();
}

void RTCPeerConnectionObserver::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeSignalingState, m_client, WebRTCUtils::toWebKitSignalingState(newState)));
}

void RTCPeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType)
{
    notImplemented();
}

PassRefPtr<MediaStreamDescriptor> RTCPeerConnectionObserver::mediaStreamDescriptorFromMediaStreamInterface(webrtc::MediaStreamInterface* stream)
{
    webrtc::AudioTrackVector audioTracks = stream->GetAudioTracks();
    webrtc::VideoTrackVector videoTracks = stream->GetVideoTracks();
    MediaStreamSourceVector audioSourceVector;
    MediaStreamSourceVector videoSourceVector;
    for (unsigned i = 0; i < audioTracks.size(); i++)
        // FIXME: Use the same ID provided by webrtc lib to identify, or map, sources and tracks in WebKit.
        audioSourceVector.append(MediaStreamAudioSource::create());

    // FIXME: Implement a generic class that can be used to video too and use it for video tracks.

    return MediaStreamDescriptor::create(audioSourceVector, videoSourceVector, MediaStreamDescriptor::IsNotEnded);
}

void RTCPeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream)
{
    RefPtr<MediaStreamDescriptor> descriptor = mediaStreamDescriptorFromMediaStreamInterface(stream);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteStream, m_client, descriptor.release()));
}

void RTCPeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream)
{
    // FIXME: We must get the correct MediaStream which is identified or mapped by the webrtc provided id.
    RefPtr<MediaStreamDescriptor> descriptor = mediaStreamDescriptorFromMediaStreamInterface(stream);
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didRemoveRemoteStream, m_client, descriptor.get()));
}

void RTCPeerConnectionObserver::OnRenegotiationNeeded()
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::negotiationNeeded, m_client));
}

void RTCPeerConnectionObserver::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState newState)
{
    if (newState == webrtc::PeerConnectionInterface::kIceGatheringComplete)
        // According to the spec we must call didGenerateIceCandidate with a null parameter
        // to indicate that the gathering is complete.
        callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, (RTCIceCandidateDescriptor*)0));

    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceGatheringState, m_client, WebRTCUtils::toWebKitIceGatheringState(newState)));
}

void RTCPeerConnectionObserver::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState newState)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didChangeIceConnectionState, m_client, WebRTCUtils::toWebKitIceConnectionState(newState)));
}

void RTCPeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::string out;
    candidate->ToString(&out);
    RefPtr<RTCIceCandidateDescriptor> ice = RTCIceCandidateDescriptor::create(out.c_str(), candidate->sdp_mid().c_str(), candidate->sdp_mline_index());
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didGenerateIceCandidate, m_client, ice.release()));
}

void RTCPeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* dataChannel)
{
    callOnMainThread(bind(&RTCPeerConnectionHandlerClient::didAddRemoteDataChannel, m_client, adoptPtr(new RTCDataChannelHandlerWebRTC(dataChannel))));
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
