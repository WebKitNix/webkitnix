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

#include "Options.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <list>

Options::Options()
    : width(0)
    , height(0)
    , viewportHorizontalDisplacement(0)
    , viewportVerticalDisplacement(0)
    , desktopModeEnabled(false)
    , forceTouchEmulationEnabled(false)
    , devicePixelRatio(1.0)
    , dumpFrames(false)
    , helpRequested(false)
{
}

struct Device {
    enum Type {
        Default,
        N9,
        IPad,
        IPhone,
        Android
    };

    int width;
    int height;
    const char* userAgent;
};

class Option {
public:
    Option(const char* name, const char* helpMessage, bool* variable)
        : m_name(name)
        , m_valueName("")
        , m_helpMessage(helpMessage)
    {
        m_assigner = [=](const char*)
        {
            *variable = true;
            return 0;
        };
    }

    template<typename T>
    Option(const char* name, const char* valueName, const char* helpMessage, const char* format, T* variable)
        : m_name(name)
        , m_valueName(valueName)
        , m_helpMessage(helpMessage)
    {
        m_assigner = [=](const char* arg)
        {
            if (!arg || sscanf(arg, format, variable) != 1)
                return -1;
            return 1;
        };
    }

    template<typename T1, typename T2>
    Option(const char* name, const char* valueName, const char* helpMessage, const char* format, T1* variable1, T2* variable2)
        : m_name(name)
        , m_valueName(valueName)
        , m_helpMessage(helpMessage)
    {
        m_assigner = [=](const char* arg)
        {
            if (!arg || sscanf(arg, format, variable1, variable2) != 2)
                return -1;
            return 1;
        };
    }

    template<typename T>
    Option(const char* name, const char* helpMessage, T* variable, T value)
        : m_name(name)
        , m_valueName("")
        , m_helpMessage(helpMessage)
    {
        m_assigner = [=](const char*)
        {
            *variable = value;
            return 0;
        };
    }

    bool match(const char* arg) const
    {
        return !strcmp(m_name, arg);
    }

    bool process(std::list<const char*>& args)
    {
        int assignResult = m_assigner(args.empty() ? 0 : args.front());
        if (assignResult < 0)
            return false;
        if (assignResult > 0)
            args.pop_front();

        return true;
    }

    const char* m_name;
    const char* m_valueName;
    const char* m_helpMessage;
    // Should return -1 on error, 0 sucess, 1 success and should pop an argument.
    std::function<int(const char*)> m_assigner;
};

static void showHelp(const std::list<Option>& options)
{
    printf("Use MiniBrowser [options] [url]\n\n");
    for (const Option& option : options) {
        std::string arg(option.m_name);
        arg += ' ';
        arg += option.m_valueName;
        printf("  %-28s %s\n", arg.c_str(), option.m_helpMessage);
    }
}

bool Options::parse(int argc, const char* argv[])
{
    Device deviceList[] = {
        { 1024, 768, "" },
        { 854, 480, "Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, like Gecko) NokiaBrowser/8.5.0 Mobile Safari/534.13" },
        { 1024, 768, "Mozilla/5.0 (iPad; CPU OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3" },
        { 960, 640, "Mozilla/5.0 (iPhone; CPU iPhone OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3" },
        { 800, 480, "Mozilla/5.0 (Linux; U; Android 4.0.2; en-us; Galaxy Nexus Build/ICL53F) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30" }
    };

    Device::Type device = Device::Default;
    char injectedBundle[256];
    injectedBundle[0] = 0;

    std::list<Option> options = {
        Option("--desktop", "Enable desktop mode.", &desktopModeEnabled),
        Option("--touch-emulation", "Force touch emulation.", &forceTouchEmulationEnabled),
        Option("-t", "Alias for --touch-emulation.", &forceTouchEmulationEnabled),
        Option("--window-size", "WxH", "Set the window size.", "%dx%d", &width, &height),
        Option("--viewport-displacement", "HxV", "Set the horizontal and vertical viewport displacement.", "%dx%d", &viewportHorizontalDisplacement, &viewportVerticalDisplacement),
        Option("--injected-bundle", "path", "Use a custom injected bundle.", "%255s", &injectedBundle),
        Option("--dpr", "value", "Set the device pixel ratio.", "%f", &devicePixelRatio),
        Option("--dump-frames", "Dump all rendered frames to PNG files.", &dumpFrames),
        Option("--n9", "Use n9 user agent.", &device, Device::N9),
        Option("--ipad", "Use iPad user agent.", &device, Device::IPad),
        Option("--iphone", "Use iPhone user agent.", &device, Device::IPhone),
        Option("--android", "Use iPhone user agent.", &device, Device::Android)
    };

    std::list<const char*> args(&argv[1], &argv[argc]);

    while (!args.empty()) {
        const char* arg = args.front();
        args.pop_front();

        if (!strcmp("--help", arg)) {
            helpRequested = true;
            showHelp(options);
            return true;
        }

        bool argMatches = false;
        for (Option option : options) {
            argMatches = option.match(arg);
            if (argMatches) {
                if (option.process(args))
                    break;
                showHelp(options);
                return false;
            }
        }
        if (!argMatches)
            url = arg;
    }

    if (width == 0)
        width = deviceList[device].width;

    if (height == 0)
        height = deviceList[device].height;

    userAgent = deviceList[device].userAgent;

    if (url.empty())
        url = "http://www.google.com";
    else if (url.find("http") != 0 && url.find("file://") != 0) {
        std::ifstream localFile(url.c_str());
        url.insert(0, localFile ? "file://" : "http://");
    }

    if (injectedBundle[0])
        Options::injectedBundle = injectedBundle;

    return true;
}
