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

#include "DefaultWebThemeEngine.h"

#include "public/WebColor.h"
#include "public/WebRect.h"
#include "public/WebSize.h"
#include <algorithm>
#include <cairo/cairo.h>
#include <cmath>

const double BGColor1 = 0xF6 / 256.0;
const double BGColor2 = 0xDE / 256.0;
const double BorderColor = 0xA4 / 256.0;
const double BorderOnHoverColor = 0x7A / 256.0;
const double CheckColor = 0x66 / 256.0;
const double TextFieldDarkBorderColor = 0x9A / 256.0;
const double TextFieldLightBorderColor = 0xEE / 256.0;

const int MenuListBorder = 5;
const int MenuListArrowSize = 6;

const int InnerSpinButtonBorder = 3;
const int InnerSpinButtonArrowSize = 2;

const WebKit::RGBA32 TapHighLightColor = 0x66000000;

namespace WebKit {

WebColor DefaultWebThemeEngine::activeSelectionBackgroundColor() const
{
    return WebColor(0, 0, 255);
}

WebColor DefaultWebThemeEngine::activeSelectionForegroundColor() const
{
    return WebColor::white;
}

WebColor DefaultWebThemeEngine::inactiveSelectionBackgroundColor() const
{
    return WebColor(176, 176, 176);
}

WebColor DefaultWebThemeEngine::inactiveSelectionForegroundColor() const
{
    return WebColor::black;
}

WebColor DefaultWebThemeEngine::activeListBoxSelectionBackgroundColor() const
{
    return activeSelectionBackgroundColor();
}

WebColor DefaultWebThemeEngine::activeListBoxSelectionForegroundColor() const
{
    return activeSelectionForegroundColor();
}

WebColor DefaultWebThemeEngine::inactiveListBoxSelectionBackgroundColor() const
{
    return inactiveSelectionBackgroundColor();
}

WebColor DefaultWebThemeEngine::inactiveListBoxSelectionForegroundColor() const
{
    return inactiveSelectionForegroundColor();
}

WebColor DefaultWebThemeEngine::activeTextSearchHighlightColor() const
{
    return WebColor(255, 150, 50); // Orange.
}

WebColor DefaultWebThemeEngine::inactiveTextSearchHighlightColor() const
{
    return WebColor(255, 255, 0); // Yellow.
}

WebColor DefaultWebThemeEngine::focusRingColor() const
{
    return WebColor::black;
}

WebColor DefaultWebThemeEngine::tapHighlightColor() const
{
    return TapHighLightColor;
}

static void gradientFill(cairo_t* cairo, double yStart, double yLength, bool inverted = false)
{
    double gradStartColor = BGColor1;
    double gradEndColor = BGColor2;
    if (inverted)
        std::swap(gradStartColor, gradEndColor);

    cairo_pattern_t* gradient = cairo_pattern_create_linear(0, yStart, 0, yStart + yLength);
    cairo_pattern_add_color_stop_rgb(gradient, 0, gradStartColor, gradStartColor, gradStartColor);
    cairo_pattern_add_color_stop_rgb(gradient, 1, gradEndColor, gradEndColor, gradEndColor);
    cairo_set_source(cairo, gradient);
    cairo_fill(cairo);
    cairo_pattern_destroy(gradient);
}

static void setupBorder(cairo_t * cairo, WebThemeEngine::State state)
{
    double borderColor = state == WebThemeEngine::StateHover ? BorderOnHoverColor : BorderColor;
    cairo_set_source_rgb(cairo, borderColor, borderColor, borderColor);
    cairo_set_line_width(cairo, 1);
}

void DefaultWebThemeEngine::paintButton(WebCanvas* canvas, State state, const WebRect& rect, const ButtonExtraParams&) const
{
    cairo_save(canvas);
    setupBorder(canvas, state);
    // Cairo uses a coordinate system not based on pixel coordinates, so
    // we need to add  0.5 to x and y coord or the line will stay between
    // two pixels instead of in the middle of a pixel.
    cairo_rectangle(canvas, rect.x + 0.5, rect.y + 0.5, rect.width, rect.height);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y, rect.height, state == StatePressed);
    cairo_restore(canvas);
}

