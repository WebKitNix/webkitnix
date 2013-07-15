/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef Nix_Platform_h
#define Nix_Platform_h

#include "AudioDevice.h"
#include "Data.h"
#include "Gamepads.h"

namespace Nix {

class AudioBus;
class FFTFrame;
class ThemeEngine;

class WEBKIT_EXPORT Platform {
public:

    WEBKIT_EXPORT static void initialize(Platform*);
    WEBKIT_EXPORT static void shutdown();
    WEBKIT_EXPORT static Platform* current();

    // Audio --------------------------------------------------------------
    virtual double audioHardwareSampleRate() { return 0; }
    virtual size_t audioHardwareBufferSize() { return 0; }
    virtual unsigned audioHardwareOutputChannels() { return 0; }

    // Creates a device for audio I/O.
    // Pass in (numberOfInputChannels > 0) if live/local audio input is desired.
    virtual AudioDevice* createAudioDevice(size_t /*bufferSize*/, unsigned /*numberOfInputChannels*/, unsigned /*numberOfChannels*/, double /*sampleRate*/, AudioDevice::RenderCallback*) { return 0; }


    // Gamepad -------------------------------------------------------------
    virtual void sampleGamepads(Gamepads& into) { into.length = 0; }


    // FFTFrame
    virtual FFTFrame* createFFTFrame(unsigned /*fftsize*/) { return 0; }
    virtual FFTFrame* createFFTFrame(const FFTFrame* /*frame*/) { return 0; }

    // Resources -----------------------------------------------------------
    // Returns a blob of data corresponding to the named resource.
    virtual Data loadResource(const char* /*name*/) { return Data(); }

    // Decodes the in-memory audio file data and returns the linear PCM audio data in the destinationBus.
    // A sample-rate conversion to sampleRate will occur if the file data is at a different sample-rate.
    // Returns true on success.
    virtual bool loadAudioResource(AudioBus* /*destinationBus*/, const char* /*audioFileData*/, size_t /*dataSize*/, double /*sampleRate*/) { return false; }

    // Theme engine
    virtual ThemeEngine* themeEngine();

protected:
    virtual ~Platform() { }
};

} // namespace Nix

#endif
