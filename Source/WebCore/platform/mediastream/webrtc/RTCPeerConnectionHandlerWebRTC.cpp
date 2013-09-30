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
#include "RTCSessionDescriptionRequest.h"
#include "WebRTCUtils.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include <wtf/text/CString.h>

namespace WebCore {

RTCPeerConnectionHandlerWebRTC::RTCPeerConnectionHandlerWebRTC(RTCPeerConnectionHandlerClient* client)
    : RTCPeerConnectionHandler()
    , m_connectionObserver(client)
    , m_createSessionObserver(new talk_base::RefCountedObject<CreateSessionDescriptionObserver>())
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

void RTCPeerConnectionHandlerWebRTC::createOffer(PassRefPtr<RTCSessionDescriptionRequest> request, PassRefPtr<MediaConstraints> constraints)
{
    m_createSessionObserver->setWebKitRequest(request);
    MediaConstraintsWebRTC mediaConstraints(constraints);
    m_webRTCPeerConnection->CreateOffer(m_createSessionObserver.get(), &mediaConstraints);
}

void RTCPeerConnectionHandlerWebRTC::createAnswer(PassRefPtr<RTCSessionDescriptionRequest> request, PassRefPtr<MediaConstraints> constraints)
{
    m_createSessionObserver->setWebKitRequest(request);
    MediaConstraintsWebRTC mediaConstraints(constraints);
    m_webRTCPeerConnection->CreateAnswer(m_createSessionObserver.get(), &mediaConstraints);
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

bool RTCPeerConnectionHandlerWebRTC::addStream(PassRefPtr<MediaStreamDescriptor> streamDescriptor, PassRefPtr<MediaConstraints> constraints)
{
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream = m_pcFactory->CreateLocalMediaStream(streamDescriptor->id().utf8().data());
    MediaConstraintsWebRTC mediaConstraints(constraints);

    unsigned numberOfSources = streamDescriptor->numberOfAudioStreams();
    for (unsigned i = 0; i < numberOfSources; i++)
        addWebRTCStream(stream.get(), streamDescriptor->audioStreams(i));

    numberOfSources = streamDescriptor->numberOfVideoStreams();
    for (unsigned i = 0; i < numberOfSources; i++)
        addWebRTCStream(stream.get(), streamDescriptor->videoStreams(i));

    return m_webRTCPeerConnection->AddStream(stream.get(), &mediaConstraints);
}

void RTCPeerConnectionHandlerWebRTC::addWebRTCStream(webrtc::MediaStreamInterface* mediaStreamInterface, MediaStreamSource* source)
{
    std::string trackId = source->id().utf8().data();
    if (source->type() == MediaStreamSource::Audio) {
        talk_base::scoped_refptr<webrtc::AudioTrackInterface> audioTrack(
            m_pcFactory->CreateAudioTrack(trackId, m_pcFactory->CreateAudioSource(0)));
        audioTrack->set_enabled(source->enabled());
        audioTrack->set_state(WebRTCUtils::toWebRTCTrackState(source->readyState()));
        mediaStreamInterface->AddTrack(audioTrack.get());
    } // TODO: video.
}

void RTCPeerConnectionHandlerWebRTC::removeStream(PassRefPtr<MediaStreamDescriptor> streamDescriptor)
{
    webrtc::MediaStreamInterface* media = getWebRTCMediaStream(streamDescriptor->id().utf8().data());
    if (!media)
        return;

    m_webRTCPeerConnection->RemoveStream(media);
}

webrtc::MediaStreamInterface* RTCPeerConnectionHandlerWebRTC::getWebRTCMediaStream(const std::string& label)
{
    talk_base::scoped_refptr<webrtc::StreamCollectionInterface> streamCollection = m_webRTCPeerConnection->local_streams();
    if (!streamCollection.get())
        return 0;

    return streamCollection->find(label);
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
