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
#include <WebKit2/WKContext.h>
#include <WebKit2/WKContextMenuItem.h>
#include <WebKit2/WKCoordinatedScene.h>
#include <WebKit2/WKPage.h>
#include <WebKit2/WKPageGroup.h>
#include <WebKit2/WKPopupItem.h>
#include <WebKit2/WKPopupMenuListener.h>

#ifdef __cplusplus
extern "C" {
#endif

struct NIXMatrix {
    double xx; double yx;
    double xy; double yy;
    double x0; double y0;
};
typedef struct NIXMatrix NIXMatrix;

WK_INLINE NIXMatrix NIXMatrixMakeTranslation(double x0, double y0)
{
    NIXMatrix m;
    m.xx = 1; m.xy = 0; m.x0 = x0;
    m.yx = 0; m.yy = 1; m.y0 = y0;
    return m;
}

enum NixInputMethodHint {
    NixImhNone                   = 0x000,
    NixImhDialableCharactersOnly = 0x001,
    NixImhDigitsOnly             = 0x002,
    NixImhEmailCharactersOnly    = 0x004,
    NixImhNoAutoUppercase        = 0x008,
    NixImhUrlCharactersOnly      = 0x010,
    NixImhNoPredictiveText       = 0x020,
    NixImhSensitiveData          = 0x040
};

typedef struct OpaqueNIXView* NIXView;

// NIXViewClient.
typedef void (*NIXViewCallback)(NIXView view, const void* clientInfo);
typedef void (*NIXViewViewNeedsDisplayCallback)(NIXView view, WKRect area, const void* clientInfo);
typedef void (*NIXViewWebProcessCrashedCallback)(NIXView view, WKStringRef url, const void* clientInfo);
typedef void (*NIXViewDoneWithTouchEventCallback)(NIXView view, const NIXTouchEvent* touchEvent, bool wasEventHandled, const void* clientInfo);
typedef void (*NIXViewDoneWithGestureEventCallback)(NIXView view, const NIXGestureEvent* gestureEvent, bool wasEventHandled, const void* clientInfo);
typedef void (*NIXViewPageDidRequestScrollCallback)(NIXView view, WKPoint position, const void* clientInfo);
typedef void (*NIXViewPageDidChangeContentsSizeCallback)(NIXView view, WKSize size, const void* clientInfo);
typedef void (*NIXViewPageDidChangeViewportAttributesCallback)(NIXView view, float width, float height, float minimumScale, float maximumScale, float initialScale, int userScalable, const void* clientInfo);
typedef void (*NIXViewPageDidFindZoomableAreaCallback)(NIXView view, WKPoint target, WKRect area, const void* clientInfo);
typedef void (*NIXViewPageUpdateTextInputStateCallback)(NIXView view, WKStringRef selectedText, WKStringRef surroundingText, uint64_t inputMethodHints, bool isContentEditable, WKRect cursorRect, WKRect editorRect, const void* clientInfo);

struct NIXViewClient {
    int                                              version;
    const void*                                      clientInfo;

    // Version 0
    NIXViewViewNeedsDisplayCallback                  viewNeedsDisplay;
    NIXViewWebProcessCrashedCallback                 webProcessCrashed;
    NIXViewCallback                                  webProcessRelaunched;
    NIXViewDoneWithTouchEventCallback                doneWithTouchEvent;
    NIXViewDoneWithGestureEventCallback              doneWithGestureEvent;
    NIXViewPageDidRequestScrollCallback              pageDidRequestScroll;
    NIXViewPageDidChangeContentsSizeCallback         didChangeContentsSize;
    NIXViewPageDidChangeViewportAttributesCallback   didChangeViewportAttributes;
    NIXViewPageDidFindZoomableAreaCallback           didFindZoomableArea;
    NIXViewPageUpdateTextInputStateCallback          updateTextInputState;
};
typedef struct NIXViewClient NIXViewClient;

enum { kNIXViewClientCurrentVersion = 0 };

WK_EXPORT NIXView NIXViewCreate(WKContextRef context, WKPageGroupRef pageGroup);
WK_EXPORT void NIXViewRelease(NIXView view);

WK_EXPORT void NIXViewSetViewClient(NIXView view, const NIXViewClient* viewClient);

WK_EXPORT void NIXViewInitialize(NIXView view);

WK_EXPORT WKSize NIXViewSize(NIXView view);
WK_EXPORT void NIXViewSetSize(NIXView view, WKSize size);

WK_EXPORT WKPoint NIXViewScrollPosition(NIXView view);
WK_EXPORT void NIXViewSetScrollPosition(NIXView view, WKPoint position);

WK_EXPORT void NIXViewSetUserViewportTransformation(NIXView view, const NIXMatrix* userViewportTransformation);
WK_EXPORT WKPoint NIXViewUserViewportToContents(NIXView view, WKPoint point);

WK_EXPORT bool NIXViewIsFocused(NIXView view);
WK_EXPORT void NIXViewSetFocused(NIXView view, bool focused);

WK_EXPORT bool NIXViewIsVisible(NIXView view);
WK_EXPORT void NIXViewSetVisible(NIXView view, bool visible);

WK_EXPORT bool NIXViewIsActive(NIXView view);
WK_EXPORT void NIXViewSetActive(NIXView view, bool active);

WK_EXPORT bool NIXViewTransparentBackground(NIXView view);
WK_EXPORT void NIXViewSetTransparentBackground(NIXView view, bool transparent);

WK_EXPORT bool NIXViewDrawBackground(NIXView view);
WK_EXPORT void NIXViewSetDrawBackground(NIXView view, bool drawBackground);

WK_EXPORT float NIXViewScale(NIXView view);
WK_EXPORT void NIXViewSetScale(NIXView view, float scale);

WK_EXPORT void NIXViewSetOpacity(NIXView view, float opacity);
WK_EXPORT float NIXViewOpacity(NIXView view);

WK_EXPORT WKSize NIXViewVisibleContentsSize(NIXView view);

WK_EXPORT void NIXViewPaintToCurrentGLContext(NIXView view);

WK_EXPORT void NIXViewFindZoomableAreaForPoint(NIXView view, WKPoint point, int horizontalRadius, int verticalRadius);

WK_EXPORT WKPageRef NIXViewGetPage(NIXView view);

WK_EXPORT void NIXViewSendMouseEvent(NIXView view, const NIXMouseEvent* event);
WK_EXPORT void NIXViewSendWheelEvent(NIXView view, const NIXWheelEvent* event);
WK_EXPORT void NIXViewSendKeyEvent(NIXView view, const NIXKeyEvent* event);
WK_EXPORT void NIXViewSendTouchEvent(NIXView view, const NIXTouchEvent* event);
WK_EXPORT void NIXViewSendGestureEvent(NIXView view, const NIXGestureEvent* event);

WK_EXPORT void NIXViewSuspendActiveDOMObjectsAndAnimations(NIXView view);
WK_EXPORT void NIXViewResumeActiveDOMObjectsAndAnimations(NIXView view);
WK_EXPORT bool NIXViewIsSuspended(NIXView view);

WK_EXPORT WKCoordinatedScene NIXViewGetCoordinatedScene(NIXView view);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // NIXView_h