void DefaultWebThemeEngine::paintTextField(WebCanvas* canvas, State, const WebRect& rect) const
{
    cairo_save(canvas);

    const double lineWidth = 2;
    const double correction = lineWidth / 2.0;

    cairo_set_line_width(canvas, lineWidth);
    cairo_set_source_rgb(canvas, TextFieldDarkBorderColor, TextFieldDarkBorderColor, TextFieldDarkBorderColor);
    cairo_move_to(canvas, rect.x + correction, rect.y + correction + rect.height);
    cairo_rel_line_to(canvas, 0, -rect.height);
    cairo_rel_line_to(canvas, rect.width, 0);
    cairo_stroke(canvas);

    cairo_set_source_rgb(canvas, TextFieldLightBorderColor, TextFieldLightBorderColor, TextFieldLightBorderColor);
    cairo_move_to(canvas, rect.x + correction + rect.width, rect.y + correction);
    cairo_rel_line_to(canvas, 0, rect.height);
    cairo_rel_line_to(canvas, -rect.width, 0);
    cairo_stroke(canvas);

    cairo_restore(canvas);
}

void DefaultWebThemeEngine::paintTextArea(WebCanvas* canvas, State state, const WebRect& rect) const
{
    paintTextField(canvas, state, rect);
}

WebSize DefaultWebThemeEngine::getCheckboxSize() const
{
    return WebSize(13, 13);
}

void DefaultWebThemeEngine::paintCheckbox(WebCanvas* canvas, State state, const WebRect& rect, const ButtonExtraParams& param) const
{
    cairo_save(canvas);
    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x + 0.5, rect.y + 0.5, rect.width, rect.height);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y, rect.height, state == StatePressed);

    if (param.checked) {
        const double border = 3;
        cairo_set_line_width(canvas, 2);
        cairo_set_source_rgb(canvas, CheckColor, CheckColor, CheckColor);
        cairo_move_to(canvas, rect.x + 0.5 + border, rect.y + 0.5 + rect.height - border);
        cairo_rel_line_to(canvas, rect.width - border * 2, -rect.height + border * 2);
        cairo_move_to(canvas, rect.x + 0.5 + border, rect.y + 0.5 + border);
        cairo_rel_line_to(canvas, rect.width - border * 2, rect.height - border * 2);
        cairo_stroke(canvas);
    }

    cairo_restore(canvas);
}

WebSize DefaultWebThemeEngine::getRadioSize() const
{
    return WebSize(13, 13);
}

void DefaultWebThemeEngine::paintRadio(WebCanvas* canvas, State state, const WebRect& rect, const ButtonExtraParams& param) const
{
    cairo_save(canvas);
    setupBorder(canvas, state);
    cairo_arc(canvas, rect.x + rect.width / 2.0, rect.y + rect.height / 2.0, rect.width / 2.0, 0, 2 * M_PI);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y, rect.height);

    if (param.checked) {
        cairo_set_source_rgb(canvas, CheckColor, CheckColor, CheckColor);
        cairo_arc(canvas, rect.x + rect.width / 2.0, rect.y + rect.height / 2.0, rect.width / 4.0, 0, 2 * M_PI);
        cairo_fill(canvas);
    }
    cairo_restore(canvas);
}

void DefaultWebThemeEngine::getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const
{
    paddingTop = MenuListBorder;
    paddingLeft = MenuListBorder;
    paddingBottom = MenuListBorder;
    paddingRight = 2 * MenuListBorder + MenuListArrowSize;
}

void DefaultWebThemeEngine::paintMenuList(WebCanvas* canvas, State state, const WebRect& rect) const
{
    cairo_save(canvas);
    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x + 0.5, rect.y + 0.5, rect.width, rect.height);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y, rect.height, state == StatePressed);

    cairo_move_to(canvas, rect.x + rect.width - MenuListArrowSize - MenuListBorder, rect.y + 1 + rect.height / 2 - MenuListArrowSize / 2);
    cairo_set_source_rgb(canvas, CheckColor, CheckColor, CheckColor);
    cairo_rel_line_to(canvas, MenuListArrowSize, 0);
    cairo_rel_line_to(canvas, -MenuListArrowSize / 2, MenuListArrowSize);
    cairo_close_path(canvas);
    cairo_fill(canvas);

    cairo_restore(canvas);
}

WebSize DefaultWebThemeEngine::getProgressBarSize() const
{
    return WebSize(2000, 300);
}

