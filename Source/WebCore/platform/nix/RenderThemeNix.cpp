/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
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
#include "RenderThemeNix.h"

#include "PaintInfo.h"
#include "PlatformContextCairo.h"
#include "public/WebCanvas.h"
#include "public/WebThemeEngine.h"
#include "public/WebRect.h"
#include "public/Platform.h"
#if ENABLE(PROGRESS_ELEMENT)
#include "RenderProgress.h"
#endif

namespace WebCore {

static const unsigned defaultButtonBackgroundColor = 0xffdddddd;

static void setSizeIfAuto(RenderStyle* style, const IntSize& size)
{
    if (style->width().isIntrinsicOrAuto())
        style->setWidth(Length(size.width(), Fixed));
    if (style->height().isAuto())
        style->setHeight(Length(size.height(), Fixed));
}

static WebKit::WebThemeEngine* themeEngine()
{
    return WebKit::Platform::current()->themeEngine();
}

static WebKit::WebCanvas* webCanvas(const PaintInfo& info)
{
    return info.context->platformContext()->cr();
}

PassRefPtr<RenderTheme> RenderTheme::themeForPage(Page*)
{
    return RenderThemeNix::create();
}

PassRefPtr<RenderTheme> RenderThemeNix::create()
{
    return adoptRef(new RenderThemeNix);
}

RenderThemeNix::RenderThemeNix()
    : RenderTheme()
{

}

RenderThemeNix::~RenderThemeNix()
{

}

void RenderThemeNix::systemFont(int, FontDescription&) const
{
}

static WebKit::WebThemeEngine::State getWebThemeState(const RenderTheme* theme, const RenderObject* o)
{
    if (!theme->isEnabled(o))
        return WebKit::WebThemeEngine::StateDisabled;
    if (theme->isPressed(o))
        return WebKit::WebThemeEngine::StatePressed;
    if (theme->isHovered(o))
        return WebKit::WebThemeEngine::StateHover;

    return WebKit::WebThemeEngine::StateNormal;
}

bool RenderThemeNix::paintButton(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebKit::WebThemeEngine::ButtonExtraParams extraParams;
    extraParams.isDefault = isDefault(o);
    extraParams.hasBorder = true;
    extraParams.backgroundColor = defaultButtonBackgroundColor;
    if (o->hasBackground())
        extraParams.backgroundColor = o->style()->visitedDependentColor(CSSPropertyBackgroundColor).rgb();

    themeEngine()->paintButton(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect), extraParams);
    return false;
}

bool RenderThemeNix::paintTextField(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    // WebThemeEngine does not handle border rounded corner and background image
    // so return true to draw CSS border and background.
    if (o->style()->hasBorderRadius() || o->style()->hasBackgroundImage())
        return true;

    themeEngine()->paintTextField(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect));
    return false;
}

bool RenderThemeNix::paintTextArea(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    return paintTextField(o, i, rect);
}

bool RenderThemeNix::paintCheckbox(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebKit::WebThemeEngine::ButtonExtraParams extraParams;
    extraParams.checked = isChecked(o);
    extraParams.indeterminate = isIndeterminate(o);

    themeEngine()->paintCheckbox(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect), extraParams);
    return false;
}

void RenderThemeNix::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    IntSize size = themeEngine()->getCheckboxSize();
    setSizeIfAuto(style, size);
}

bool RenderThemeNix::paintRadio(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebKit::WebThemeEngine::ButtonExtraParams extraParams;
    extraParams.checked = isChecked(o);
    extraParams.indeterminate = isIndeterminate(o);

    themeEngine()->paintRadio(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect), extraParams);
    return false;
}

void RenderThemeNix::setRadioSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    IntSize size = themeEngine()->getRadioSize();
    setSizeIfAuto(style, size);
}

bool RenderThemeNix::paintMenuList(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    themeEngine()->paintMenuList(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect));
    return false;
}

void RenderThemeNix::adjustMenuListStyle(StyleResolver*, RenderStyle* style, Element*) const
{
    style->resetBorder();
    style->setWhiteSpace(PRE);

    int paddingTop = 0;
    int paddingLeft = 0;
    int paddingBottom = 0;
    int paddingRight = 0;
    themeEngine()->getMenuListPadding(paddingTop, paddingLeft, paddingBottom, paddingRight);
    style->setPaddingTop(Length(paddingTop, Fixed));
    style->setPaddingRight(Length(paddingRight, Fixed));
    style->setPaddingBottom(Length(paddingBottom, Fixed));
    style->setPaddingLeft(Length(paddingLeft, Fixed));
}

#if ENABLE(PROGRESS_ELEMENT)
void RenderThemeNix::adjustProgressBarStyle(StyleResolver*, RenderStyle*, Element *) const
{
    style->setBoxShadow(nullptr);
}

bool RenderThemeNix::paintProgressBar(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    RenderProgress* renderProgress = toRenderProgress(o);
    WebKit::WebThemeEngine::ProgressBarExtraParams extraParams;
    extraParams.isDeterminate = renderProgress->isDeterminate();
    extraParams.position = renderProgress->position();
    extraParams.animationProgress = renderProgress->animationProgress();
    extraParams.animationStartTime = renderProgress->animationStartTime();
    themeEngine()->paintProgressBar(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect), extraParams);

    return false;
}

double RenderThemeNix::animationRepeatIntervalForProgressBar(RenderProgress*) const
{
    return themeEngine()->getAnimationRepeatIntervalForProgressBar();
}

double RenderThemeNix::animationDurationForProgressBar(RenderProgress*) const
{
    return themeEngine()->getAnimationDurationForProgressBar();
}
#endif

void RenderThemeNix::adjustInnerSpinButtonStyle(StyleResolver*, RenderStyle* style, Element*) const
{
    style->resetBorder();
    style->setWhiteSpace(PRE);

    int paddingTop = 0;
    int paddingLeft = 0;
    int paddingBottom = 0;
    int paddingRight = 0;
    themeEngine()->getInnerSpinButtonPadding(paddingTop, paddingLeft, paddingBottom, paddingRight);
    style->setPaddingTop(Length(paddingTop, Fixed));
    style->setPaddingRight(Length(paddingRight, Fixed));
    style->setPaddingBottom(Length(paddingBottom, Fixed));
    style->setPaddingLeft(Length(paddingLeft, Fixed));
}

bool RenderThemeNix::paintInnerSpinButton(RenderObject* o, const PaintInfo& i, const IntRect& rect)
{
    WebKit::WebThemeEngine::InnerSpinButtonExtraParams extraParams;
    extraParams.spinUp = isSpinUpButtonPartPressed(o);
    extraParams.readOnly = isReadOnlyControl(o);

    themeEngine()->paintInnerSpinButton(webCanvas(i), getWebThemeState(this, o), WebKit::WebRect(rect), extraParams);
    return false;
}

}
