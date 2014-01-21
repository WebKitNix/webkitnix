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
#include <public/MediaConstraints.h>
#include <public/MediaStream.h>
#include <public/MediaStreamCenter.h>
#include <public/MediaStreamAudioSource.h>
#include <public/Platform.h>
#include <public/Rect.h>
#include <public/Size.h>
#include <WebKit2/WKRetainPtr.h>

#include <glib.h>
#include <cstring>
#include <algorithm>

namespace TestWebKitAPI {

namespace TestMediaStreamAudioAPI {

class MockAudioOutputDevice : public Nix::AudioDevice {
public:
    MockAudioOutputDevice(const char* inputDeviceId, size_t bufferSize, unsigned, unsigned, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback)
        : m_bufferSize(bufferSize)
        , m_sampleRate(sampleRate)
        , m_renderCallback(renderCallback)
        , m_loopId(0)
    {
        // AudioContext currently chooses and enable the 1st audio source with a non-empty device id.
        if (!std::strcmp(inputDeviceId, "DummyAudioDeviceId#0"))
            WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("DummyAudioDeviceId#0").get(), 0);
    }

    ~MockAudioOutputDevice() override
    {
        g_source_remove(m_loopId);
    }

    static gboolean audioProcessLoop(gpointer userData)
    {
        MockAudioOutputDevice* device = static_cast<MockAudioOutputDevice*>(userData);
        size_t bufferSize = device->m_bufferSize;
        std::vector<float*> sourceDataVector(2);
        // This audio loop will start to run regardless of the underlying
        // AudioContext being runnable (e.g. the HRTF database is still being
        // loaded), returning zeroes (silence) by default in the audioDataVector and
        // discarding the sourceDataVector.
        // We send a sequence of ones to signal when the loopback is ready and
        // once it's detected we can start sending the "ramp" pulse for the test.
        static bool sendRamp = false;
        for (float*& data : sourceDataVector) {
            data = new float[bufferSize];
            if (sendRamp) {
                for (size_t j = 0; j < bufferSize; ++j)
                    data[j] = (float) j;
            } else
                std::fill(data, data + bufferSize, 1.0);
        }

        std::vector<float*> audioDataVector(2);
        for (float*& data : audioDataVector) {
            data = new float[bufferSize];
            std::memset(data, 0, bufferSize * sizeof(float));
        }
        device->m_renderCallback->render(sourceDataVector, audioDataVector, bufferSize);

        // Detect pulse ramp (0*)(1+)(0..1023).
        bool shouldContinue = true;
        static size_t correctSamples = 0;
        size_t currentValue = 0;
        for (size_t i = 0; i < bufferSize && shouldContinue; ++i) {

            if (!Util::fuzzyCompare(audioDataVector[0][i], audioDataVector[1][i])) {
                shouldContinue = false;
                continue;
            }

            currentValue = static_cast<size_t>(audioDataVector[0][i]);

            if (!currentValue && !sendRamp) {
                // System not initialized.
                continue;
            } else if (currentValue == 1 && !correctSamples)
                sendRamp = true;
            else if (currentValue == correctSamples)
                correctSamples++;
            else
                shouldContinue = false;

            if (correctSamples == bufferSize) {
                WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("AudioDataVectorRendered").get(), 0);
                shouldContinue = false;
            }
        }

        for (size_t i = 0; i < sourceDataVector.size(); ++i)
            delete[] sourceDataVector[i];
        for (size_t i = 0; i < audioDataVector.size(); ++i)
            delete[] audioDataVector[i];
        return shouldContinue;
    }

    virtual void start() override
    {
        m_loopId = g_idle_add(MockAudioOutputDevice::audioProcessLoop, this);
    }

    virtual void stop() override
    {
        g_source_remove(m_loopId);
    }

    virtual double sampleRate() override
    {
        return m_sampleRate;
    }

private:
    size_t m_bufferSize;
    double m_sampleRate;
    guint m_loopId;
    Nix::AudioDevice::RenderCallback* m_renderCallback;
};

