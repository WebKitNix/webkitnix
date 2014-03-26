/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#if ENABLE(MEDIA_STREAM)

#include <public/MediaConstraints.h>

#include "MediaConstraints.h"

#include <wtf/text/CString.h>

namespace Nix {

static MediaConstraint toNixMediaConstraint(const WebCore::MediaConstraint& other)
{
    MediaConstraint result;

    result.m_name = other.m_name.utf8().data();
    result.m_value = other.m_value.utf8().data();

    return result;
}

MediaConstraints::MediaConstraints(const PassRefPtr<WebCore::MediaConstraints>& constraints)
    : m_private(constraints)
{
}

MediaConstraints::MediaConstraints(WebCore::MediaConstraints* constraints)
    : m_private(constraints)
{
}

void MediaConstraints::assign(const MediaConstraints& other)
{
    m_private = other.m_private;
}

void MediaConstraints::reset()
{
    m_private.clear();
}

bool MediaConstraints::isNull() const
{
    return !m_private;
}

void MediaConstraints::getMandatoryConstraints(std::vector<MediaConstraint>& constraints) const
{
    ASSERT(!isNull());
    WTF::Vector<WebCore::MediaConstraint> mandatoryConstraints;
    m_private->getMandatoryConstraints(mandatoryConstraints);
    std::vector<MediaConstraint> result(mandatoryConstraints.size());
    for (size_t i = 0; i < mandatoryConstraints.size(); ++i)
        result[i] = toNixMediaConstraint(mandatoryConstraints[i]);
    constraints.swap(result);
}

void MediaConstraints::getOptionalConstraints(std::vector<MediaConstraint>& constraints) const
{
    ASSERT(!isNull());
    WTF::Vector<WebCore::MediaConstraint> optionalConstraints;
    m_private->getOptionalConstraints(optionalConstraints);
    std::vector<MediaConstraint> result(optionalConstraints.size());
    for (size_t i = 0; i < optionalConstraints.size(); ++i)
        result[i] = toNixMediaConstraint(optionalConstraints[i]);
    constraints.swap(result);
}

bool MediaConstraints::getMandatoryConstraintValue(const char* name, std::string& value) const
{
    ASSERT(!isNull());
    WTF::String result;
    if (m_private->getMandatoryConstraintValue(name, result)) {
        value = result.utf8().data();
        return true;
    }
    return false;
}

bool MediaConstraints::getOptionalConstraintValue(const char* name, std::string& value) const
{
    ASSERT(!isNull());
    WTF::String result;
    if (m_private->getOptionalConstraintValue(name, result)) {
        value = result.utf8().data();
        return true;
    }
    return false;
}

} // namespace Nix

#endif // ENABLE(MEDIA_STREAM)

