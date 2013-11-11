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

#include "CreateSessionDescriptionObserver.h"

#include "RTCSessionDescriptionDescriptor.h"
#include "RTCSessionDescriptionRequest.h"
#include <wtf/Functional.h>
#include <wtf/MainThread.h>

namespace WebCore {

void CreateSessionDescriptionObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    std::string sdp;
    desc->ToString(&sdp);
    RefPtr<RTCSessionDescriptionDescriptor> sessionDescriptor;
    sessionDescriptor = RTCSessionDescriptionDescriptor::create(WTF::String::fromUTF8(desc->type().c_str()), WTF::String(sdp.c_str()));
    callOnMainThread(bind(&RTCSessionDescriptionRequest::requestSucceeded, m_webKitRequest.get(), sessionDescriptor));
    m_webKitRequest.clear();
}

void CreateSessionDescriptionObserver::OnFailure(const std::string& error)
{
    callOnMainThread(bind(&RTCSessionDescriptionRequest::requestFailed, m_webKitRequest.get(), error.c_str()));
    m_webKitRequest.clear();
}

void CreateSessionDescriptionObserver::setWebKitRequest(PassRefPtr<RTCSessionDescriptionRequest> request)
{
    m_webKitRequest = request;
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
