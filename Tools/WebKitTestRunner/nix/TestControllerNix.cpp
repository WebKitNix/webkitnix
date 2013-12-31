/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Igalia S.L.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "TestController.h"

#include <cstdio>
#include <cstdlib>
#include <glib.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <wtf/Platform.h>
#include <wtf/text/WTFString.h>

namespace WTR {

static guint gTimeoutSourceId = 0;
static GMainLoop* mainLoop = 0;
static gchar* nixTemporaryDirectory = 0;

static void cancelTimeout()
{
    if (!gTimeoutSourceId)
        return;
    g_source_remove(gTimeoutSourceId);
    gTimeoutSourceId = 0;
}

void TestController::notifyDone()
{
    if (mainLoop)
        g_main_loop_quit(mainLoop);
    cancelTimeout();
}

void TestController::platformInitialize()
{
}

void TestController::platformDestroy()
{
    if (nixTemporaryDirectory) {
        g_free(nixTemporaryDirectory);
        nixTemporaryDirectory = 0;
    }
}

static gboolean timeoutCallback(gpointer)
{
    fprintf(stderr, "FAIL: TestControllerRunLoop timed out.\n");
    g_main_loop_quit(mainLoop);
    return 0;
}

void TestController::platformRunUntil(bool& condition, double timeout)
{
    ASSERT(!mainLoop);
    cancelTimeout();
    if (timeout != m_noTimeout)
        gTimeoutSourceId = g_timeout_add(timeout * 1000, timeoutCallback, 0);
    mainLoop = g_main_loop_new(0, false);
    g_main_loop_run(mainLoop);
    g_main_loop_unref(mainLoop);
    mainLoop = 0;
}

static const char* getEnvironmentVariableOrExit(const char* variableName)
{
    const char* value = getenv(variableName);
    if (!value) {
        fprintf(stderr, "%s environment variable not found\n", variableName);
        exit(1);
    }

    return value;
}

static void getWebKitAppPath(char* buffer, size_t bufferSize)
{
    ssize_t len = readlink("/proc/self/exe", buffer, bufferSize);
    if (len != -1) {
        buffer[len] = 0;
        char* dirPath = strdup(buffer);
        dirPath = dirname(dirPath);
        strncpy(buffer, dirPath, bufferSize);
        free(dirPath);
    } else {
        fprintf(stderr, "Can't read /proc/self/exe!\n");
        exit(1);
    }
}

void TestController::initializeInjectedBundlePath()
{
    const char* bundlePath = getenv("TEST_RUNNER_INJECTED_BUNDLE_FILENAME");
    if (!bundlePath) {
        // Try to guess the injected bundle path.
        static char path[PATH_MAX];
        getWebKitAppPath(path, sizeof(path));
        strcat(path, "/../lib/libTestRunnerInjectedBundle.so");
        FILE* f = fopen(path, "r");
        if (!f) {
            fprintf(stderr, "Cannot find the injected bundle at %s\n", path);
            exit(1);
        }
        fclose(f);
        bundlePath = path;
    }
    m_injectedBundlePath.adopt(WKStringCreateWithUTF8CString(bundlePath));
}

void TestController::initializeTestPluginDirectory()
{
    const char* pluginPath = getenv("TEST_RUNNER_PLUGIN_PATH");
    if (!pluginPath) {
        // Try to guess the plugin path.
        static char path[PATH_MAX];
        getWebKitAppPath(path, sizeof(path));
        strcat(path, "/../lib/libTestRunnerInjectedBundle.so");
        pluginPath = path;
    }
    m_testPluginDirectory.adopt(WKStringCreateWithUTF8CString(pluginPath));
}

void TestController::platformInitializeContext()
{
}

void TestController::setHidden(bool)
{
    // FIXME: Need to implement this to test visibilityState.
}

void TestController::runModal(PlatformWebView*)
{
    // FIXME: Need to implement this to test showModalDialog.
}

const char* TestController::platformLibraryPathForTesting()
{
    ASSERT(!nixTemporaryDirectory);

    nixTemporaryDirectory = g_dir_make_tmp("WebKitNixTests-XXXXXX", 0);
    return nixTemporaryDirectory;
}

} // namespace WTR
