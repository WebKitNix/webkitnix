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
#include "MediaStreamTrack.h"
#include "NotImplemented.h"
#include "RTCConfiguration.h"
#include "RTCDTMFSenderHandler.h"
#include "RTCDataChannelHandler.h"
#include "RTCDataChannelHandlerWebRTC.h"
#include "RTCIceCandidateDescriptor.h"
#include "RTCSessionDescriptionRequest.h"
#include "RTCStatsRequest.h"
#include "RTCVoidRequest.h"
#include "WebRTCUtils.h"
#include "talk/app/webrtc/datachannelinterface.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include <wtf/text/CString.h>

namespace WebCore {

RTCPeerConnectionHandlerWebRTC::RTCPeerConnectionHandlerWebRTC(RTCPeerConnectionHandlerClient* client)
    : RTCPeerConnectionHandler()
    , m_connectionObserver(client)
    , m_createSessionObserver(new talk_base::RefCountedObject<CreateSessionDescriptionObserver>())
    , m_setSessionObserver(new talk_base::RefCountedObject<SetSessionDescriptionObserver>())
    , m_getStatsObserver(new talk_base::RefCountedObject<GetStatsObserver>())
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

void RTCPeerConnectionHandlerWebRTC::setLocalDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> description)
{
    RefPtr<RTCVoidRequest> voidRequest = request;
    webrtc::SessionDescriptionInterface* webRTCDescription = getWebRTCSessionDescription(voidRequest, description);
    if (!webRTCDescription)
        return;

    m_setSessionObserver->setWebKitRequest(voidRequest);
    m_webRTCPeerConnection->SetLocalDescription(m_setSessionObserver.get(), webRTCDescription);
    m_localSessionDescription = description;
}

void RTCPeerConnectionHandlerWebRTC::setRemoteDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> description)
{
    RefPtr<RTCVoidRequest> voidRequest = request;
    webrtc::SessionDescriptionInterface* webRTCDescription = getWebRTCSessionDescription(voidRequest, description);
    if (!webRTCDescription)
        return;

    m_setSessionObserver->setWebKitRequest(voidRequest);
    m_webRTCPeerConnection->SetRemoteDescription(m_setSessionObserver.get(), webRTCDescription);
    m_remoteSessionDescription = description;
}

webrtc::SessionDescriptionInterface* RTCPeerConnectionHandlerWebRTC::getWebRTCSessionDescription(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCSessionDescriptionDescriptor> description)
{
    std::string sdp = description->sdp().utf8().data();
    std::string type = description->type().utf8().data();
    webrtc::SdpParseError error;
    webrtc::SessionDescriptionInterface* webRTCDescription = webrtc::CreateSessionDescription(type, sdp, &error);
    if (!webRTCDescription) {
        std::string reasonStr = "Failed to parse SessionDescription. ";
        reasonStr.append(error.line);
        reasonStr.append(" ");
        reasonStr.append(error.description);
        request->requestFailed(WTF::String(reasonStr.c_str()));
        return 0;
    }

    return webRTCDescription;
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerWebRTC::localDescription()
{
    return m_localSessionDescription;
}

PassRefPtr<RTCSessionDescriptionDescriptor> RTCPeerConnectionHandlerWebRTC::remoteDescription()
{
    return m_remoteSessionDescription;
}

bool RTCPeerConnectionHandlerWebRTC::updateIce(PassRefPtr<RTCConfiguration> configuration, PassRefPtr<MediaConstraints> constraints)
{
    webrtc::PeerConnectionInterface::IceServers servers;
    WebRTCUtils::toWebRTCIceServers(configuration, &servers);
    MediaConstraintsWebRTC mediaConstraints(constraints);

    return m_webRTCPeerConnection->UpdateIce(servers, &mediaConstraints);
}

bool RTCPeerConnectionHandlerWebRTC::addIceCandidate(PassRefPtr<RTCVoidRequest> request, PassRefPtr<RTCIceCandidateDescriptor> iceDescriptor)
{
    webrtc::IceCandidateInterface *ice = webrtc::CreateIceCandidate(iceDescriptor->sdpMid().utf8().data(), iceDescriptor->sdpMLineIndex(), iceDescriptor->candidate().utf8().data());
    talk_base::scoped_ptr<webrtc::IceCandidateInterface> webRTCCandidate(ice);

    if (!webRTCCandidate) {
        request->requestFailed("Invalid ICE candidate");
        return false;
    }

    bool iceAdded = m_webRTCPeerConnection->AddIceCandidate(webRTCCandidate.get());
    if (iceAdded)
        request->requestSucceeded();
    else
        request->requestFailed("Could not add ICE candidate");

    return iceAdded;
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

webrtc::MediaStreamTrackInterface* RTCPeerConnectionHandlerWebRTC::getWebRTCMediaStreamTrack(PassRefPtr<RTCStatsRequest> request)
{
    talk_base::scoped_refptr<webrtc::StreamCollectionInterface> streamCollection = m_webRTCPeerConnection->local_streams();
    if (!streamCollection.get())
        return 0;

    std::string trackId = request->track()->id().utf8().data();
    if (request->track()->source()->type() == MediaStreamSource::Audio)
        return streamCollection->FindAudioTrack(trackId);

    if (request->track()->source()->type() == MediaStreamSource::Video)
        return streamCollection->FindVideoTrack(trackId);

    return 0;
}

void RTCPeerConnectionHandlerWebRTC::getStats(PassRefPtr<RTCStatsRequest> request)
{
    webrtc::MediaStreamTrackInterface* track = 0;
    if (request->hasSelector()) {
        track = getWebRTCMediaStreamTrack(request);

        if (!track) {
            m_getStatsObserver->OnComplete();
            return;
        }
    }
    m_getStatsObserver->setWebKitRequest(request);
    if (!m_webRTCPeerConnection->GetStats(m_getStatsObserver, track)) {
        m_getStatsObserver->OnComplete();
        return;
    }
}

PassOwnPtr<RTCDataChannelHandler> RTCPeerConnectionHandlerWebRTC::createDataChannel(const String& label, const RTCDataChannelInit& init)
{
    webrtc::DataChannelInit config;
    config.ordered = init.ordered;
    config.maxRetransmitTime = init.maxRetransmitTime;
    config.maxRetransmits = init.maxRetransmits;
    config.protocol = init.protocol.utf8().data();
    config.negotiated = init.negotiated;
    config.id = init.id;

    std::string channelLabel = label.utf8().data();
    talk_base::scoped_refptr<webrtc::DataChannelInterface> channel(m_webRTCPeerConnection->CreateDataChannel(channelLabel, &config));
    if (!channel)
        return 0;

    return adoptPtr(new RTCDataChannelHandlerWebRTC(channel));
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
