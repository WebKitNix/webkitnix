/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef Nix_MediaStreamSource_h
#define Nix_MediaStreamSource_h

#include "Common.h"
#include "PrivatePtr.h"
#include "Vector.h"

namespace WebCore {
class MediaStreamSource;
}

namespace Nix {
class AudioDestinationConsumer;
class String;

class MediaStreamSource {
public:
    class ExtraData {
    public:
        ExtraData() : m_owner(0) { }
        virtual ~ExtraData() { }

        WEBKIT_EXPORT MediaStreamSource owner();

#ifdef BUILDING_NIX__
        void setOwner(WebCore::MediaStreamSource*);
#endif

    private:
        WebCore::MediaStreamSource* m_owner;
    };

    enum Type {
        TypeAudio,
        TypeVideo
    };

    enum ReadyState {
        ReadyStateLive = 0,
        ReadyStateMuted = 1,
        ReadyStateEnded = 2
    };

    MediaStreamSource() { }
    MediaStreamSource(const MediaStreamSource& other) { assign(other); }
    ~MediaStreamSource() { reset(); }

    MediaStreamSource& operator=(const MediaStreamSource& other)
    {
        assign(other);
        return *this;
    }

    WEBKIT_EXPORT void assign(const MediaStreamSource&);

    WEBKIT_EXPORT void initialize(const String& id, Type, const String& name);
    WEBKIT_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }

    WEBKIT_EXPORT String id() const;
    WEBKIT_EXPORT Type type() const;
    WEBKIT_EXPORT String name() const;

    WEBKIT_EXPORT void setReadyState(ReadyState);
    WEBKIT_EXPORT ReadyState readyState() const;

    // Extra data associated with this object.
    // If non-null, the extra data pointer will be deleted when the object is destroyed.
    // Setting the extra data pointer will cause any existing non-null
    // extra data pointer to be deleted.
    WEBKIT_EXPORT ExtraData* extraData() const;
    WEBKIT_EXPORT void setExtraData(ExtraData*);

    WEBKIT_EXPORT String deviceId() const;
    WEBKIT_EXPORT void setDeviceId(const String&);

    // Only used if if this is a WebAudio source.
    // The Nix::AudioDestinationConsumer is not owned, and has to be disposed of separately
    // after calling removeAudioConsumer.
    WEBKIT_EXPORT bool requiresAudioConsumer() const;
    WEBKIT_EXPORT void addAudioConsumer(AudioDestinationConsumer*);
    WEBKIT_EXPORT bool removeAudioConsumer(AudioDestinationConsumer*);

#if BUILDING_NIX__
    MediaStreamSource(const WTF::PassRefPtr<WebCore::MediaStreamSource>&);
    MediaStreamSource& operator=(WebCore::MediaStreamSource*);
    operator WTF::PassRefPtr<WebCore::MediaStreamSource>() const;
    operator WebCore::MediaStreamSource*() const;
#endif

private:
    PrivatePtr<WebCore::MediaStreamSource> m_private;
};

} // namespace Nix

#endif // Nix_MediaStreamSource_h
