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
#include "AudioUtilitiesNix.h"
#include "InjectedBundleTest.h"
#include "PlatformUtilities.h"
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
    virtual void start() override { g_idle_add(MockAudioOutputDevice::audioProcessLoop, this); }
    virtual void stop() override { }
    virtual double sampleRate() override { return m_sampleRate; }

    size_t m_bufferSize;
    double m_sampleRate;
    Nix::AudioDevice::RenderCallback* m_renderCallback;
};

class TestWebAudioPlatform : public Nix::Platform {
public:
    virtual float audioHardwareSampleRate() override { return 44100; }
    virtual size_t audioHardwareBufferSize() override { return 1024; }
    virtual unsigned audioHardwareOutputChannels() override { return 2; }
    virtual Nix::FFTFrame* createFFTFrame(unsigned fftsize) {
        return new Util::FFTFrameTest(fftsize);
    }
    virtual Nix::AudioDevice* createAudioDevice(const char*, size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback) override
    {
        return new MockAudioOutputDevice(bufferSize, numberOfInputChannels, numberOfChannels, sampleRate, renderCallback);
    }
    virtual Nix::MultiChannelPCMData* decodeAudioResource(const void* audioFileData, size_t dataSize, double sampleRate) override
    {
        return Util::decodeAudioResource(audioFileData, dataSize, sampleRate);
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
