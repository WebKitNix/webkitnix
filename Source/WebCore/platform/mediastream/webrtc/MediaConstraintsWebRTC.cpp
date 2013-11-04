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

#include "MediaConstraintsWebRTC.h"

#include <wtf/text/CString.h>

namespace WebCore {

const String MediaConstraintsWebRTC::s_validConstraints[] = {
    "OfferToReceiveAudio",
    "OfferToReceiveVideo",
    "VoiceActivityDetection",
    "IceTransports",
    "IceRestart",
    "RequestIdentity",
    "" // Loop stop condition.
};

MediaConstraintsWebRTC::MediaConstraintsWebRTC(PassRefPtr<MediaConstraints> constraints)
{
    Vector<MediaConstraint> mandatory;
    constraints->getMandatoryConstraints(mandatory);
    toMediaConstraintsWebRTC(mandatory, m_mandatory);
    Vector<MediaConstraint> optional;
    constraints->getOptionalConstraints(optional);
    toMediaConstraintsWebRTC(optional, m_optional);
}

bool MediaConstraintsWebRTC::isConstraintValid(const String& constraint)
{
    for (unsigned i = 0; !s_validConstraints[i].isEmpty(); ++i) {
        if (constraint == s_validConstraints[i])
            return true;
    }

    return false;
}

void MediaConstraintsWebRTC::pushConstraint(const String& constraint, const String& value, webrtc::MediaConstraintsInterface::Constraints& webRTCConstraints)
{
    webrtc::MediaConstraintsInterface::Constraint newConstraint;
    newConstraint.key = constraint.utf8().data();
    newConstraint.value = value.utf8().data();
    webRTCConstraints.push_back(newConstraint);
}

void MediaConstraintsWebRTC::toMediaConstraintsWebRTC(const WTF::Vector<MediaConstraint>& constraints, webrtc::MediaConstraintsInterface::Constraints& webRTCConstraints)
{
    for (const MediaConstraint& constraint : constraints) {
        if (isConstraintValid(constraint.m_name))
            pushConstraint(constraint.m_name, constraint.m_value, webRTCConstraints);
    }
}

const webrtc::MediaConstraintsInterface::Constraints& MediaConstraintsWebRTC::GetMandatory() const
{
    return m_mandatory;
}

const webrtc::MediaConstraintsInterface::Constraints& MediaConstraintsWebRTC::GetOptional() const
{
    return m_optional;
}

void MediaConstraintsWebRTC::addMandatoryConstraint(const String& constraint, const String& value)
{
    pushConstraint(constraint, value, m_mandatory);
}

void MediaConstraintsWebRTC::addOptionalConstraint(const String& constraint, const String& value)
{
    pushConstraint(constraint, value, m_optional);
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM) && USE(WEBRTCLIB)
