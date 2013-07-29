/*
 * Copyright (C) 2009 Julien Chaffraix <jchaffraix@pleyo.com>
 * Copyright (C) 2011, 2012 Research In Motion Limited. All rights reserved.
 * Copyright (C) 2013 University of Szeged
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CookieDatabaseBackingStore_h
#define CookieDatabaseBackingStore_h

#include "ParsedCookie.h"
#include "SQLiteDatabase.h"
#include "Timer.h"
#include <wtf/HashMap.h>
#include <wtf/Vector.h>
#include <wtf/text/StringHash.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class CookieNode;

class CookieDatabaseBackingStore {
public:

    CookieDatabaseBackingStore();
    ~CookieDatabaseBackingStore();
    void init(CookieNode*);

    void insert(const PassRefPtr<ParsedCookie>);
    void update(const PassRefPtr<ParsedCookie>);
    void remove(const PassRefPtr<ParsedCookie>);
    void removeAll();

private:

    enum Action {
        Insert,
        Update,
        Delete
    };

    void open(const String& cookieJar);
    void close();
    void getCookiesFromDatabase(CookieNode*);
    void removeLastAccessedCookies();
    void addToChangeQueue(const PassRefPtr<ParsedCookie> changedCookie, Action actionParam);
    void sendChangesToDatabase();
    void dbTimerCallback(Timer<CookieDatabaseBackingStore>*);

    typedef pair<const RefPtr<ParsedCookie>, Action> CookieAction;
    Vector<CookieAction> m_changedCookies;

    SQLiteDatabase m_db;
    SQLiteStatement *m_insertStatement;
    SQLiteStatement *m_updateStatement;
    SQLiteStatement *m_deleteStatement;

    Timer<CookieDatabaseBackingStore> m_dbTimer;
    Mutex m_mutex;
};

} // namespace WebCore

#endif // CookieDatabaseBackingStore_h
