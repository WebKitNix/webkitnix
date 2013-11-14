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
#include <iostream>

using std::cerr;

namespace ToolsNix {

GLOffscreenBuffer::GLOffscreenBuffer(unsigned width, unsigned height)
    : m_width(width)
    , m_height(height)
    , m_display(0)
    , m_context(0)
    , m_surface(0)
{
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!m_display) {
        cerr << "Error: eglGetDisplay()\n";
        return;
    }

    if (!eglInitialize(m_display, 0, 0)) {
        cerr << "Error: eglInitialize()\n";
        return;
    }

    static const EGLint attributes[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint configCount;
    if (!eglChooseConfig(m_display, attributes, &config, 1, &configCount)) {
        cerr << "Error: eglChooseConfig()\n";
        return;
    }

    if (configCount != 1) {
        cerr << "Error: couldn't get a valid config for EGL\n";
        return;
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        cerr << "Error: eglBindAPI()\n";
        return;
    }

    static const EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE,
    };

    m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, contextAttributes);
    if (!m_context) {
        cerr << "Error: eglCreateContext()\n";
        return;
    }

    static const EGLint surfaceAttributes[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_NONE
    };

    m_surface = eglCreatePbufferSurface(m_display, config, surfaceAttributes);
    if (!m_surface) {
        cerr << "Error: eglCreatePbufferSurface()\n";
        return;
    }
}

GLOffscreenBuffer::~GLOffscreenBuffer()
{
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(m_display, m_context);
    eglDestroySurface(m_display, m_surface);
    eglTerminate(m_display);
}

bool GLOffscreenBuffer::makeCurrent()
{
    if (!m_surface)
        return false;

    return eglMakeCurrent(m_display, m_surface, m_surface, m_context);
}

} // namespace ToolsNix
