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

#ifndef GamepadController_h
#define GamepadController_h

#include "JSWrappable.h"
#include <JavaScriptCore/JSObjectRef.h>
#include <wtf/Platform.h>
#include <wtf/RefPtr.h>

#if PLATFORM(NIX)
#include <public/Gamepads.h>
#endif

namespace WTR {

class GamepadController : public JSWrappable {
public:
    static PassRefPtr<GamepadController> create();
    ~GamepadController();

    void makeWindowObject(JSContextRef, JSObjectRef windowObject, JSValueRef* exception);
    virtual JSClassRef wrapperClass();

    // Bound methods
    void connect(int index);
    void disconnect(int index);
    void setId(int index, JSStringRef id);
    void setButtonCount(int index, int button);
    void setButtonData(int index, int button, float buttonData);
    void setAxisCount(int index, int axis);
    void setAxisData(int index, int button, float axisData);

#if PLATFORM(NIX)
    void sampleGamepads(Nix::Gamepads& into);
#endif

private:
    GamepadController();

#if PLATFORM(NIX)
    Nix::Gamepads m_gamepads;
#endif
};

} // namespace WTR
#endif // GamepadController_h
