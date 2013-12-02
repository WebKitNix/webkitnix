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

#include "MenuEntry.h"

#include "../BrowserControl.h"
#include <X11/cursorfont.h>

MenuEntry::MenuEntry(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size, std::string entryName, int entryID)
    : VisualComponent(display, control, size)
    , m_entryName(entryName)
    , m_entryID(entryID)
{
    createXWindow(parent, context);

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), m_size.size.width, m_size.size.height);
    m_cairo = cairo_create(m_surface);
    cairo_select_font_face(m_cairo, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 12);

    XDefineCursor(m_display, m_window, XCreateFontCursor(m_display, XC_left_ptr));

    drawText(m_entryName);
}

MenuEntry::~MenuEntry()
{
    cairo_surface_destroy(m_surface);
    cairo_destroy(m_cairo);
}

void MenuEntry::createXWindow(Window parent, XContext context)
{
    correctWindowSize();
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, entryBorderSize, createXColor("#444444"), createXColor("#EFEFEF"));

    XSelectInput(m_display, m_window, ButtonPressMask | ButtonReleaseMask);
    XSaveContext(m_display, m_window, context, (XPointer)this);
    XMapWindow(m_display, m_window);
}

void MenuEntry::correctWindowSize()
{
    m_size.origin.x -= entryBorderSize;
    m_size.size.height -= entryBorderSize;
}

void MenuEntry::handleEvent(const XEvent& event)
{
    if (event.type == ButtonRelease)
        passMenuEntryValue(m_entryID);
}

void MenuEntry::passMenuEntryValue(int selectedItem)
{
    Atom messageType = XInternAtom(m_display, "MenuEntry", False);
    XClientMessageEvent itemSelectedEvent;

    itemSelectedEvent.type = ClientMessage;
    itemSelectedEvent.format = 16;
    itemSelectedEvent.data.s[0] = selectedItem;
    itemSelectedEvent.message_type = messageType;

    XSendEvent(m_display, 0, False, 0, (XEvent*)&itemSelectedEvent);
}

void MenuEntry::drawText(std::string label)
{
    const int textOffsetX = 4;
    const int textOffsetY = 13;

    cairo_set_source_rgb(m_cairo, 0.2, 0.2, 0.2);
    cairo_move_to(m_cairo, textOffsetX, textOffsetY);
    cairo_show_text(m_cairo, label.c_str());
}
