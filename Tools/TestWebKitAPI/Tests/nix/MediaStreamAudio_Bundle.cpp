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

    ~MockAudioOutputDevice() OVERRIDE
    {
        g_source_remove(m_loopId);
    }

    static gboolean audioProcessLoop(gpointer userData)
    {
        MockAudioOutputDevice* device = static_cast<MockAudioOutputDevice*>(userData);
        size_t bufferSize = device->m_bufferSize;
        std::vector<float*> sourceDataVector(2);
        // Send a single "ramp" pulse and silence on the next iterations.
        static bool first = true;
        for (float*& data : sourceDataVector) {
            data = new float[bufferSize];
            if (first) {
                for (size_t j = 0; j < bufferSize; ++j)
                    data[j] = (float) j;
            } else
                std::memset(data, 0, bufferSize * sizeof(float));
        }
        first = false;

        std::vector<float*> audioDataVector(2);
        for (float*& data : audioDataVector) {
            data = new float[bufferSize];
            std::memset(data, 0, bufferSize * sizeof(float));
        }
        device->m_renderCallback->render(sourceDataVector, audioDataVector, bufferSize);

        static bool shouldContinue = true;
        static int count = 1;
        for (size_t i = 0; i < bufferSize; ++i) {
            if (audioDataVector[0][i]) {
                if (Util::fuzzyCompare(audioDataVector[0][i], audioDataVector[1][i]) && Util::fuzzyCompare(audioDataVector[0][i], count))
                    count += 1;
                else
                    shouldContinue = false;
            }
        }

        if (shouldContinue && count == bufferSize) {
            WKBundlePostMessage(InjectedBundleController::shared().bundle(), Util::toWK("AudioDataVectorRendered").get(), 0);
            shouldContinue = false;
        }
        for (size_t i = 0; i < sourceDataVector.size(); ++i)
            delete[] sourceDataVector[i];
        for (size_t i = 0; i < audioDataVector.size(); ++i)
            delete[] audioDataVector[i];
        return shouldContinue;
    }

    virtual void start() OVERRIDE
    {
        m_loopId = g_idle_add(MockAudioOutputDevice::audioProcessLoop, this);
    }

    virtual void stop() OVERRIDE
    {
        g_source_remove(m_loopId);
    }

    virtual double sampleRate() OVERRIDE
    {
        return m_sampleRate;
    }

private:
    size_t m_bufferSize;
    double m_sampleRate;
    guint m_loopId;
    Nix::AudioDevice::RenderCallback* m_renderCallback;
};

class MockMediaStreamCenter : public Nix::MediaStreamCenter {
public:
    virtual const char* validateRequestConstraints(Nix::MediaConstraints& audioConstraints, Nix::MediaConstraints& videoConstraints) OVERRIDE
    {
        return nullptr; // All constraints are ok.
    }

    virtual Nix::MediaStream createMediaStream(Nix::MediaConstraints& audioConstraints, Nix::MediaConstraints& videoConstraints) OVERRIDE
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
        }

        Nix::MediaStream mediaStream;
        mediaStream.initialize(audioSources, videoSources, Nix::MediaStream::IsNotEnded);
        return mediaStream;
    }
};

class TestMediaStreamAudioPlatform : public Nix::Platform {
public:
    virtual float audioHardwareSampleRate() OVERRIDE
    {
        return 44100;
    }

    virtual size_t audioHardwareBufferSize() OVERRIDE
    {
        return 1024;
    }

    virtual unsigned audioHardwareOutputChannels() OVERRIDE
    {
        return 2;
    }

    virtual Nix::AudioDevice* createAudioDevice(const char* inputDeviceId, size_t bufferSize, unsigned numberOfInputChannels, unsigned numberOfChannels, double sampleRate, Nix::AudioDevice::RenderCallback* renderCallback) OVERRIDE
    {
        return new MockAudioOutputDevice(inputDeviceId, bufferSize, numberOfInputChannels, numberOfChannels, sampleRate, renderCallback);
    }

    virtual Nix::MediaStreamCenter* createMediaStreamCenter() OVERRIDE
    {
        return new MockMediaStreamCenter();
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
