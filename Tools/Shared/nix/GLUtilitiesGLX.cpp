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

#include "GLUtilities.h"
#include <X11/Xlib.h>
#include <iostream>

namespace ToolsNix {

GLOffscreenBuffer::GLOffscreenBuffer(unsigned width, unsigned height)
    : m_width(width)
    , m_height(height)
    , m_display(XOpenDisplay(0))
    , m_pbuffer(0)
    , m_context(0)
{
    if (!m_display) {
        std::cerr << "Error: XOpenDisplay()\n";
        return;
    }

    static const int configAttributes[] = {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DOUBLEBUFFER, GL_FALSE,
        0
    };
    int configCount;
    GLXFBConfig* config = glXChooseFBConfig(m_display, 0, configAttributes, &configCount);
    if (!configCount) {
        std::cerr << "Error: glXChooseFBConfig()\n";
        XFree(config);
        XCloseDisplay(m_display);
        return;
    }

    static const int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH, static_cast<int>(width),
        GLX_PBUFFER_HEIGHT, static_cast<int>(height),
        0
    };
    m_pbuffer = glXCreatePbuffer(m_display, config[0], pbufferAttributes);
    if (!m_pbuffer) {
        std::cerr << "Error: glXCreatePbuffer()\n";
        XFree(config);
        XCloseDisplay(m_display);
        return;
    }

    m_context = glXCreateNewContext(m_display, config[0], GLX_RGBA_TYPE, 0, GL_TRUE);
    XFree(config);
    if (!m_context) {
        std::cerr << "Error: glXCreateNewContext()\n";
        glXDestroyPbuffer(m_display, m_pbuffer);
        XCloseDisplay(m_display);
        return;
    }
}

GLOffscreenBuffer::~GLOffscreenBuffer()
{
    if (!m_context)
        return;
    glXMakeCurrent(m_display, None, None);
    glXDestroyContext(m_display, m_context);
    glXDestroyPbuffer(m_display, m_pbuffer);
    XCloseDisplay(m_display);
}

bool GLOffscreenBuffer::makeCurrent()
{
    if (!m_context)
        return false;

    return glXMakeCurrent(m_display, m_pbuffer, m_context);
}

} // namespace ToolsNix
