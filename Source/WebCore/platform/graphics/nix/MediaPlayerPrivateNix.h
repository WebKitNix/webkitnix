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

#ifndef MediaPlayerPrivateNix_h
#define MediaPlayerPrivateNix_h

#if ENABLE(VIDEO)

#include "MediaPlayerPrivate.h"
#include <public/MediaPlayer.h>

namespace WebCore {

class MediaPlayerPrivateNix : public MediaPlayerPrivateInterface {
public:
    static void registerMediaEngine(MediaEngineRegistrar);

    virtual void load(const WTF::String&) override;
    virtual void cancelLoad() override;
    virtual void prepareToPlay() override;
    virtual void play() override;
    virtual void pause() override;
    virtual bool paused() const override;

    virtual float duration() const override;
    void durationChanged() const { m_player->durationChanged(); }
    virtual float currentTime() const override;
    void currentTimeChanged() const { m_player->timeChanged(); }

    virtual IntSize naturalSize() const override;
    virtual bool hasVideo() const override;
    virtual bool hasAudio() const override;
    virtual void setVisible(bool) override;

    virtual bool seeking() const override;
    virtual void seek(float) override;
    virtual float maxTimeSeekable() const override;

    virtual void setRate(float) override;
    virtual void setVolume(float) override;

    virtual MediaPlayer::NetworkState networkState() const override;
    void setNetworkState(MediaPlayer::NetworkState);
    virtual MediaPlayer::ReadyState readyState() const override;
    void setReadyState(MediaPlayer::ReadyState);
    virtual MediaPlayer::MovieLoadType movieLoadType() const override;

    virtual PassRefPtr<WebCore::TimeRanges> buffered() const override;
    virtual bool didLoadingProgress() const override;
    virtual void setSize(const WebCore::IntSize&) override;
    virtual void paint(WebCore::GraphicsContext*, const WebCore::IntRect&) override;

private:
    MediaPlayerPrivateNix(MediaPlayer*);

    static PassOwnPtr<MediaPlayerPrivateInterface> create(MediaPlayer*);
    static void getSupportedTypes(HashSet<String>&);
    static MediaPlayer::SupportsType supportsType(const MediaEngineSupportParameters&);
    bool isLiveStream() const;

    MediaPlayer* m_player;
    OwnPtr<Nix::MediaPlayer> m_nixPlayer;

    MediaPlayer::ReadyState m_readyState;
    MediaPlayer::NetworkState m_networkState;

    bool m_paused;
};

}

#endif // ENABLE(VIDEO)

#endif // MediaPlayerNix_h
