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

#include "RTCPeerConnectionHandlerWebRTC.h"

#include "MediaConstraintsWebRTC.h"
#include "NotImplemented.h"
#include "RTCConfiguration.h"
#include "RTCDTMFSenderHandler.h"
#include "RTCDataChannelHandler.h"
#include "WebRTCUtils.h"
#include "talk/app/webrtc/peerconnectionfactory.h"

namespace WebCore {

RTCPeerConnectionHandlerWebRTC::RTCPeerConnectionHandlerWebRTC(RTCPeerConnectionHandlerClient* client)
    : RTCPeerConnectionHandler()
    , m_connectionObserver(client)
{
}

bool RTCPeerConnectionHandlerWebRTC::initialize(PassRefPtr<RTCConfiguration> configuration, PassRefPtr<MediaConstraints> constraints)
{
    MediaConstraintsWebRTC mediaConstraints(constraints);
    webrtc::PeerConnectionInterface::IceServers servers;
    WebRTCUtils::toWebRTCIceServers(configuration, &servers);

    return createPeerConnection(servers, mediaConstraints);
}

bool RTCPeerConnectionHandlerWebRTC::createPeerConnection(const webrtc::PeerConnectionInterface::IceServers& servers, const webrtc::MediaConstraintsInterface& constraints)
{

    m_pcFactory = webrtc::CreatePeerConnectionFactory();
    m_webRTCPeerConnection = m_pcFactory->CreatePeerConnection(servers, &constraints, 0, &m_connectionObserver);
    if (!m_webRTCPeerConnection.get())
        return false;

    return true;
}

void RTCPeerConnectionHandlerWebRTC::createOffer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>)
{
    notImplemented();
}

void RTCPeerConnectionHandlerWebRTC::createAnswer(PassRefPtr<RTCSessionDescriptionRequest>, PassRefPtr<MediaConstraints>)
{
    notImplemented();
}

void RTCPeerConnectionHandlerWebRTC::setLocalDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>)
{
    notImplemented();
}

void RTCPeerConnectionHandlerWebRTC::setRemoteDescription(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCSessionDescriptionDescriptor>)
{
    notImplemented();
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerWebRTC::localDescription()
{
    notImplemented();
    return 0;
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerWebRTC::remoteDescription()
{
    notImplemented();
    return 0;
}

bool RTCPeerConnectionHandlerWebRTC::updateIce(PassRefPtr<RTCConfiguration>, PassRefPtr<MediaConstraints>)
{
    notImplemented();
    return false;
}

bool RTCPeerConnectionHandlerWebRTC::addIceCandidate(PassRefPtr<RTCVoidRequest>, PassRefPtr<RTCIceCandidateDescriptor>)
{
    notImplemented();
    return false;
}

bool RTCPeerConnectionHandlerWebRTC::addStream(PassRefPtr<MediaStreamDescriptor>, PassRefPtr<MediaConstraints>)
{
    notImplemented();
    return false;
}

void RTCPeerConnectionHandlerWebRTC::removeStream(PassRefPtr<MediaStreamDescriptor>)
{
    notImplemented();
}

void RTCPeerConnectionHandlerWebRTC::getStats(PassRefPtr<RTCStatsRequest>)
{
    notImplemented();
}

PassOwnPtr<RTCDataChannelHandler> RTCPeerConnectionHandlerWebRTC::createDataChannel(const String&, const RTCDataChannelInit&)
{
    notImplemented();
    return 0;
}

PassOwnPtr<RTCDTMFSenderHandler> RTCPeerConnectionHandlerWebRTC::createDTMFSender(PassRefPtr<MediaStreamSource>)
{
    notImplemented();
    return 0;
}

void RTCPeerConnectionHandlerWebRTC::stop()
{
    m_webRTCPeerConnection->Close();
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