void DefaultWebThemeEngine::paintProgressBar(WebCanvas* canvas, State state, const WebRect& rect, const ProgressBarExtraParams& params) const
{
    cairo_save(canvas);

    if (params.isDeterminate) {
        // Background
        setupBorder(canvas, state);
        cairo_rectangle(canvas, rect.x, rect.y, rect.width, rect.height);
        cairo_fill(canvas);
        // Progress
        cairo_rectangle(canvas, rect.x, rect.y, rect.width * params.position, rect.height);
        gradientFill(canvas, rect.y, rect.height);
    } else {
        cairo_rectangle(canvas, rect.x, rect.y, rect.width, rect.height);
        gradientFill(canvas, rect.y, rect.height, true);
    }

    cairo_restore(canvas);
}

// These values have been copied from RenderThemeChromiumSkia.cpp
static const int progressActivityBlocks = 5;
static const int progressAnimationFrames = 10;
static const double progressAnimationInterval = 0.125;

double DefaultWebThemeEngine::getAnimationRepeatIntervalForProgressBar() const
{
    return progressAnimationInterval;
}

double DefaultWebThemeEngine::getAnimationDurationForProgressBar() const
{
    return progressAnimationInterval * progressAnimationFrames * 2; // "2" for back and forth;
}

void DefaultWebThemeEngine::getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const
{
    paddingTop = InnerSpinButtonBorder;
    paddingLeft = InnerSpinButtonBorder;
    paddingBottom = InnerSpinButtonBorder;
    paddingRight = 2 * InnerSpinButtonBorder + InnerSpinButtonArrowSize;
}

void DefaultWebThemeEngine::paintInnerSpinButton(WebCanvas* canvas, State state, const WebRect& rect, const InnerSpinButtonExtraParams& param) const
{
    double rectHalfHeight = rect.height / 2;

    cairo_save(canvas);
    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x + 0.5, rect.y + 1.5, rect.width, rectHalfHeight);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y, rectHalfHeight, state == StatePressed && param.spinUp);

    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x + 0.5, rect.y + 0.5 + rectHalfHeight, rect.width, rectHalfHeight);
    cairo_stroke_preserve(canvas);

    gradientFill(canvas, rect.y + rectHalfHeight, rectHalfHeight, state == StatePressed && !param.spinUp);

    cairo_move_to(canvas, rect.x + rect.width - InnerSpinButtonArrowSize - InnerSpinButtonBorder * 2, rect.y + rectHalfHeight + rect.height / 4 - InnerSpinButtonArrowSize + 1.5);
    cairo_set_source_rgb(canvas, CheckColor, CheckColor, CheckColor);
    cairo_rel_line_to(canvas, MenuListArrowSize, 0);
    cairo_rel_line_to(canvas, -MenuListArrowSize / 2, MenuListArrowSize);
    cairo_close_path(canvas);

    cairo_move_to(canvas, rect.x + rect.width - InnerSpinButtonArrowSize - InnerSpinButtonBorder * 2, rect.y + rect.height / 4 + InnerSpinButtonArrowSize);
    cairo_set_source_rgb(canvas, CheckColor, CheckColor, CheckColor);
    cairo_rel_line_to(canvas, MenuListArrowSize, 0);
    cairo_rel_line_to(canvas, -MenuListArrowSize / 2, -MenuListArrowSize);
    cairo_close_path(canvas);

    cairo_fill(canvas);
}

void DefaultWebThemeEngine::paintMeter(WebCanvas* canvas, State state, const WebRect& rect, const MeterExtraParams& params) const
{
    cairo_save(canvas);

    // Background
    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(canvas);
    // Meter
    cairo_rectangle(canvas, rect.x, rect.y, rect.width * params.valueRatio(), rect.height);
    gradientFill(canvas, rect.y, rect.height);

    cairo_restore(canvas);
}

const int SliderTrackHeight = 6;

void DefaultWebThemeEngine::paintSliderTrack(WebCanvas* canvas, State, const WebRect& rect) const
{
    cairo_save(canvas);
    cairo_rectangle(canvas, rect.x, rect.y + (rect.height - SliderTrackHeight) / 2.0, rect.width, SliderTrackHeight);
    gradientFill(canvas, rect.y, rect.height, true);
    cairo_restore(canvas);
}

void DefaultWebThemeEngine::paintSliderThumb(WebCanvas* canvas, State state, const WebRect& rect) const
{
    cairo_save(canvas);
    setupBorder(canvas, state);
    cairo_rectangle(canvas, rect.x, rect.y, rect.width, rect.height);
    cairo_stroke_preserve(canvas);
    gradientFill(canvas, rect.y, rect.height, state == StatePressed);
    cairo_restore(canvas);
}

}
