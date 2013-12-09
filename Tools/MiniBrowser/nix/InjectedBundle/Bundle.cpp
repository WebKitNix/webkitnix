/*
 * Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
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

#include <WebKit2/WKBundle.h>
#include <WebKit2/WKBundleInitialize.h>
#include <WebKit2/WKBundlePage.h>
#include <WebKit2/WKString.h>
#include <cstring>
#include <cstdio>

static WKBundleRef globalBundle;

// WKBundlePageClient

static void willAddMessageToConsole(WKBundlePageRef, WKStringRef message, uint32_t lineNumber, const void*)
{
    size_t messageStringSize =  WKStringGetMaximumUTF8CStringSize(message);
    char* messageString = new char[messageStringSize];
    WKStringGetUTF8CString(message, messageString, messageStringSize);

    printf("[js:%u] %s\n", lineNumber, messageString);

    delete[] messageString;
}

// WKBundleClient

static void didCreatePage(WKBundleRef, WKBundlePageRef page, const void*)
{
    WKBundlePageUIClientV2 uiClient;
    memset(&uiClient, 0, sizeof(uiClient));

    uiClient.base.version = 2;
    uiClient.willAddMessageToConsole = willAddMessageToConsole;

    WKBundlePageSetUIClient(page, &uiClient.base);
}

extern "C" {
void WKBundleInitialize(WKBundleRef bundle, WKTypeRef)
{
    globalBundle = bundle;

    WKBundleClientV1 client;
    memset(&client, 0, sizeof(client));

    client.base.version = 1;
    client.didCreatePage = didCreatePage;

    WKBundleSetClient(bundle, &client.base);
}
}
