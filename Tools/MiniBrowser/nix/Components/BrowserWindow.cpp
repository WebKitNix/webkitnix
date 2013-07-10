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

#include "BrowserWindow.h"

#include "../BrowserControl.h"

Atom wmDeleteMessageAtom;

BrowserWindow::BrowserWindow(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size)
    : VisualComponent(display, control, size)
{
    createXWindow(parent, context);
}

void BrowserWindow::setTitle(const char* title)
{
    XStoreName(m_display, m_window, title);
}

void BrowserWindow::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 0, 0, 0);

    wmDeleteMessageAtom = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &wmDeleteMessageAtom, 1);

    XSelectInput(m_display, m_window, ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask);
    XSaveContext(m_display, m_window, context, (XPointer)this);
    XMapWindow(m_display, m_window);
}

void BrowserWindow::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case KeyPress:
        m_control->handleKeyPressEvent(event);
        break;
    case KeyRelease:
        m_control->handleKeyReleaseEvent(event);
        break;
    case ConfigureNotify:
        updateSizeIfNeeded(event.xconfigure.width, event.xconfigure.height);
        break;
    case ClientMessage:
        if (event.xclient.data.l[0] == wmDeleteMessageAtom)
            m_control->handleClose();
        break;
    }
}

void BrowserWindow::updateSizeIfNeeded(int width, int height)
{
    if (width == m_size.size.width && height == m_size.size.height)
        return;

    m_size.size.width = width;
    m_size.size.height = height;

    m_control->handleSizeChanged(width, height);
}

ToolBar::ToolBar(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size) :
    VisualComponent(display, control, size)
{
    createXWindow(parent, context);
}

void ToolBar::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 0, 0, createXColor("#E1E1E1"));

    XMapWindow(m_display, m_window);
}
