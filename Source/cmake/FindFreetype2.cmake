# Copyright (C) 2013 Nokia Corporation and/or its subsidiary(-ies).
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

find_package(PkgConfig)
pkg_check_modules(PC_FREETYPE REQUIRED freetype2)

find_path(FREETYPE_HEADER
    NAMES freetype.h
    HINTS ${PC_FREETYPE_INCLUDE_DIRS}
          ${PC_FREETYPE_INCLUDEDIR}
    PATH_SUFFIXES freetype
)

find_path(FREETYPE_ROOT_INCLUDE_DIR
    NAMES freetype/freetype.h
    HINTS ${PC_FREETYPE_INCLUDE_DIRS}
          ${PC_FREETYPE_INCLUDEDIR}
)

find_library(FREETYPE2_LIBRARIES
    NAMES freetype
    HINTS ${PC_FREETYPE_LIBDIR}
          ${PC_FREETYPE_LIBRARY_DIRS}
)

set(FREETYPE2_INCLUDE_DIRS
    ${FREETYPE_HEADER}
    ${FREETYPE_ROOT_INCLUDE_DIR}
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FreeType2 DEFAULT_MSG FREETYPE2_INCLUDE_DIRS FREETYPE2_LIBRARIES)