static void testMediaStreamAttributes(Nix::MediaStreamSource *source)
{
    bool orig_enabled = source->enabled();
    bool orig_muted = source->muted();
    bool orig_readonly = source->readonly();
    bool failed = false;

    source->setEnabled(true);
    if (!source->enabled())
        failed = true;

    source->setEnabled(false);
    if (source->enabled())
        failed = true;

    source->setMuted(true);
    if (!source->muted())
        failed = true;

    source->setMuted(false);
    if (source->muted())
        failed = true;

    source->setReadonly(true);
    if (!source->readonly())
        failed = true;

    source->setReadonly(false);
    if (source->readonly())
        failed = true;

    if (!failed)
        WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("MediaStreamAttributesOk").get(), 0);

    source->setEnabled(orig_enabled);
    source->setMuted(orig_muted);
    source->setReadonly(orig_readonly);
}

class MockMediaStreamCenter : public Nix::MediaStreamCenter {
public:
    virtual const char* validateRequestConstraints(Nix::MediaConstraints& audioConstraints, Nix::MediaConstraints& videoConstraints) override
    {
        std::vector<Nix::MediaConstraint> optional;
        audioConstraints.getOptionalConstraints(optional);
        const char *retVal = "volume";

        Nix::MediaConstraint volume = optional[0];
        if (volume.m_name == "volume" && volume.m_value == "99") {
            WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("MediaConstraintsOk").get(), 0);
            retVal = nullptr;
        }

        return retVal;
    }

    virtual Nix::MediaStream createMediaStream(Nix::MediaConstraints& audioConstraints, Nix::MediaConstraints& videoConstraints) override
    {
        std::vector<Nix::MediaStreamSource*> audioSources;
        std::vector<Nix::MediaStreamSource*> videoSources;

        if (!audioConstraints.isNull()) {
            Nix::MediaStreamAudioSource *tmp = nullptr;
            audioSources = std::vector<Nix::MediaStreamSource*>(3);

            tmp = new Nix::MediaStreamAudioSource();
            tmp->setDeviceId("DummyAudioDeviceId#0");
            audioSources[0] = tmp;

            tmp = new Nix::MediaStreamAudioSource();
            tmp->setDeviceId("DummyAudioDeviceId#1");
            audioSources[1] = tmp;

            tmp = new Nix::MediaStreamAudioSource();
            tmp->setDeviceId("DummyAudioDeviceId#2");
            audioSources[2] = tmp;

            testMediaStreamAttributes(tmp);
        }

        Nix::MediaStream mediaStream;
        mediaStream.initialize(audioSources, videoSources);
        return mediaStream;
    }
};

class TestMediaStreamAudioPlatform : public Nix::Platform {
public:
    virtual float audioHardwareSampleRate() override
    {
        return 44100;
    }

    virtual size_t audioHardwareBufferSize() override
    {
        return 1024;
    }

    virtual unsigned audioHardwareOutputChannels() override
    {
        return 2;
    }

    virtual Nix::AudioDevice* createAudioDevice(const char* inputDeviceId, size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback) override
    {
        return new MockAudioOutputDevice(inputDeviceId, bufferSize, numberOfInputChannels, numberOfChannels, sampleRate, renderCallback);
    }

    virtual Nix::FFTFrame* createFFTFrame(unsigned fftsize)
    {
        return new Util::FFTFrameTest(fftsize);
    }

    virtual Nix::MediaStreamCenter* createMediaStreamCenter() override
    {
        return new MockMediaStreamCenter();
    }
    virtual Nix::MultiChannelPCMData* decodeAudioResource(const void* audioFileData, size_t dataSize, double sampleRate) override
    {
        return Util::decodeAudioResource(audioFileData, dataSize, sampleRate);
    }
};
} // namespace TestMediaStreamAudioAPI

class MediaStreamAudioTest : public InjectedBundleTest {
public:
    MediaStreamAudioTest(const std::string& identifier)
        : InjectedBundleTest(identifier)
    {
        static TestMediaStreamAudioAPI::TestMediaStreamAudioPlatform platform;
        Nix::Platform::initialize(&platform);
    }
};

static InjectedBundleTest::Register<MediaStreamAudioTest> registrar("MediaStreamAudioTest");

} // namespace TestWebKitAPI
