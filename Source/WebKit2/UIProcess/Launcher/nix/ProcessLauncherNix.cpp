/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Portions Copyright (c) 2010 Motorola Mobility, Inc.  All rights reserved.
 * Copyright (C) 2013 Peter Gal <galpeter@inf.u-szeged.hu>, University of Szeged
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
 * THIS SOFTWARE IS PROVIDED BY MOTOROLA INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MOTOROLA INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ProcessLauncher.h"

#include "Connection.h"
#include "ProcessExecutablePath.h"
#include <WebCore/AuthenticationChallenge.h>
#include <WebCore/FileSystem.h>
#include <WebCore/NetworkingContext.h>
#include <WebCore/ResourceHandle.h>
#include <WebCore/RunLoop.h>
#include <errno.h>
#include <glib.h>
#include <locale.h>
#include <wtf/OwnArrayPtr.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>
#include <wtf/gobject/GOwnPtr.h>
#include <wtf/gobject/GlibUtilities.h>

#if OS(LINUX)
#include <sys/prctl.h>
#include <sys/socket.h>
#endif

#ifdef SOCK_SEQPACKET
#define SOCKET_TYPE SOCK_SEQPACKET
#else
#define SOCKET_TYPE SOCK_STREAM
#endif

using namespace WebCore;

namespace WebKit {

static void childSetupFunction(gpointer userData)
{
    int socket = GPOINTER_TO_INT(userData);
    close(socket);
}

static Vector<OwnArrayPtr<char>> createArgsArray(const String& prefix, const String& executablePath, const String& socket, const String& pluginPath)
{
    ASSERT(!executablePath.isEmpty());
    ASSERT(!socket.isEmpty());

    Vector<String> splitArgs;
    prefix.split(' ', splitArgs);

    splitArgs.append(executablePath);
    splitArgs.append(socket);
    if (!pluginPath.isEmpty())
        splitArgs.append(pluginPath);

    Vector<OwnArrayPtr<char>> args(splitArgs.size() + 1); // Extra room for null.

    size_t numArgs = splitArgs.size();
    for (size_t i = 0; i < numArgs; ++i) {
        CString param = splitArgs[i].utf8();
        args[i] = adoptArrayPtr(new char[param.length() + 1]); // Room for the terminating null coming from the CString.
        strncpy(args[i].get(), param.data(), param.length() + 1); // +1 here so that strncpy copies the ending null.
    }

    // execvp() needs the pointers' array to be null-terminated.
    args[numArgs] = nullptr;

    return args;
}

void ProcessLauncher::launchProcess()
{
    GPid pid = 0;

    int sockets[2];
    if (socketpair(AF_UNIX, SOCKET_TYPE, 0, sockets) < 0) {
        g_printerr("Creation of socket failed: %s.\n", g_strerror(errno));
        ASSERT_NOT_REACHED();
        return;
    }

    String processCmdPrefix, executablePath, pluginPath;
    if (m_launchOptions.processType == WebProcess)
        executablePath = executablePathOfWebProcess();
    else {
        executablePath = executablePathOfPluginProcess();
        pluginPath = m_launchOptions.extraInitializationData.get("plugin-path");
    }

#ifndef NDEBUG
    if (!m_launchOptions.processCmdPrefix.isEmpty())
        processCmdPrefix = m_launchOptions.processCmdPrefix;
#endif

    Vector<OwnArrayPtr<char>> args = createArgsArray(processCmdPrefix, executablePath, String::number(sockets[0]), pluginPath);

    GOwnPtr<GError> error;
    int spawnFlags = G_SPAWN_LEAVE_DESCRIPTORS_OPEN | G_SPAWN_SEARCH_PATH;
    if (!g_spawn_async(0, reinterpret_cast<char**>(args.data()), 0, static_cast<GSpawnFlags>(spawnFlags), childSetupFunction, GINT_TO_POINTER(sockets[1]), &pid, &error.outPtr())) {
        g_printerr("Unable to fork a new WebProcess: %s.\n", error->message);
        ASSERT_NOT_REACHED();
    }

    close(sockets[0]);
    m_processIdentifier = pid;

    // We've finished launching the process, message back to the main run loop.
    RunLoop::main()->dispatch(bind(&ProcessLauncher::didFinishLaunchingProcess, this, m_processIdentifier, sockets[1]));
}

void ProcessLauncher::terminateProcess()
{
    if (m_isLaunching) {
        invalidate();
        return;
    }

    if (!m_processIdentifier)
        return;

    kill(m_processIdentifier, SIGKILL);
    m_processIdentifier = 0;
}

void ProcessLauncher::platformInvalidate()
{
}

} // namespace WebKit
