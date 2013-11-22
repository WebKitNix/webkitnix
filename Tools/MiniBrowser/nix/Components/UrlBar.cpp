/*
 * Copyright (C) 2012-2013 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 University of Szeged
 * Copyright (C) 2013 Roland Takacs <rtakacs@inf.u-szeged.hu>
 * Copyright (C) 2013 Matyas Mustoha <mmatyas@inf.u-szeged.hu>
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

#include "UrlBar.h"

#include "../BrowserControl.h"
#include <fstream>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

UrlBar::UrlBar(Display* display, Window parent, XContext context, BrowserControl* control, WKRect size, std::string url)
    : VisualComponent(display, control, size)
    , m_isFocused(false)
    , m_isMousePressed(false)
    , m_url(url)
    , m_copiedText(url)
    , m_cursorPosition(0)
    , m_selectedCharacterCount(0)
    , m_textOffset(0)
    , m_loadProgress(0.0)
{
    createXWindow(parent, context);

    m_surface = cairo_xlib_surface_create(display, m_window, DefaultVisual(display, 0), m_size.size.width, m_size.size.height);
    m_cairo = cairo_create(m_surface);
    cairo_select_font_face(m_cairo, "Verdana", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 14);

    XDefineCursor(display, m_window, XCreateFontCursor(display, XC_xterm));

    m_clipboardAtom = XInternAtom(m_display, "CLIPBOARD", True);
}

UrlBar::~UrlBar()
{
    cairo_surface_destroy(m_surface);
    cairo_destroy(m_cairo);
}

void UrlBar::createXWindow(Window parent, XContext context)
{
    m_window = XCreateSimpleWindow(m_display, parent, m_size.origin.x, m_size.origin.y, m_size.size.width, m_size.size.height, 1, createXColor("#C1C1C1"), WhitePixel(m_display, 0));

    XSelectInput(m_display, m_window, ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XSaveContext(m_display, m_window, context, (XPointer)this);
    XMapWindow(m_display, m_window);
}

void UrlBar::handleEvent(const XEvent& event)
{
    switch (event.type) {
    case Expose:
        drawUrlBar();
        break;
    case ButtonPress:
        addFocus();
        handleMultiClick(event.xbutton);
        break;
    case ButtonRelease:
        m_isMousePressed = false;
        drawUrlBar();
        break;
    case SelectionRequest:
        respondClipboardRequest(event.xselectionrequest);
        break;
    case SelectionNotify:
        pasteClipboardText(event);
        drawUrlBar();
        break;
    case MotionNotify:
        calculateSelectedCharacters(event.xmotion.x);
        drawUrlBar();
        break;
    }
}

void UrlBar::initSelection(int clickX)
{
    m_cursorPosition = updateCursorPosition(clickX);

    m_selectedCharacterCount = 0;
    m_isMousePressed = true;
}

bool UrlBar::isSeparator(char c)
{
    // Based on RFC 2616
    switch (c) {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case '&':
    case '#':
    case '%':
    case ',':
    case ';':
    case ':':
    case '"':
    case '\'':
    case '/':
    case '\\':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
    case '.':
        return true;
    default:
        return false;
    }
}

void UrlBar::selectWord()
{
    int rightCharPos = m_cursorPosition;
    while (rightCharPos < m_url.length() && !isSeparator(m_url[rightCharPos])) {
        rightCharPos++;
        m_selectedCharacterCount++;
    }
    while (m_cursorPosition > 0 && !isSeparator(m_url[m_cursorPosition - 1])) {
        m_cursorPosition--;
        m_selectedCharacterCount++;
    }
}

void UrlBar::handleMultiClick(const XButtonPressedEvent& pressEvent)
{
    m_control->updateClickCount(pressEvent);
    switch (m_control->clickCount()) {
    case 1:
        initSelection(pressEvent.x);
        break;
    case 2:
        selectWord();
        break;
    default:
        m_cursorPosition = 0;
        m_selectedCharacterCount = m_url.length();
    }
}

void UrlBar::calculateSelectedCharacters(int mouseXPos)
{
    if (!m_isMousePressed)
        return;

    m_selectedCharacterCount = updateCursorPosition(mouseXPos) - m_cursorPosition;
}

void UrlBar::setText(std::string url)
{
    m_url = url;

    drawUrlBar();
}

void UrlBar::setLoadProgress(double progress)
{
    m_loadProgress = progress;

    drawUrlBar();
}

bool UrlBar::focused()
{
    return m_isFocused;
}

void UrlBar::addFocus()
{
    if (!m_isFocused) {
        m_isFocused = true;
        m_control->passFocusToUrlBar();
    }
}

void UrlBar::releaseFocus()
{
    if (m_isFocused) {
        m_isFocused = false;
        m_selectedCharacterCount = 0;
        drawUrlBar();
    }
}

void UrlBar::drawUrlBar()
{
    XClearWindow(m_display, m_window);
    drawHighLight();
    drawBackground();
    drawText();
    drawCursor();
}

void UrlBar::drawHighLight()
{
    if (!m_selectedCharacterCount)
        return;

    int start = m_textOffset + textWidth(m_url.substr(0, m_cursorPosition).c_str());
    int width = m_textOffset + textWidth(m_url.substr(0, m_cursorPosition + m_selectedCharacterCount).c_str()) - start;

    cairo_set_source_rgba(m_cairo, 1.0, 0.5, 0.0, 0.5);
    cairo_rectangle(m_cairo, start, 2, width, m_size.size.height - 4);
    cairo_fill(m_cairo);
}

void UrlBar::drawBackground()
{
    if (m_loadProgress == 0.0 || m_loadProgress == 1.0)
        return;
    cairo_set_source_rgb(m_cairo, 0.8, 0.8, 0.8);
    cairo_rectangle(m_cairo, 0, 0, m_size.size.width * m_loadProgress, m_size.size.height);
    cairo_fill(m_cairo);
}

void UrlBar::drawText()
{
    updateTextOffset();

    cairo_set_source_rgb(m_cairo, 0.2, 0.2, 0.2);
    cairo_move_to(m_cairo, m_textOffset, 16);
    cairo_show_text(m_cairo, m_url.c_str());
}

void UrlBar::drawCursor()
{
    if (!m_isFocused || m_selectedCharacterCount)
        return;

    int cursorXCoord = textWidth(m_url.substr(0, m_cursorPosition).c_str()) + m_textOffset;

    cairo_set_source_rgb(m_cairo, 0.2, 0.2, 0.2);
    cairo_move_to(m_cairo, cursorXCoord, 4);
    cairo_line_to(m_cairo, cursorXCoord, 18);
    cairo_stroke(m_cairo);
}

void UrlBar::updateTextOffset()
{
    int urlPixelWidth = textWidth(m_url.c_str());

    // If the url text fits into the urlbar.
    if (urlPixelWidth < m_size.size.width) {
        m_textOffset = 0;
        return;
    }

    std::string subUrl = m_url.substr(0, m_cursorPosition);
    int cursorPosition = textWidth(subUrl.c_str()) + m_textOffset;
    int visibleTextWidth = urlPixelWidth + m_textOffset;
    int remainingWidth = m_size.size.width - visibleTextWidth;
    // On resize, if the width of the visible url text is less than the text area width.
    if (remainingWidth > 0) {
        m_textOffset += remainingWidth;
        return;
    }

    // On cursor movement, if the cursor leave the urlbar.
    if (cursorPosition < 0)
        m_textOffset -= cursorPosition;
    else if (cursorPosition > m_size.size.width)
        m_textOffset -= cursorPosition - m_size.size.width;
}

void UrlBar::handleKeyPressEvent(const XKeyPressedEvent& event)
{
    char normalKey;
    KeySym specialKey;

    XLookupString(const_cast<XKeyEvent*>(&event), &normalKey, sizeof(char), &specialKey, 0);

    switch (specialKey) {
    case XK_BackSpace:
        removeCharacter();
        break;
    case XK_Delete:
        deleteCharacter();
        break;
    case XK_Home:
        moveCursorHome(event.state);
        break;
    case XK_End:
        moveCursorEnd(event.state);
        break;
    case XK_Left:
        moveCursorLeft(event.state);
        break;
    case XK_Right:
        moveCursorRight(event.state);
        break;
    case XK_Return:
        loadPage();
        break;
    case XK_Insert:
        if (event.state & ControlMask)
            becomeClipboardOwner();
        else if (event.state & ShiftMask)
            requestClipboardText();
        break;
    default:
        if (event.state & ControlMask) {
            // while the Control key is pressed, character codes start from 1 (A=1, B=2, ...).
            switch (normalKey + 'A' - 1) {
            case 'A':
                m_cursorPosition = 0;
                m_selectedCharacterCount = m_url.length();
                break;
            case 'C':
                becomeClipboardOwner();
                break;
            case 'V':
                requestClipboardText();
                break;
            case 'X':
                becomeClipboardOwner();
                removeSelectedText();
                break;
            }
        } else
            appendCharacter(normalKey);

        break;
    }

    drawUrlBar();
}

void UrlBar::moveCursorHome(unsigned state)
{
    // If shift is pressed at the same time, selection needs to go home.
    if (state & ShiftMask)
        m_selectedCharacterCount = -m_cursorPosition;
    else {
        m_cursorPosition = 0;
        m_selectedCharacterCount = 0;
    }
}

void UrlBar::moveCursorEnd(unsigned state)
{
    // If shift is pressed at the same time, selection needs to go end.
    if (state & ShiftMask)
        m_selectedCharacterCount = m_url.length() - m_cursorPosition;
    else {
        m_cursorPosition = m_url.length();
        m_selectedCharacterCount = 0;
    }
}

void UrlBar::moveCursorLeft(unsigned state)
{
    // If shift is pressed at the same time, selection needs to be moved.
    if (state & ShiftMask) {
        if (m_cursorPosition + m_selectedCharacterCount > 0)
            m_selectedCharacterCount--;
        return;
    }

    if (m_cursorPosition)
        m_cursorPosition--;

    m_selectedCharacterCount = 0;
}

void UrlBar::moveCursorRight(unsigned state)
{
    // If shift is pressed at the same time, selection needs to be moved.
    if (state & ShiftMask) {
        if (m_cursorPosition + m_selectedCharacterCount < m_url.length())
            m_selectedCharacterCount++;
        return;
    }

    if (m_cursorPosition < m_url.length())
        m_cursorPosition++;

    m_selectedCharacterCount = 0;
}

void UrlBar::deleteCharacter()
{
    if (m_url.empty())
        return;

    if (m_selectedCharacterCount) {
        removeSelectedText();
        return;
    }

    // If the cursor is after the last character.
    if (m_cursorPosition >= m_url.length())
        return;

    m_url.erase(m_cursorPosition, 1);
}

void UrlBar::removeCharacter()
{
    if (m_url.empty())
        return;

    if (m_selectedCharacterCount) {
        removeSelectedText();
        return;
    }

    // If the cursor is before the first character.
    if (!m_cursorPosition)
        return;

    m_cursorPosition--;
    m_url.erase(m_cursorPosition, 1);
}

void UrlBar::removeSelectedText()
{
    if (m_selectedCharacterCount < 0)
        m_cursorPosition += m_selectedCharacterCount;

    m_url.erase(m_cursorPosition, abs(m_selectedCharacterCount));
    m_selectedCharacterCount = 0;
}

void UrlBar::appendCharacter(const char c)
{
    if (c < 32 || c >= 127)
        return;

    if (m_selectedCharacterCount)
        removeSelectedText();

    m_url.insert(m_cursorPosition, 1, c);
    m_cursorPosition++;
}

void UrlBar::loadPage()
{
    if (m_url.find("http") != 0 && m_url.find("file://") != 0) {
        std::ifstream localFile(m_url.c_str());
        m_url.insert(0, localFile ? "file://" : "http://");
    }

    m_control->loadPage(m_url.c_str());
}

int UrlBar::textWidth(const char* text)
{
    cairo_text_extents_t extents;
    cairo_text_extents(m_cairo, text, &extents);

    return extents.x_advance;
}

int UrlBar::calculateCharacterPosition(int clickX, int min, int max)
{
    std::string subUrl = m_url.substr(0, max);
    std::string pointedCharacter = m_url.substr(min, 1);

    int subUrlPixelWidth = textWidth(subUrl.c_str());
    int pointedCharacterPixelWidth = textWidth(pointedCharacter.c_str());

    // Defines the click position on the pointed character and determines
    // that the cursor should be on the left or the right side.
    int clickPositionOnCharacter = subUrlPixelWidth - clickX;
    if (clickPositionOnCharacter > (pointedCharacterPixelWidth / 2))
        return min;

    return max;
}

// Binary search to determine the cursor position.
int UrlBar::pointedCharacterIndex(int clickX, int min, int max)
{
    if (max - min == 1)
        return calculateCharacterPosition(clickX, min, max);

    int midpoint = (max + min) / 2;
    std::string subUrl = m_url.substr(0, midpoint);
    int subUrlPixelWidth = textWidth(subUrl.c_str());

    if (subUrlPixelWidth > clickX)
        return pointedCharacterIndex(clickX, min, midpoint);

    return pointedCharacterIndex(clickX, midpoint, max);
}

int UrlBar::updateCursorPosition(int clickX)
{
    if (m_url.empty())
        return 0;

    if (clickX < 0)
        clickX = 0;

    bool clickedAfterTheUrl = (m_textOffset < 0) && (clickX > textWidth(m_url.c_str()));
    if (clickedAfterTheUrl)
        return m_url.length();

    return pointedCharacterIndex(clickX - m_textOffset, 0, m_url.length());
}

void UrlBar::requestClipboardText()
{
    XConvertSelection(m_display, m_clipboardAtom, XA_STRING, XA_STRING, m_window, CurrentTime);
}

void UrlBar::pasteClipboardText(const XEvent &event)
{
    Atom clipType;
    int clipFormat;
    unsigned long clipLength;
    unsigned long clipRemainingBytes;
    unsigned char *clipBuffer = 0;

    if (event.xselection.property == None)
        return;

    XGetWindowProperty(m_display, m_window,
        event.xselection.property, 0, 255, False, AnyPropertyType,
        &clipType, &clipFormat, &clipLength, &clipRemainingBytes, &clipBuffer);

    for (unsigned long i = 0; i < clipLength; i++)
        appendCharacter(clipBuffer[i]);

    XFree(clipBuffer);
}

void UrlBar::becomeClipboardOwner()
{
    if (!m_selectedCharacterCount)
        return;

    int startPos = m_cursorPosition;
    if (m_selectedCharacterCount < 0)
        startPos = m_cursorPosition + m_selectedCharacterCount;

    m_copiedText = m_url.substr(startPos, abs(m_selectedCharacterCount));

    XSetSelectionOwner(m_display, m_clipboardAtom, m_window, CurrentTime);
    if (XGetSelectionOwner(m_display, m_clipboardAtom) != m_window)
        fprintf(stderr, "Could not set clipboard ownership.\n");

    XSetSelectionOwner(m_display, XA_PRIMARY, m_window, CurrentTime);
    if (XGetSelectionOwner(m_display, XA_PRIMARY) != m_window)
        fprintf(stderr, "Could not set primary selection ownership.\n");
}

void UrlBar::respondClipboardRequest(const XSelectionRequestEvent& requestEvent)
{
    XEvent responseEvent;
    responseEvent.xselection.type = SelectionNotify;
    responseEvent.xselection.property = None;
    responseEvent.xselection.display = requestEvent.display;
    responseEvent.xselection.requestor = requestEvent.requestor;
    responseEvent.xselection.selection = requestEvent.selection;
    responseEvent.xselection.target = requestEvent.target;
    responseEvent.xselection.time = requestEvent.time;

    XChangeProperty(m_display, requestEvent.requestor, requestEvent.property, XA_STRING,
        8, PropModeReplace, reinterpret_cast<const unsigned char *>(m_copiedText.c_str()), m_copiedText.length());

    responseEvent.xselection.property = requestEvent.property;
    XSendEvent(m_display, requestEvent.requestor, False, 0, &responseEvent);
    XFlush(m_display);
}
