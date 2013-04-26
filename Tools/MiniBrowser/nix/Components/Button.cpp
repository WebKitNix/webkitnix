/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Roland Takacs <rtakacs@inf.u-szeged.hu>
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

#include "Button.h"

#include "../BrowserControl.h"

Button::Button(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size, ButtonType type)
    : VisualComponent(display, control, size)
    , m_type(type)
{
    createXWindow(parent, context);

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), m_size.size.width, m_size.size.height);
    m_image = cairo_image_surface_create_from_png(imagePath(type));
    m_cairo = cairo_create(m_surface);
}

Button::~Button()
{
    cairo_surface_destroy(m_surface);
    cairo_surface_destroy(m_image);
    cairo_destroy(m_cairo);
}

void Button::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 0, 0, createXColor("#E1E1E1"));

    XSelectInput(m_display, m_window, ExposureMask | ButtonPressMask | ButtonReleaseMask);
    XSaveContext(m_display, m_window, context, (XPointer)this);
    XMapWindow(m_display, m_window);
}

const char* Button::imagePath(ButtonType type)
{
    switch (type) {
    case Back:
        return MINIBROWSER_ICON_DIR "/previous.png";
    case Forward:
        return MINIBROWSER_ICON_DIR "/next.png";
        break;
    case Refresh:
        return MINIBROWSER_ICON_DIR "/refresh.png";
    default:
        return 0;
    }
}

void Button::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case Expose:
        drawImage();
        break;
    case ButtonRelease:
        navigate();
        break;
    }
}

void Button::drawImage()
{
    cairo_set_source_surface(m_cairo, m_image, 1, 1);
    cairo_paint(m_cairo);
}

void Button::navigate()
{
    switch (m_type) {
    case Back:
        m_control->pageGoBack();
        break;
    case Forward:
        m_control->pageGoForward();
        break;
    case Refresh:
        m_control->pageReload();
        break;
    }
}
