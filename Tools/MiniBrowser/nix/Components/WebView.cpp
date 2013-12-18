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

#include "WebView.h"

#include "../BrowserControl.h"
#include <GL/gl.h>
#include <X11/cursorfont.h>

WebView::WebView(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size)
    : VisualComponent(display, control, size)
{
    createXWindow(parent, context);

    createGLContext();
    makeCurrent();
    glEnable(GL_DEPTH_TEST);
}

WebView::~WebView()
{
    destroyGLContext();
    XDestroyWindow(m_display, m_window);
}

void WebView::createXWindow(Window parent, XContext context)
{
    XVisualInfo visualInfoTemplate;
    int visualInfoCount;
    visualInfoTemplate.visualid = setupXVisualID();
    XVisualInfo* visualInfo = XGetVisualInfo(m_display, VisualIDMask, &visualInfoTemplate, &visualInfoCount);
    if (!visualInfo)
        fatalError("couldn't get X visual\n");

    XSetWindowAttributes setAttributes;
    setAttributes.colormap = XCreateColormap(m_display, parent, visualInfo->visual, AllocNone);
    setAttributes.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    m_window = XCreateWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &setAttributes);
    XFree(visualInfo);

    XMapWindow(m_display, m_window);
    XSaveContext(m_display, m_window, context, (XPointer)this);
}

void WebView::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case Expose:
        m_control->handleExposeEvent();
        break;
    case ButtonPress:
        m_control->handleButtonPressEvent(reinterpret_cast<const XButtonPressedEvent&>(event));
        break;
    case ButtonRelease:
        m_control->handleButtonReleaseEvent(reinterpret_cast<const XButtonReleasedEvent&>(event));
        break;
    case MotionNotify:
        m_control->handlePointerMoveEvent(reinterpret_cast<const XPointerMovedEvent&>(event));
        break;
    }
}

static unsigned cursorShape(int type)
{
    static unsigned cursorShapes[] = {
        XC_left_ptr,                // "cursor/pointer",
        XC_X_cursor,                // "cursor/cross",
        XC_hand2,                   // "cursor/hand",
        XC_xterm,                   // "cursor/i_beam",
        XC_watch,                   // "cursor/wait",
        XC_question_arrow,          // "cursor/help",
        XC_right_side,              // "cursor/east_resize",
        XC_top_side,                // "cursor/north_resize",
        XC_top_right_corner,        // "cursor/north_east_resize",
        XC_top_left_corner,         // "cursor/north_west_resize",
        XC_bottom_side,             // "cursor/south_resize",
        XC_bottom_right_corner,     // "cursor/south_east_resize",
        XC_bottom_left_corner,      // "cursor/south_west_resize",
        XC_left_side,               // "cursor/west_resize",
        XC_sb_v_double_arrow,       // "cursor/north_south_resize",
        XC_sb_h_double_arrow,       // "cursor/east_west_resize",
        XC_left_ptr,         // "cursor/north_east_south_west_resize",
        XC_left_ptr,         // "cursor/north_west_south_east_resize",
        XC_left_ptr,         // "cursor/column_resize",
        XC_left_ptr,         // "cursor/row_resize",
        XC_tcross,                  // "cursor/middle_panning",
        XC_right_tee,               // "cursor/east_panning",
        XC_top_tee,                 // "cursor/north_panning",
        XC_ur_angle,                // "cursor/north_east_panning",
        XC_ul_angle,                // "cursor/north_west_panning",
        XC_bottom_tee,              // "cursor/south_panning",
        XC_lr_angle,                // "cursor/south_east_panning",
        XC_ll_angle,                // "cursor/south_west_panning",
        XC_left_tee,                // "cursor/west_panning",
        XC_fleur,                   // "cursor/move",                     duplicated!
        XC_left_ptr,         // "cursor/vertical_text",
        XC_left_ptr,         // "cursor/cell",
        XC_left_ptr,         // "cursor/context_menu",
        XC_left_ptr,         // "cursor/alias",
        XC_exchange,               // "cursor/progress",
        XC_left_ptr,         // "cursor/no_drop",
        XC_left_ptr,         // "cursor/copy",
        XC_left_ptr,         // "cursor/none",
        XC_X_cursor,                // "cursor/not_allowed",
        XC_X_cursor,         // "cursor/zoom_in",
        XC_X_cursor,         // "cursor/zoom_out",
        XC_fleur,                   // "cursor/grab",                     duplicated!
        XC_fleur,                   // "cursor/grabbing",                 duplicated!
        XC_left_ptr          // custom type
    };
    return cursorShapes[type];
}

void WebView::setCursor(unsigned shape)
{
    XDefineCursor(m_display, m_window, XCreateFontCursor(m_display, cursorShape(shape)));
}
