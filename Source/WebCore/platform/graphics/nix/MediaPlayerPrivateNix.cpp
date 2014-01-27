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

#if ENABLE(VIDEO)
#include "MediaPlayerPrivateNix.h"

#include <public/Platform.h>
#include <wtf/text/CString.h>

namespace WebCore {

void MediaPlayerPrivateNix::registerMediaEngine(MediaEngineRegistrar registrar)
{
    auto nixPlayer = adoptPtr(Nix::Platform::current()->createMediaPlayer(nullptr));
    if (nixPlayer)
        registrar(create, getSupportedTypes, supportsType, 0, 0, 0);
}

PassOwnPtr<MediaPlayerPrivateInterface> MediaPlayerPrivateNix::create(MediaPlayer* player)
{
    return adoptPtr(new MediaPlayerPrivateNix(player));
}

void MediaPlayerPrivateNix::getSupportedTypes(HashSet<String>&)
{
}

MediaPlayer::SupportsType MediaPlayerPrivateNix::supportsType(const MediaEngineSupportParameters&)
{
    // MediaPlayer calls load even if the engine says it doesn't support the format :-/
    return MediaPlayer::IsNotSupported;
}

MediaPlayer::MovieLoadType MediaPlayerPrivateNix::movieLoadType() const
{
    if (m_readyState == MediaPlayer::HaveNothing)
        return MediaPlayer::Unknown;

    if (isLiveStream())
        return MediaPlayer::LiveStream;

    return MediaPlayer::Download;
}

bool MediaPlayerPrivateNix::isLiveStream() const
{
    ASSERT(m_nixPlayer);
    return m_nixPlayer->isLiveStream();
}

class NixMediaPlayerClient : public Nix::MediaPlayerClient {
public:
    NixMediaPlayerClient(MediaPlayerPrivateNix* player)
        : m_player(player)
    {
    }

    virtual void durationChanged() const override
    {
        m_player->durationChanged();
    }

    virtual void currentTimeChanged() const override
    {
        m_player->currentTimeChanged();
    }

    virtual void readyStateChanged(ReadyState state) override
    {
        m_player->setReadyState((MediaPlayer::ReadyState)state);
    }

    virtual void networkStateChanged(NetworkState state) override
    {
        m_player->setNetworkState((MediaPlayer::NetworkState)state);
    }
private:
    MediaPlayerPrivateNix* m_player;
};

MediaPlayerPrivateNix::MediaPlayerPrivateNix(MediaPlayer* player)
    : m_player(player)
    , m_nixPlayer(adoptPtr(Nix::Platform::current()->createMediaPlayer(new NixMediaPlayerClient(this))))
    , m_readyState(MediaPlayer::HaveNothing)
    , m_networkState(MediaPlayer::Idle)
    , m_paused(true)
{
}

void MediaPlayerPrivateNix::load(const WTF::String& url)
{
    URL urlObj(URL(), url);
    String cleanUrl(url);

    // Clean out everything after file:// url path.
    if (urlObj.isLocalFile())
        cleanUrl = cleanUrl.substring(0, urlObj.pathEnd());

    setNetworkState(MediaPlayer::Loading);
    setReadyState(MediaPlayer::HaveNothing);
    m_nixPlayer->load(cleanUrl.utf8().data());
}

void MediaPlayerPrivateNix::cancelLoad()
{
    // TODO: Add cancelLoad to Nix::MediaPlayer
}

void MediaPlayerPrivateNix::prepareToPlay()
{
}

void MediaPlayerPrivateNix::play()
{
    m_nixPlayer->play();
    m_paused = false;
}

void MediaPlayerPrivateNix::pause()
{
    m_nixPlayer->pause();
    m_paused = true;
}

float MediaPlayerPrivateNix::duration() const
{
    return m_nixPlayer->duration();
}

float MediaPlayerPrivateNix::currentTime() const
{
    return m_nixPlayer->currentTime();
}

IntSize MediaPlayerPrivateNix::naturalSize() const
{
    return IntSize();
}

bool MediaPlayerPrivateNix::hasVideo() const
{
    return false;
}
bool MediaPlayerPrivateNix::hasAudio() const
{
    return true;
}

void MediaPlayerPrivateNix::setVisible(bool)
{
}

bool MediaPlayerPrivateNix::seeking() const
{
    return m_nixPlayer->seeking();
}

void MediaPlayerPrivateNix::seek(float time)
{
    return m_nixPlayer->seek(time);
}

float MediaPlayerPrivateNix::maxTimeSeekable() const
{
    return duration();
}

void MediaPlayerPrivateNix::setRate(float rate)
{
    m_nixPlayer->setPlaybackRate(rate);
}

void MediaPlayerPrivateNix::setVolume(float volume)
{
    m_nixPlayer->setVolume(volume);
}

bool MediaPlayerPrivateNix::paused() const
{
    return m_paused;
}

MediaPlayer::NetworkState MediaPlayerPrivateNix::networkState() const
{
    return m_networkState;
}

void MediaPlayerPrivateNix::setNetworkState(MediaPlayer::NetworkState state)
{
    m_networkState = state;
    m_player->networkStateChanged();
}

MediaPlayer::ReadyState MediaPlayerPrivateNix::readyState() const
{
    return m_readyState;
}

void MediaPlayerPrivateNix::setReadyState(MediaPlayer::ReadyState state)
{
    m_readyState = state;
    m_player->readyStateChanged();
}

PassRefPtr<WebCore::TimeRanges> MediaPlayerPrivateNix::buffered() const
{
    // Add this to Nix::MediaPlayer
    return 0;
}

bool MediaPlayerPrivateNix::didLoadingProgress() const
{
    return true;
}

void MediaPlayerPrivateNix::setSize(const WebCore::IntSize&)
{
}

void MediaPlayerPrivateNix::paint(WebCore::GraphicsContext*, const WebCore::IntRect&)
{
}

} // namespace WebCore

#endif // ENABLE(VIDEO)
