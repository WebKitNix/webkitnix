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

#include "RTCDataChannelHandlerWebRTC.h"

#include <wtf/text/CString.h>

namespace WebCore {

RTCDataChannelHandlerWebRTC::RTCDataChannelHandlerWebRTC(webrtc::DataChannelInterface* channel)
    : m_observer(channel)
    , m_webRTCDataChannel(channel)
{
    m_webRTCDataChannel->RegisterObserver(&m_observer);
}

void RTCDataChannelHandlerWebRTC::setClient(RTCDataChannelHandlerClient* client)
{
    m_client = client;
    m_observer.setClient(client);
}

String RTCDataChannelHandlerWebRTC::label()
{
    return m_webRTCDataChannel->label().c_str();
}

bool RTCDataChannelHandlerWebRTC::ordered()
{
    return m_webRTCDataChannel->ordered();
}

unsigned short RTCDataChannelHandlerWebRTC::maxRetransmitTime()
{
    return m_webRTCDataChannel->maxRetransmitTime();
}

unsigned short RTCDataChannelHandlerWebRTC::maxRetransmits()
{
    return m_webRTCDataChannel->maxRetransmits();
}

String RTCDataChannelHandlerWebRTC::protocol()
{
    return m_webRTCDataChannel->protocol().c_str();
}

bool RTCDataChannelHandlerWebRTC::negotiated()
{
    return m_webRTCDataChannel->negotiated();
}

unsigned short RTCDataChannelHandlerWebRTC::id()
{
    return m_webRTCDataChannel->id();
}

unsigned long RTCDataChannelHandlerWebRTC::bufferedAmount()
{
    return m_webRTCDataChannel->buffered_amount();
}

bool RTCDataChannelHandlerWebRTC::sendStringData(const String& data)
{
    webrtc::DataBuffer buffer(talk_base::Buffer(data.utf8().data(), data.sizeInBytes()), false);
    return m_webRTCDataChannel->Send(buffer);
}

bool RTCDataChannelHandlerWebRTC::sendRawData(const char* data, size_t size)
{
    webrtc::DataBuffer buffer(talk_base::Buffer(data, size), true);
    return m_webRTCDataChannel->Send(buffer);
}

void RTCDataChannelHandlerWebRTC::close()
{
    m_webRTCDataChannel->Close();
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
