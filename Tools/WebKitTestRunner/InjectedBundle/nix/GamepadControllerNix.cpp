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
#include "GamepadController.h"

#include "PlatformClient.h"
#include "StringFunctions.h"


namespace WTR {

static bool isValidIndex(int index)
{
    return index >= 0 && index < Nix::Gamepads::itemsLengthCap;
}

GamepadController::GamepadController()
{
    PlatformClient::current()->registerGamepadController(this);
}

GamepadController::~GamepadController()
{
}

void GamepadController::connect(int index)
{
    if (!isValidIndex(index))
        return;

    m_gamepads.items[index].connected = true;
    m_gamepads.length++;
}

void GamepadController::disconnect(int index)
{
    if (m_gamepads.length == 0)
        return;

    if (!isValidIndex(index))
        return;

    m_gamepads.items[index].connected = false;
    m_gamepads.length--;
}

void GamepadController::setId(int index, JSStringRef id)
{
    if (!isValidIndex(index))
        return;

    char buffer[Nix::Gamepad::idLengthCap];
    size_t size = JSStringGetUTF8CString(id, reinterpret_cast<char*>(&buffer), Nix::Gamepad::idLengthCap);

    for (unsigned i = 0; i <= size; ++i) {
        m_gamepads.items[index].id[i] = buffer[i];
    }
}

void GamepadController::setButtonCount(int index, int buttons)
{
    if (!isValidIndex(index))
        return;

    if (buttons < 0)
        return;

    m_gamepads.items[index].buttonsLength = buttons;
}

void GamepadController::setButtonData(int index, int button, float buttonData)
{
    if (!isValidIndex(index))
        return;

    if (button < 0 || button >= m_gamepads.items[index].buttonsLength)
        return;

    m_gamepads.items[index].buttons[button] = buttonData;
}

void GamepadController::setAxisCount(int index, int axis)
{
    if (!isValidIndex(index))
        return;

    if (axis < 0)
        return;

    m_gamepads.items[index].axesLength = axis;
}

void GamepadController::setAxisData(int index, int axis, float axisData)
{
    if (!isValidIndex(index))
        return;

    if (axis < 0 || axis >= m_gamepads.items[index].axesLength)
        return;

    m_gamepads.items[index].axes[axis] = axisData;
}

void GamepadController::sampleGamepads(Nix::Gamepads& into)
{
    for (unsigned i = 0; i < Nix::Gamepads::itemsLengthCap; ++i)
        memcpy(&into.items[i], &m_gamepads.items[i], sizeof(m_gamepads.items[i]));

    into.length = m_gamepads.length;
}

} // namespace WTR
