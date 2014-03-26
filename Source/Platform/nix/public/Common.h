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

#ifndef Nix_Common_h
#define Nix_Common_h

// Exported symbols need to be annotated with NIX_EXPORT
#define NIX_EXPORT __attribute__((visibility("default")))

#include <stddef.h> // For size_t

#define DEFINE_NIX_API_ASSIGNMENT(type) \
        type operator=(const type&); \
        type operator=(type&); \

#define DEFINE_NIX_API_ASSIGNMENT_IMPL(type) \
    type type::operator=(const type& other) \
    { \
        m_private = other.m_private; \
        return *this; \
    } \
    type type::operator=(type& other) \
    { \
        m_private = other.m_private; \
        return *this; \
    } \

#if BUILDING_NIX__
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>

#define DEFINE_BASE_NIX_API_INTERNAL(type, webcoreType) \
        type(const WTF::PassRefPtr<WebCore::webcoreType>& value) \
            : m_private(value) \
        { \
        } \
        type(WebCore::webcoreType* value) \
            : m_private(value) \
        { \
        } \
        type& operator=(WebCore::webcoreType* value) \
        { \
            m_private = value; \
            return *this; \
        } \
        type& operator=(const WTF::PassRefPtr<WebCore::webcoreType>& value) \
        { \
            m_private = value; \
            return *this; \
        } \
        operator WTF::PassRefPtr<WebCore::webcoreType>() const \
        { \
            return toWebCoreType(); \
        } \
        operator WebCore::webcoreType*() const \
        { \
            return toWebCoreType(); \
        } \
    protected: \
        WebCore::webcoreType* toWebCoreType() const \
        { \
            return m_private.get(); \
        } \
        WTF::RefPtr<WebCore::webcoreType> m_private; \

#endif

#endif // Nix_Common_h
