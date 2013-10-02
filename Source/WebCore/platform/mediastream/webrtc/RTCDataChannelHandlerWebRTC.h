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

#ifndef RTCDataChannelHandlerWebRTC_h
#define RTCDataChannelHandlerWebRTC_h

#if ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#include "RTCDataChannelHandler.h"

#include "WebRTCDefinitions.h"
#include "observers/RTCDataChannelObserver.h"
#include "talk/app/webrtc/datachannelinterface.h"
#include "talk/base/scoped_ref_ptr.h"

namespace WebCore {

class RTCDataChannelHandlerWebRTC : public RTCDataChannelHandler {
public:
    RTCDataChannelHandlerWebRTC(webrtc::DataChannelInterface*);
    virtual void setClient(RTCDataChannelHandlerClient*);

    virtual String label();
    virtual bool ordered();
    virtual unsigned short maxRetransmitTime();
    virtual unsigned short maxRetransmits();
    virtual String protocol();
    virtual bool negotiated();
    virtual unsigned short id();
    virtual unsigned long bufferedAmount();

    virtual bool sendStringData(const String&);
    virtual bool sendRawData(const char*, size_t);
    virtual void close();

private:
    RTCDataChannelHandlerClient* m_client;
    RTCDataChannelObserver m_observer;
    talk_base::scoped_refptr<webrtc::DataChannelInterface> m_webRTCDataChannel;
};

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)

#endif // RTCDataChannelHandlerWebRTC_h
