/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AudioUtilitiesNix_h
#define AudioUtilitiesNix_h

#include <public/FFTFrame.h>
#include <public/MultiChannelPCMData.h>

#include <cstring>
#include <memory>

namespace TestWebKitAPI {
namespace Util {

class FFTFrameTest : public Nix::FFTFrame {
public:
    FFTFrameTest(unsigned size) : m_size(size), m_dummyData(new float[size]) { }

    virtual FFTFrame* copy() const { return new FFTFrameTest(m_size); }

    virtual void doFFT(const float*) { }
    virtual void doInverseFFT(float*) { }

    virtual unsigned frequencyDomainSampleCount() const { return 0; }
    virtual float* realData() const { return m_dummyData.get(); }
    virtual float* imagData() const { return realData(); }

private:
    unsigned m_size;
    std::unique_ptr<float> m_dummyData;
};

Nix::MultiChannelPCMData* decodeAudioResource(const void* audioFileData, size_t dataSize, double sampleRate);

} // namespace Util
} // namespace TestWebKitAPI

#endif // AudioUtilitiesNix_h
