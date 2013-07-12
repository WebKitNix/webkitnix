/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "AudioBus.h"

#include "AudioFileReader.h"
#include <public/Platform.h>
#include <public/WebAudioBus.h>
#include <wtf/PassRefPtr.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringConcatenate.h>

namespace WebCore {

PassRefPtr<AudioBus> decodeAudioFileData(const char* data, size_t size, double sampleRate)
{
    Nix::WebAudioBus webAudioBus;
    if (Nix::Platform::current()->loadAudioResource(&webAudioBus, data, size, sampleRate))
        return webAudioBus.audioBus();
    return PassRefPtr<AudioBus>();
}

PassRefPtr<AudioBus> AudioBus::loadPlatformResource(const char* name, float sampleRate)
{
    String absoluteFilename(makeString(DATA_DIR, "/webaudio/resources/", name, ".wav"));
    const Nix::Data resource = Nix::Platform::current()->loadResource(absoluteFilename.utf8().data());

    if (resource.isEmpty())
        return PassRefPtr<AudioBus>();

    // FIXME: the sampleRate parameter is ignored. It should be removed from the API.
    RefPtr<AudioBus> audioBus = decodeAudioFileData(resource.data(), resource.size(), sampleRate);

    if (!audioBus.get())
        return PassRefPtr<AudioBus>();

    // If the bus is already at the requested sample-rate then return as is.
    if (audioBus->sampleRate() == sampleRate)
        return audioBus.release();

    return AudioBus::createBySampleRateConverting(audioBus.get(), false, sampleRate);
}

PassRefPtr<AudioBus> createBusFromInMemoryAudioFile(const void* data, size_t dataSize, bool mixToMono, float sampleRate)
{
    // FIXME: the sampleRate parameter is ignored. It should be removed from the API.
    RefPtr<AudioBus> audioBus = decodeAudioFileData(static_cast<const char*>(data), dataSize, sampleRate);
    if (!audioBus.get())
        return PassRefPtr<AudioBus>();

    // If the bus needs no conversion then return as is.
    if ((!mixToMono || audioBus->numberOfChannels() == 1) && audioBus->sampleRate() == sampleRate)
        return audioBus.release();

    return AudioBus::createBySampleRateConverting(audioBus.get(), mixToMono, sampleRate);
}

} // namespace WebCore

#endif // ENABLE(WEB_AUDIO)
