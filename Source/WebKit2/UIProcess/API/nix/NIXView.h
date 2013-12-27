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

#ifndef NIXView_h
#define NIXView_h

#include "NIXEvents.h"
#include <WebKit2/WKBase.h>
#include <WebKit2/WKContext.h>
#include <WebKit2/WKContextMenuItem.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKPageGroup.h>
#include <WebKit2/WKPopupItem.h>
#include <WebKit2/WKPopupMenuListener.h>
#include <WebKit2/WKView.h>
#include <WebKit2/WKViewportAttributes.h>

#ifdef __cplusplus
extern "C" {
#endif

enum NIXInputMethodHint {
    NIXImhNone                   = 0x000,
    NIXImhDialableCharactersOnly = 0x001,
    NIXImhDigitsOnly             = 0x002,
    NIXImhEmailCharactersOnly    = 0x004,
    NIXImhNoAutoUppercase        = 0x008,
    NIXImhUrlCharactersOnly      = 0x010,
    NIXImhNoPredictiveText       = 0x020,
    NIXImhSensitiveData          = 0x040
};

struct NIXTextInputState {
    WKStringRef selectedText;
    WKStringRef surroundingText;
    WKStringRef submitLabel;
    uint64_t inputMethodHints;
    bool isContentEditable;
    unsigned cursorPosition;
    unsigned anchorPosition;
    WKRect cursorRect;
    WKRect editorRect;
};
typedef struct NIXTextInputState NIXTextInputState;

// NIXViewClient.
typedef void (*NIXViewCallback)(WKViewRef, const void* clientInfo);
typedef void (*NIXViewDoneWithTouchEventCallback)(WKViewRef view, const NIXTouchEvent* touchEvent, bool wasEventHandled, const void* clientInfo);
typedef void (*NIXViewPageDidFindZoomableAreaCallback)(WKViewRef view, WKPoint target, WKRect area, const void* clientInfo);
typedef void (*NIXViewPageUpdateTextInputStateCallback)(WKViewRef view, const NIXTextInputState* state, const void* clientInfo);
typedef void (*NIXViewSetCursor)(WKViewRef view, unsigned int shape, const void* clientInfo);
typedef void (*NIXViewDidChangeTooltipCallback)(WKViewRef view, const WKStringRef tooltip, const void* clientInfo);

typedef struct NIXViewClientBase {
    int                                              version;
    const void*                                      clientInfo;
} NIXViewClientBase;

typedef struct NIXViewClientV0 {
    NIXViewClientBase                                base;

    // Version 0
    NIXViewDoneWithTouchEventCallback                doneWithTouchEvent;
    NIXViewPageDidFindZoomableAreaCallback           didFindZoomableArea;
    NIXViewPageUpdateTextInputStateCallback          updateTextInputState;
    NIXViewSetCursor                                 setCursor;
    NIXViewDidChangeTooltipCallback                  didChangeTooltip;
    NIXViewCallback                                  didCommitLoadForMainFrame;
} NIXViewClientV0;

WK_EXPORT float NIXViewGetScaleToFitContents(WKViewRef);

WK_EXPORT void NIXViewScaleToFitContents(WKViewRef);

WK_EXPORT void NIXViewSetNixViewClient(WKViewRef view, const NIXViewClientBase* viewClient);

WK_EXPORT WKSize NIXViewVisibleContentsSize(WKViewRef view);

WK_EXPORT void NIXViewSetScreenRect(WKViewRef, WKRect);

WK_EXPORT void NIXViewFindZoomableAreaForPoint(WKViewRef view, WKPoint point, int horizontalRadius, int verticalRadius);

WK_EXPORT void NIXViewSendMouseEvent(WKViewRef view, const NIXMouseEvent* event);
WK_EXPORT void NIXViewSendWheelEvent(WKViewRef view, const NIXWheelEvent* event);
WK_EXPORT void NIXViewSendKeyEvent(WKViewRef view, const NIXKeyEvent* event);
WK_EXPORT void NIXViewSendTouchEvent(WKViewRef view, const NIXTouchEvent* event);
WK_EXPORT void NIXViewSendGestureEvent(WKViewRef view, const NIXGestureEvent* event);

WK_EXPORT bool NIXViewIsSuspended(WKViewRef view);

WK_EXPORT WKSize NIXViewportAttributesGetSize(WKViewportAttributesRef);
WK_EXPORT double NIXViewportAttributesGetMinimumScale(WKViewportAttributesRef);
WK_EXPORT double NIXViewportAttributesGetMaximumScale(WKViewportAttributesRef);
WK_EXPORT double NIXViewportAttributesGetInitialScale(WKViewportAttributesRef);
WK_EXPORT bool NIXViewportAttributesGetIsUserScalable(WKViewportAttributesRef);

WK_EXPORT void NIXViewViewportInteractionStart(WKViewRef view);
WK_EXPORT void NIXViewViewportInteractionStop(WKViewRef view);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // NIXView_h
