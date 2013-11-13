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
#include "InjectedBundleTest.h"
#include "PlatformUtilities.h"
#include <public/FFTFrame.h>
#include <public/MultiChannelPCMData.h>
#include <public/Platform.h>
#include <public/Rect.h>
#include <public/Size.h>
#include <WebKit2/WKRetainPtr.h>

#include <glib.h>
#include <cstring>

namespace TestWebKitAPI {

namespace TestWebAudioAPI {

class MockAudioOutputDevice : public Nix::AudioDevice {
public:
    MockAudioOutputDevice(size_t bufferSize, unsigned, unsigned, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback)
        : m_bufferSize(bufferSize)
        , m_sampleRate(sampleRate)
        , m_renderCallback(renderCallback)
    {
    }
    static gboolean audioProcessLoop(gpointer userData)
    {
        MockAudioOutputDevice* device = static_cast<MockAudioOutputDevice*>(userData);
        size_t bufferSize = device->m_bufferSize;
        std::vector<float*> sourceDataVector;
        std::vector<float*> audioDataVector(2);
        for (size_t i = 0; i < audioDataVector.size(); ++i) {
            audioDataVector[i] = new float[bufferSize];
            std::memset(audioDataVector[i], 0, bufferSize * sizeof(float));
        }
        device->m_renderCallback->render(sourceDataVector, audioDataVector, bufferSize);

        // Detect pulse ramp (0+,1..1023).
        bool shouldContinue = true;
        static size_t correctSamples = 0;
        size_t currentValue = 0;
        for (size_t i = 0; i < bufferSize && shouldContinue; ++i) {

            if (!Util::fuzzyCompare(audioDataVector[0][i], audioDataVector[1][i])) {
                shouldContinue = false;
                continue;
            }

            currentValue = static_cast<size_t>(audioDataVector[0][i]);

            if (currentValue == 0 && (correctSamples == 0 || correctSamples == 1))
                correctSamples = 1;
            else if (currentValue == correctSamples)
                correctSamples++;
            else
                shouldContinue = false;

            if (correctSamples == bufferSize) {
                WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("AudioDataVectorRendered").get(), 0);
                shouldContinue = false;
            }
        }

        for (size_t i = 0; i < audioDataVector.size(); ++i)
            delete[] audioDataVector[i];
        return shouldContinue;
    }
    virtual void start() OVERRIDE { g_idle_add(MockAudioOutputDevice::audioProcessLoop, this); }
    virtual void stop() OVERRIDE { }
    virtual double sampleRate() OVERRIDE { return m_sampleRate; }

    size_t m_bufferSize;
    double m_sampleRate;
    Nix::AudioDevice::RenderCallback* m_renderCallback;
};


class FFTFrameTest : public Nix::FFTFrame {
public:
    FFTFrameTest(unsigned size) : m_size(size), m_dummyData(new float[size]){ }

    virtual FFTFrame* copy() const { return new FFTFrameTest(m_size); }

    virtual void doFFT(const float*) {}
    virtual void doInverseFFT(float*) {}

    virtual unsigned frequencyDomainSampleCount() const { return 0; }
    virtual float* realData() const { return m_dummyData.get(); }
    virtual float* imagData() const { return realData(); }

private:
    unsigned m_size;
    std::unique_ptr<float> m_dummyData;
};

class TestWebAudioPlatform : public Nix::Platform {
public:
    virtual float audioHardwareSampleRate() OVERRIDE { return 44100; }
    virtual size_t audioHardwareBufferSize() OVERRIDE { return 1024; }
    virtual unsigned audioHardwareOutputChannels() OVERRIDE { return 2; }
    virtual Nix::FFTFrame* createFFTFrame(unsigned fftsize) {
        return new FFTFrameTest(fftsize);
    }
    virtual Nix::AudioDevice* createAudioDevice(const char*, size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback) OVERRIDE
    {
        return new MockAudioOutputDevice(bufferSize, numberOfInputChannels, numberOfChannels, sampleRate, renderCallback);
    }
    virtual Nix::MultiChannelPCMData* decodeAudioResource(const void* audioFileData, size_t dataSize, double sampleRate) OVERRIDE
    {
        // HRTFElevation will break in debug if a different size is given.
        // As we don't properly decode the file in the test, we fake it.
        dataSize = 240 * 256;
        Nix::MultiChannelPCMData* bus = new Nix::MultiChannelPCMData(2, dataSize, sampleRate);
        memcpy(bus->channelData(0), audioFileData, dataSize / 4);
        memcpy(bus->channelData(1), audioFileData, dataSize / 4);
        return bus;
    }
};

} // namespace TestWebAudioAPI

class WebAudioTest : public InjectedBundleTest {
public:
    WebAudioTest(const std::string& identifier)
        : InjectedBundleTest(identifier)
    {
        static TestWebAudioAPI::TestWebAudioPlatform platform;
        Nix::Platform::initialize(&platform);
    }
};

static InjectedBundleTest::Register<WebAudioTest> registrar("WebAudioTest");

} // namespace TestWebKitAPI
