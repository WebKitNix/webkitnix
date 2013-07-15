/*
 * Copyright (C) 2013-2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"

#if ENABLE(WEB_AUDIO)
#include "FFTFrame.h"

#include "VectorMath.h"
#include <public/Platform.h>
#include <public/FFTFrame.h>


namespace WebCore {

FFTFrame::FFTFrame(unsigned fftSize)
    : m_FFTSize(fftSize)
    , m_log2FFTSize(static_cast<unsigned>(log2(fftSize)))
{
    m_fftFrame = adoptPtr(Nix::Platform::current()->createFFTFrame(fftSize));
}

FFTFrame::FFTFrame(const FFTFrame& frame)
    : m_FFTSize(frame.m_FFTSize)
    , m_log2FFTSize(frame.m_log2FFTSize)
{
    m_fftFrame = adoptPtr(Nix::Platform::current()->createFFTFrame(frame.m_fftFrame.get()));
}

FFTFrame::~FFTFrame()
{
    m_fftFrame.release();
}

void FFTFrame::multiply(const FFTFrame& frame)
{
    if (!m_fftFrame)
        return;

    m_fftFrame->multiply(*frame.m_fftFrame);

    // Scale accounts the peculiar scaling of vecLib on the Mac.
    // This ensures the right scaling all the way back to inverse FFT.
    // FIXME: if we change the scaling on the Mac then this scale
    // factor will need to change too.
    scalePlanarData(0.5f);
}

// Provides time domain samples in argument "data". Frame will store the transformed data.
void FFTFrame::doFFT(const float* data)
{
    if (!m_fftFrame)
        return;

    m_fftFrame->doFFT(data);

    scalePlanarData(2.0f);
}

// Calculates inverse transform from the stored data, putting the results in argument 'data'.
void FFTFrame::doInverseFFT(float* data)
{
    if (!m_fftFrame)
        return;

    m_fftFrame->doInverseFFT(data);

    // Scale so that a forward then inverse FFT yields exactly the original data.
    const float scale = 1.0 / (2 * m_FFTSize);
    for (unsigned i = 0; i < m_FFTSize; ++i)
        data[i] = scale * data[i];
}

void FFTFrame::initialize()
{
}

void FFTFrame::cleanup()
{
}

void FFTFrame::scalePlanarData(float scale)
{
    float* realP = m_fftFrame->realData();
    float* imagP = m_fftFrame->imagData();

    const unsigned framesToProcess = m_fftFrame->frequencyDomainSampleCount();

    for (unsigned i = 0; i < framesToProcess; ++i) {
        realP[i] *= scale;
        imagP[i] *= scale;
    }
}

float* FFTFrame::realData() const
{
    if (!m_fftFrame)
        return 0;

    return const_cast<float*>(m_fftFrame->realData());
}

float* FFTFrame::imagData() const
{
    if (!m_fftFrame)
        return 0;

    return const_cast<float*>(m_fftFrame->imagData());
}

} // namespace WebCore

#endif // ENABLE(WEB_AUDIO)
