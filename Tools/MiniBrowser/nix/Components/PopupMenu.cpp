/*
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Zsolt Borbely <borbezs@inf.u-szeged.hu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "PopupMenu.h"

#include "../BrowserControl.h"
#include <stdio.h>

PopupMenu::PopupMenu(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size, std::vector<std::string>* menuItems)
    : VisualComponent(display, control, size)
    , m_xContext(context)
    , m_menuItems(menuItems)
    , m_countOfVisibleEntries(numberOfMenuItems())
    , m_section(0)
    , m_tooMuchEntries(false)
    , m_hasScrollButtons(false)
    , m_scrollUpButton(0)
    , m_scrollDownButton(0)
{

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), 0, 0);
    m_cairo = cairo_create(m_surface);
    cairo_select_font_face(m_cairo, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 12);

    createXWindow(parent, context);

    if (m_countOfVisibleEntries > 0)
        drawMenuEntries();
    else
        fprintf(stderr, "Not enough space for menuentries.\n");
}

PopupMenu::~PopupMenu()
{
    delete m_scrollUpButton;
    delete m_scrollDownButton;
    removeMenuEntries();

    cairo_surface_destroy(m_surface);
    cairo_destroy(m_cairo);
}

void PopupMenu::removeMenuEntries()
{
    std::vector<MenuEntry*>::iterator it = m_menuEntries.begin();
    std::vector<MenuEntry*>::iterator last = m_menuEntries.end();

    for (; it != last; ++it)
        delete *it;

    m_menuEntries.clear();
}

int PopupMenu::numberOfMenuItems()
{
    return m_menuItems->size();
}

void PopupMenu::createXWindow(Window parent, XContext context)
{
    calculateWindowSize();
    correctWindowWidth();

    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, borderWidth, createXColor("#555555"), createXColor("#EFEFEF"));
    XMapWindow(m_display, m_window);
}

void PopupMenu::scrollUp()
{
    setVisibleSection(Backward);
}

void PopupMenu::scrollDown()
{
    setVisibleSection(Forward);
}

Button* PopupMenu::createScrollButton(int x, int y, const char* imageName, ButtonFunction callback)
{
    WKRect buttonRect = WKRectMake(x, y, m_size.size.width, entryHeight);
    std::string imagePath(MINIBROWSER_ICON_DIR);
    imagePath.append(imageName);

    Button* button = new Button(m_display, m_window, m_xContext, m_control, buttonRect, imagePath.c_str(), callback);
    return button;
}

void PopupMenu::drawMenuEntries(int firstElement)
{
    int lastElement = firstElement + m_countOfVisibleEntries;
    int elementIndex = 0;

    if (m_tooMuchEntries) {
        if (!m_hasScrollButtons) {
            m_hasScrollButtons = true;

            const int downButtonYPosition = (m_countOfVisibleEntries + 1) * entryHeight;
            m_scrollUpButton = createScrollButton(0, 0, "up.png", std::bind(&PopupMenu::scrollUp, this));
            m_scrollDownButton = createScrollButton(0, downButtonYPosition + entryBorderSize, "down.png", std::bind(&PopupMenu::scrollDown, this));
        }
        elementIndex = 1;
    }

    for (int i = firstElement; i < lastElement; ++i) {
        m_menuEntries.push_back(new MenuEntry(m_display, m_window, m_xContext, m_control, WKRectMake(0, elementIndex * entryHeight, m_size.size.width, entryHeight), m_menuItems->at(i), i));
        ++elementIndex;
    }
}

void PopupMenu::setVisibleSection(ScrollDirection direction)
{
    int currentSection = m_section;

    m_section += direction;

    if (m_section < 0)
        m_section = 0;

    if (m_section > (int)(numberOfMenuItems() / m_countOfVisibleEntries))
        m_section = (int)(numberOfMenuItems() / m_countOfVisibleEntries);

    if (currentSection == m_section)
        return;

    int start = (m_section * m_countOfVisibleEntries);

    if (start > numberOfMenuItems() - m_countOfVisibleEntries)
        start = numberOfMenuItems() - m_countOfVisibleEntries;

    removeMenuEntries();
    drawMenuEntries(start);
}

void PopupMenu::correctWindowWidth()
{
    const int textOffset = 16;
    cairo_text_extents_t extents;

    std::vector<std::string>::iterator it = m_menuItems->begin();
    std::vector<std::string>::iterator last = m_menuItems->end();

    for (; it != last; ++it) {
        cairo_text_extents(m_cairo, (*it).c_str(), &extents);
        if (extents.x_advance > m_size.size.width)
            m_size.size.width = extents.x_advance + textOffset;
    }
}

void PopupMenu::calculateWindowSize()
{
    const int comboboxHeight = m_size.size.height;
    m_size.size.height = numberOfMenuItems() * entryHeight;

    bool opensDown = (m_control->webViewSize().height / 2) > m_size.origin.y;
    const int numberOfScrollButtons = 2;
    const int distanceFromEdge = entryHeight;
    const int startPoint = opensDown ? m_size.origin.y : distanceFromEdge;
    const int endPoint = opensDown ? (m_control->webViewSize().height - distanceFromEdge) : (m_size.origin.y - comboboxHeight);

    if (startPoint + ((numberOfMenuItems() + numberOfScrollButtons) * entryHeight) >= endPoint) {
        m_tooMuchEntries = true;
        const int countOfFittingElements = (int)((endPoint - startPoint) / entryHeight);
        m_countOfVisibleEntries = countOfFittingElements - numberOfScrollButtons;
        m_size.size.height = countOfFittingElements * entryHeight;
    }

    m_size.origin.y = opensDown ? startPoint : endPoint - m_size.size.height;

    // Adapting the size of the window according to the borderwidth.
    m_size.origin.x -= borderWidth;
    m_size.origin.y -= 2 * borderWidth;
    m_size.size.height += entryBorderSize;
}
