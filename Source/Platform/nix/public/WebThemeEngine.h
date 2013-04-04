/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#ifndef WebThemeEngine_h
#define WebThemeEngine_h

#include "WebSize.h"
#include "WebCanvas.h"

namespace WebKit {

typedef unsigned WebColor;

class WebRect;

class WebThemeEngine {
public:
    // The current state of the associated Part.
    enum State {
        StateDisabled,
        StateHover,
        StateNormal,
        StatePressed
    };

    // Extra parameters for drawing the PartScrollbarHorizontalTrack and
    // PartScrollbarVerticalTrack.
    struct ScrollbarTrackExtraParams {
        // The bounds of the entire track, as opposed to the part being painted.
        int trackX;
        int trackY;
        int trackWidth;
        int trackHeight;
    };

    // Extra parameters for PartCheckbox, PartPushButton and PartRadio.
    struct ButtonExtraParams {
        bool checked;
        bool indeterminate; // Whether the button state is indeterminate.
        bool isDefault; // Whether the button is default button.
        bool hasBorder;
        WebColor backgroundColor;
    };

    // Extra parameters for PartMenuList
    struct MenuListExtraParams {
        bool hasBorder;
        bool hasBorderRadius;
        int arrowX;
        int arrowY;
        WebColor backgroundColor;
    };

    // Extra parameters for PartSliderTrack and PartSliderThumb
    struct SliderExtraParams {
        bool vertical;
        bool inDrag;
    };

    // Extra parameters for PartInnerSpinButton
    struct InnerSpinButtonExtraParams {
        bool spinUp;
        bool readOnly;
    };

    // Extra parameters for PartProgressBar
    struct ProgressBarExtraParams {
        bool determinate;
        int valueRectX;
        int valueRectY;
        int valueRectWidth;
        int valueRectHeight;
    };

    virtual void paintButton(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const = 0;
    virtual void paintTextField(WebCanvas*, State, const WebRect&) const = 0;
    virtual WebSize getCheckboxSize() const = 0;
    virtual void paintCheckbox(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const = 0;
    virtual WebSize getRadioSize() const = 0;
    virtual void paintRadio(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const = 0;
    virtual void paintTextArea(WebCanvas*, State, const WebRect&) const = 0;
    virtual void paintMenuList(WebCanvas*, State, const WebRect&) const = 0;
};

} // namespace WebKit

#endif
