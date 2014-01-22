/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
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
#include "TestRunner.h"
#include "InjectedBundle.h"
#include <glib.h>

namespace WTR {

static gboolean waitToDumpWatchdogTimerCallback(gpointer)
{
    InjectedBundle::shared().testRunner()->waitToDumpWatchdogTimerFired();
    return 0;
}

void TestRunner::platformInitialize()
{
    m_waitToDumpWatchdogTimer = 0;
}

void TestRunner::invalidateWaitToDumpWatchdogTimer()
{
    if (!m_waitToDumpWatchdogTimer)
        return;

    g_source_remove(m_waitToDumpWatchdogTimer);
    m_waitToDumpWatchdogTimer = 0;
}

void TestRunner::initializeWaitToDumpWatchdogTimerIfNeeded()
{
    if (m_waitToDumpWatchdogTimer)
        return;

    m_waitToDumpWatchdogTimer = g_timeout_add(waitToDumpWatchdogTimerInterval * 1000,
                                              waitToDumpWatchdogTimerCallback, 0);
}

JSRetainPtr<JSStringRef> TestRunner::pathToLocalResource(JSStringRef url)
{
    return url;
}

} // namespace WTR
