/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Attila Dusnoki <adusnoki@inf.u-szeged.hu>
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

#include "Tooltip.h"

#include "../BrowserControl.h"

Tooltip::Tooltip(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size)
    : VisualComponent(display, control, size)
{
    createXWindow(parent, context);

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), 0, 0);
    m_cairo = cairo_create(m_surface);
    cairo_select_font_face(m_cairo, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 12);
}

Tooltip::~Tooltip()
{
    cairo_surface_destroy(m_surface);
    cairo_destroy(m_cairo);
}

void Tooltip::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, 0, 0, 1 /*width*/, 1 /*height*/, 1 /*border*/, createXColor("#C1C1C1"), BlackPixel(m_display, 0));
}

void Tooltip::resizeWindow(int mouseX, int mouseY)
{
    WKSize webViewSize = m_control->webViewSize();

    m_size.size.width = widthOfLongestLine() + textWidthIndent * 2;
    m_size.size.height = lineHeightSize * m_wrappedText.size() + lineHeightSize / 2;
    m_size.origin.x = mouseX + windowSlideOffsetX(webViewSize, mouseX);
    m_size.origin.y = mouseY + windowSlideOffsetY(webViewSize, mouseY);

    cairo_xlib_surface_set_size(m_surface, m_size.size.width, m_size.size.height);
    XMoveResizeWindow(m_display, m_window, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height);
}

// In case the tooltip would slide out of the window, we need an offset
// that corrects the position.
int Tooltip::windowSlideOffsetX(WKSize webViewSize, int mouseX)
{
    int endOfWindowWidth = mouseX + m_size.size.width;

    if (webViewSize.width > endOfWindowWidth + mousePositionXOffset)
        return mousePositionXOffset;

    return webViewSize.width - endOfWindowWidth;
}

int Tooltip::windowSlideOffsetY(WKSize webViewSize, int mouseY)
{
    int endOfWindowHeight = mouseY + m_size.size.height;

    if (webViewSize.height > endOfWindowHeight + mousePositionYOffset)
        return mousePositionYOffset;

    return webViewSize.height - endOfWindowHeight;
}

int Tooltip::textWidth(const char* text)
{
    cairo_text_extents_t extents;
    cairo_text_extents(m_cairo, text, &extents);

    return extents.x_advance;
}

void Tooltip::showTooltip(int mouseX, int mouseY, std::string text)
{
    static std::string lastText;

    if (lastText.compare(text)) {
        lastText = text;
        wordWrap(text);
    }

    resizeWindow(mouseX, mouseY);
    XMapWindow(m_display, m_window);

    int numberOfLines = m_wrappedText.size();
    for (int line = 0; line < numberOfLines; ++line)
        drawText(m_wrappedText.at(line), line);
}

void Tooltip::hideTooltip()
{
    XUnmapWindow(m_display, m_window);
}

void Tooltip::drawText(std::string& text, int line)
{
    cairo_set_source_rgb(m_cairo, 1, 1, 1);
    cairo_move_to(m_cairo, textWidthIndent, lineHeightSize * line + lineHeightSize);
    cairo_show_text(m_cairo, text.c_str());
}

void Tooltip::wordWrap(std::string text)
{
    m_wrappedText.clear();

    if (text.length() < maximumCharactersPerLine) {
        m_wrappedText.push_back(text);
        return;
    }

    int characterPosition = maximumCharactersPerLine;
    int lastSpacePosition = 0;

    while (characterPosition < text.length()) {
        int spacePosition = text.rfind(' ', characterPosition);

        // If there is too long word.
        if (spacePosition < lastSpacePosition) {
            characterPosition++;
            continue;
        }

        if (spacePosition == std::string::npos)
            spacePosition = text.find(' ', characterPosition);

        if (spacePosition != std::string::npos) {
            m_wrappedText.push_back(text.substr(lastSpacePosition, spacePosition - lastSpacePosition));
            lastSpacePosition = spacePosition + 1;
            characterPosition = spacePosition + maximumCharactersPerLine;
        }
    }

    m_wrappedText.push_back(text.substr(lastSpacePosition, text.length()));
}

int Tooltip::widthOfLongestLine()
{
    std::vector<std::string>::iterator it = m_wrappedText.begin();
    std::vector<std::string>::iterator last = m_wrappedText.end();
    std::string longestLineText;

    for (; it != last; ++it) {
        if (textWidth(it->c_str()) > textWidth(longestLineText.c_str()))
            longestLineText = *it;
    }

    return textWidth(longestLineText.c_str());
}
