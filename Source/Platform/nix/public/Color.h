/*
 *  Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef Nix_Color_h
#define Nix_Color_h

#include <algorithm>

namespace Nix {

typedef unsigned RGBA32; // RGBA quadruplet

struct Color {
    static const RGBA32 black = 0xFF000000;
    static const RGBA32 white = 0xFFFFFFFF;
    static const RGBA32 darkGray = 0xFF808080;
    static const RGBA32 gray = 0xFFA0A0A0;
    static const RGBA32 lightGray = 0xFFC0C0C0;
    static const RGBA32 transparent = 0x00000000;

    RGBA32 rgba;

    Color()
        : rgba(Color::black)
    {
    }

    Color(RGBA32 rgba)
        : rgba(rgba)
    {
    }

    Color(int r, int g, int b, int a = 255)
        : rgba(std::max(0, std::min(a, 255)) << 24 | std::max(0, std::min(r, 255)) << 16 | std::max(0, std::min(g, 255)) << 8 | std::max(0, std::min(b, 255)))
    {
    }

    operator RGBA32() const
    {
        return rgba;
    }

    unsigned char r() const
    {
        return (rgba & 0x00FF0000) >> 16;
    }

    unsigned char g() const
    {
        return (rgba & 0x0000FF00) >> 8;
    }

    unsigned char b() const
    {
        return (rgba & 0x000000FF);
    }

    unsigned char a() const
    {
        return (rgba & 0xFF000000) >> 24;
    }
};

inline bool operator==(const Color& a, const Color& b)
{
    return a.rgba == b.rgba;
}

inline bool operator!=(const Color& a, const Color& b)
{
    return !(a == b);
}

} // namespace Nix

#endif
