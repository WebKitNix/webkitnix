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

#ifndef DefaultWebThemeEngine_h
#define DefaultWebThemeEngine_h

#include "public/ThemeEngine.h"

namespace Nix {

class DefaultWebThemeEngine final : public ThemeEngine {
public:
    // Text selection colors.
    virtual Color activeSelectionBackgroundColor() const override;
    virtual Color activeSelectionForegroundColor() const override;
    virtual Color inactiveSelectionBackgroundColor() const override;
    virtual Color inactiveSelectionForegroundColor() const override;

    // List box selection colors
    virtual Color activeListBoxSelectionBackgroundColor() const override;
    virtual Color activeListBoxSelectionForegroundColor() const override;
    virtual Color inactiveListBoxSelectionBackgroundColor() const override;
    virtual Color inactiveListBoxSelectionForegroundColor() const override;

    virtual Color activeTextSearchHighlightColor() const override;
    virtual Color inactiveTextSearchHighlightColor() const override;

    virtual Color focusRingColor() const override;

    virtual Color tapHighlightColor() const override;

    virtual void paintButton(Canvas*, State, const Rect&, const ButtonExtraParams&) const override;
    virtual void paintTextField(Canvas*, State, const Rect&) const override;
    virtual void paintTextArea(Canvas*, State, const Rect&) const override;
    virtual Size getCheckboxSize() const override;
    virtual void paintCheckbox(Canvas*, State, const Rect&, const ButtonExtraParams&) const override;
    virtual Size getRadioSize() const override;
    virtual void paintRadio(Canvas*, State, const Rect&, const ButtonExtraParams&) const override;
    virtual void paintMenuList(Canvas *, State, const Rect &) const override;
    virtual void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const override;
    virtual void paintProgressBar(Canvas*, State, const Rect&, const ProgressBarExtraParams&) const override;
    virtual double getAnimationRepeatIntervalForProgressBar() const override;
    virtual double getAnimationDurationForProgressBar() const override;
    virtual void paintInnerSpinButton(Canvas *, State, const Rect &, const InnerSpinButtonExtraParams&) const override;
    virtual void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const override;
    virtual void paintMeter(Canvas*, State, const Rect&, const MeterExtraParams&) const override;
    virtual void paintSliderTrack(Canvas*, State, const Rect&) const override;
    virtual void paintSliderThumb(Canvas*, State, const Rect&) const override;

    // Media Player
    virtual void paintMediaPlayButton(Canvas*, MediaPlayerState, const Rect&) const override;
    virtual void paintMediaMuteButton(Canvas*, MediaPlayerState, const Rect&) const override;
    virtual void paintMediaSeekBackButton(Canvas*, const Rect&) const override;
    virtual void paintMediaSeekForwardButton(Canvas*, const Rect&) const override;
    virtual void paintMediaRewindButton(Canvas*, const Rect&) const override;
};

} // namespace Nix

#endif // DefaultWebThemeEngine_h
