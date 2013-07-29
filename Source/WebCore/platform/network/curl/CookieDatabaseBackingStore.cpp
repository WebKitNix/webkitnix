/*
 * Copyright (C) 2009 Julien Chaffraix <jchaffraix@pleyo.com>
 * Copyright (C) 2010, 2011, 2012 Research In Motion Limited. All rights reserved.
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

#include "config.h"
#include "CookieDatabaseBackingStore.h"

#include "CookieNode.h"
#include "Logging.h"
#include "ParsedCookie.h"
#include "SQLiteStatement.h"
#include "SQLiteTransaction.h"
#include <wtf/text/StringBuilder.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

CookieDatabaseBackingStore::CookieDatabaseBackingStore()
    : m_insertStatement(0)
    , m_updateStatement(0)
    , m_deleteStatement(0)
    , m_dbTimer(this, &CookieDatabaseBackingStore::dbTimerCallback)
{
    open("cookies.db");
}

void CookieDatabaseBackingStore::init(CookieNode* tree)
{
    getCookiesFromDatabase(tree);
    m_dbTimer.startRepeating(60);
}

CookieDatabaseBackingStore::~CookieDatabaseBackingStore()
{
    close();
}

void CookieDatabaseBackingStore::open(const String& cookieJar)
{
    if (m_db.isOpen())
        close();

    if (!m_db.open(cookieJar)) {
        LOG_ERROR("Could not open the cookie database. No cookie will be stored!");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }

    if (!m_db.executeCommand("CREATE TABLE IF NOT EXISTS cookies (name TEXT, value TEXT, host TEXT, path TEXT, expiry DOUBLE, lastAccessed DOUBLE, isSecure INTEGER, isHttpOnly INTEGER, creationTime DOUBLE, protocol TEXT, PRIMARY KEY (protocol, host, path, name));")) {
        LOG_ERROR("Could not create the table to store the cookies into. No cookie will be stored!");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        close();
        return;
    }

    m_insertStatement = new SQLiteStatement(m_db, "INSERT OR REPLACE INTO cookies (name, value, host, path, expiry, lastAccessed, isSecure, isHttpOnly, creationTime, protocol) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10);");
    if (m_insertStatement->prepare()) {
        LOG_ERROR("Cannot save cookies");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
    }

    m_updateStatement = new SQLiteStatement(m_db, "UPDATE cookies SET name = ?1, value = ?2, host = ?3, path = ?4, expiry = ?5, lastAccessed = ?6, isSecure = ?7, isHttpOnly = ?8, creationTime = ?9, protocol = ?10 where name = ?1 and host = ?3 and path = ?4;");
    if (m_updateStatement->prepare()) {
        LOG_ERROR("Cannot update cookies");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
    }

    m_deleteStatement = new SQLiteStatement(m_db, "DELETE FROM cookies WHERE name=?1 and host=?2 and path=?3 and protocol=?4;");
    if (m_deleteStatement->prepare()) {
        LOG_ERROR("Cannot delete cookies");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
    }
}

void CookieDatabaseBackingStore::dbTimerCallback(Timer<CookieDatabaseBackingStore>*)
{
    sendChangesToDatabase();
}

void CookieDatabaseBackingStore::sendChangesToDatabase()
{
    MutexLocker lock(m_mutex);
    size_t sizeOfChange = m_changedCookies.size();

    if (!sizeOfChange)
        return;

    SQLiteTransaction transaction(m_db, false);
    transaction.begin();

    for (size_t i = 0; i < sizeOfChange; i++) {
        SQLiteStatement* m_statement;
        const RefPtr<ParsedCookie> cookie = m_changedCookies[i].first;
        Action action = m_changedCookies[i].second;

        if (action == Delete) {
            m_statement = m_deleteStatement;

            // Binds all the values
            if (m_statement->bindText(1, cookie->name()) || m_statement->bindText(2, cookie->domain())
                || m_statement->bindText(3, cookie->path()) || m_statement->bindText(4, cookie->protocol())) {
                LOG_ERROR("Cannot bind cookie data to delete");
                LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
                continue;
            }
        } else {
            if (action == Update)
                m_statement = m_updateStatement;
            else
                m_statement = m_insertStatement;

            // Binds all the values
            if (m_statement->bindText(1, cookie->name()) || m_statement->bindText(2, cookie->value())
                || m_statement->bindText(3, cookie->domain()) || m_statement->bindText(4, cookie->path())
                || m_statement->bindDouble(5, cookie->expiry()) || m_statement->bindDouble(6, cookie->lastAccessed())
                || m_statement->bindInt64(7, cookie->isSecure()) || m_statement->bindInt64(8, cookie->isHttpOnly())
                || m_statement->bindDouble(9, cookie->creationTime()) || m_statement->bindText(10, cookie->protocol())) {
                LOG_ERROR("Cannot bind cookie data to save");
                LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
                continue;
            }
        }

        int rc = m_statement->step();
        m_statement->reset();
        if (rc != SQLResultOk && rc != SQLResultDone) {
            LOG_ERROR("Cannot make call to the database");
            LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
            continue;
        }
    }

    transaction.commit();
    m_changedCookies.clear();
}

void CookieDatabaseBackingStore::close()
{
    m_dbTimer.stop();
    sendChangesToDatabase();

    delete m_insertStatement;
    delete m_updateStatement;
    delete m_deleteStatement;

    if (m_db.isOpen())
        m_db.close();
}

void CookieDatabaseBackingStore::getCookiesFromDatabase(CookieNode* tree)
{
    if (!m_db.isOpen())
        return;

    SQLiteStatement selectStatement(m_db, "SELECT name, value, host, path, expiry, lastAccessed, isSecure, isHttpOnly, creationTime, protocol FROM cookies ORDER BY lastAccessed ASC;");
    if (selectStatement.prepare()) {
        LOG_ERROR("Cannot retrieved cookies from the database");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }

    unsigned counter = 0;
    while (selectStatement.step() == SQLResultRow) {
        String name = selectStatement.getColumnText(0);
        String value = selectStatement.getColumnText(1);
        String domain = selectStatement.getColumnText(2);
        String path = selectStatement.getColumnText(3);
        double expiry = selectStatement.getColumnDouble(4);
        double lastAccessed = selectStatement.getColumnDouble(5);
        bool isSecure = selectStatement.getColumnInt(6);
        bool isHttpOnly = selectStatement.getColumnInt(7);
        double creationTime = selectStatement.getColumnDouble(8);
        String protocol = selectStatement.getColumnText(9);

        RefPtr<ParsedCookie> cookie = ParsedCookie::create(name, value, domain, protocol, path, expiry, lastAccessed, creationTime, isSecure, isHttpOnly);
        if (cookie->hasExpired()) {
            remove(cookie);
            continue;
        }

        Vector<String> splittedHost;
        cookie->domain().split(".", false, splittedHost);
        tree->insert(cookie, splittedHost, this, WithHttpOnlyCookies, OnlyInMemory);
        counter++;
    }

    if (counter >= s_globalMaxCookieCount)
        removeLastAccessedCookies();
}

void CookieDatabaseBackingStore::insert(const PassRefPtr<ParsedCookie> cookie)
{
    if (!cookie->hasExpired())
        addToChangeQueue(cookie, Insert);
}

void CookieDatabaseBackingStore::update(const PassRefPtr<ParsedCookie> cookie)
{
    if (!cookie->hasExpired())
        addToChangeQueue(cookie, Update);
    else
        remove(cookie);
}

void CookieDatabaseBackingStore::remove(const PassRefPtr<ParsedCookie> cookie)
{
    addToChangeQueue(cookie, Delete);
}

void CookieDatabaseBackingStore::removeAll()
{
    if (!m_db.isOpen())
        return;

    {
        MutexLocker lock(m_mutex);
        m_changedCookies.clear();
    }

    SQLiteStatement deleteStatement(m_db, "DELETE FROM cookies;");
    if (deleteStatement.prepare()) {
        LOG_ERROR("Could not prepare DELETE * statement");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }

    if (!deleteStatement.executeCommand()) {
        LOG_ERROR("Cannot delete cookie from database");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }
}

void CookieDatabaseBackingStore::removeLastAccessedCookies()
{
    if (!m_db.isOpen())
        return;

    SQLiteStatement deleteStatement(m_db, "DELETE FROM cookies WHERE (name, value, host, path) lastAccessed IN (SELECT name, value, host, path FROM cookies ORDER BY lastAccessed ASC LIMIT ?1);");
    deleteStatement.bindInt64(1, s_cookiesToDeleteWhenLimitReached);

    if (deleteStatement.prepare()) {
        LOG_ERROR("Could not prepare DELETE * statement");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }

    if (!deleteStatement.executeCommand()) {
        LOG_ERROR("Cannot delete cookie from database");
        LOG_ERROR("SQLite Error Message: %s", m_db.lastErrorMsg());
        return;
    }
}

void CookieDatabaseBackingStore::addToChangeQueue(const PassRefPtr<ParsedCookie> changedCookie, Action actionParam)
{
    CookieAction change(changedCookie, actionParam);
    MutexLocker lock(m_mutex);
    m_changedCookies.append(change);
}

} // namespace WebCore
