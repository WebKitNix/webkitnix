/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
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

#ifndef Nix_MediaStream_h
#define Nix_MediaStream_h

#include "Common.h"
#include "PrivatePtr.h"
#include "Vector.h"

namespace WebCore {
class MediaStreamDescriptor;
}

namespace Nix {

class MediaStreamSource;
class MediaStreamTrack;
class String;

class MediaStream {
public:
    class ExtraData {
    public:
        virtual ~ExtraData() { }
    };

    MediaStream() { }
    MediaStream(const MediaStream& other) { assign(other); }
    ~MediaStream() { reset(); }

    MediaStream& operator=(const MediaStream& other)
    {
        assign(other);
        return *this;
    }

    WEBKIT_EXPORT void assign(const MediaStream&);

    // DEPRECATED
    WEBKIT_EXPORT void initialize(const String& label, const Vector<MediaStreamSource>& audioSources, const Vector<MediaStreamSource>& videoSources);

    WEBKIT_EXPORT void initialize(const Vector<MediaStreamTrack>& audioTracks, const Vector<MediaStreamTrack>& videoTracks);
    WEBKIT_EXPORT void initialize(const String& label, const Vector<MediaStreamTrack>& audioTracks, const Vector<MediaStreamTrack>& videoTracks);

    WEBKIT_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }

    // DEPRECATED
    WEBKIT_EXPORT String label() const;

    WEBKIT_EXPORT String id() const;

    WEBKIT_EXPORT void audioTracks(Vector<MediaStreamTrack>&) const;
    WEBKIT_EXPORT void videoTracks(Vector<MediaStreamTrack>&) const;

    WEBKIT_EXPORT void addTrack(const MediaStreamTrack&);
    WEBKIT_EXPORT void removeTrack(const MediaStreamTrack&);

    // DEPRECATED
    void audioSources(Vector<MediaStreamTrack>& tracks) const { audioTracks(tracks); }
    void videoSources(Vector<MediaStreamTrack>& tracks) const { videoTracks(tracks); }

    // Extra data associated with this Nix::MediaStream.
    // If non-null, the extra data pointer will be deleted when the object is destroyed.
    // Setting the extra data pointer will cause any existing non-null
    // extra data pointer to be deleted.
    WEBKIT_EXPORT ExtraData* extraData() const;
    WEBKIT_EXPORT void setExtraData(ExtraData*);

#if BUILDING_NIX__
    MediaStream(WebCore::MediaStreamDescriptor*);
    MediaStream(const WTF::PassRefPtr<WebCore::MediaStreamDescriptor>&);
    operator WTF::PassRefPtr<WebCore::MediaStreamDescriptor>() const;
    operator WebCore::MediaStreamDescriptor*() const;
    MediaStream& operator=(const WTF::PassRefPtr<WebCore::MediaStreamDescriptor>&);
#endif

private:
    PrivatePtr<WebCore::MediaStreamDescriptor> m_private;
};

} // namespace Nix

#endif // Nix_MediaStream_h
