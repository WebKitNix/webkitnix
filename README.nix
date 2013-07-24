GLX vs. EGL
===========

GLX is used by default, to use OpenGLES2+EGL pass --opengles2 to build-webkit script.


Packages required
=================

WebKitNix requires some packages in the system as well as
automatically pull other dependencies using the jhbuild tool.

To compile WebKitNix all the three groups of packages below are necessary:

# These are needed for jhbuild tool to work.
# Debian / Ubuntu
$ sudo apt-get install gobject-introspection \
                       icon-naming-utils \
                       libgcrypt11-dev \
                       libgpg-error-dev \
                       libp11-kit-dev \
                       libtiff4-dev \
                       libcroco3-dev \
                       ruby

# Fedora
$ sudo yum install perl-version \
                   gobject-introspection-devel \
                   icon-naming-utils \
                   libgcrypt-devel \
                   libgpg-error-devel \
                   p11-kit-devel \
                   libtiff-devel \
                   libcroco-devel \
                   libjpeg-turbo-devel \
                   ruby


# These are needed for the dependencies that are automatically compiled.
# Debian / Ubuntu
$ sudo apt-get install gettext \
                       libpng12-dev \
                       g++\
                       gtk-doc-tools \
                       libgnutls-dev

# Fedora
$ sudo yum install gettext \
                   libpng-devel \
                   gcc-c++ \
                   gnutls-devel \
                   gtk-doc


# These are needed for WebKitNix itself.
# Debian / Ubuntu
$ sudo apt-get install cmake \
                       flex bison gperf \
                       libsqlite3-dev \
                       libxslt1-dev \
                       libicu-dev \
                       libgl1-mesa-dev \
                       libavcodec-dev \
                       libxrender-dev \
                       libxcomposite-dev

# Fedora
$ sudo yum install cmake \
                   flex \
                   bison \
                   gperf \
                   sqlite-devel \
                   libxslt-devel \
                   libicu-devel \
                   mesa-libGL-devel \
                   intltool \
                   ffmpeg-devel


NOTE: the packages libgl1-mesa-dev or mesa-libGL-devel can be
replaced by another that provides the OpenGL headers (GLES-based
systems, for example).

Other dependencies like glib, cairo et al. are pulled by jhbuild, by
using the script:

$ Tools/Scripts/update-webkitnix-libs

They are compiled and installed into WebKitBuild/Dependencies.


Debugging
=========

Nix uses the same approach as EFL does to provide debugging support.
It is available only in DEBUG mode.

If you need to debug a crash in a WebProcess while executing the
MiniBrowser you can set the WEB_PROCESS_CMD_PREFIX environment variable.
It takes some time to load all needed symbols - so be patient.

$ WEB_PROCESS_CMD_PREFIX="/usr/bin/xterm -title WebProcess -e gdb --args" \
                                        WebKitBuild/Debug/bin/MiniBrowser

The /usr/bin/xterm is necessary or gdb will run in the current terminal,
which can get particularly confusing since it's running in the background,
and if you're also running the main process in gdb, won't work at all
(the two instances will fight over the terminal). It's necessary to pass the
full path to the xterm binary otherwise it will fail.

If you want to debug a specific test (TEST_OF_YOUR_CHOICE in the example
below) you can run WebKitTestRunner with WEB_PROCESS_CMD_PREFIX exactly as
you did with MiniBrowser:

WEB_PROCESS_CMD_PREFIX="/usr/bin/xterm -title WebProcess -e gdb --args" \
WebKitBuild/Debug/bin/WebKitTestRunner --no-timeout-at-all \
                                         LayoutTests/TEST_OF_YOUR_CHOICE

If you want to have detailed logs from LOG(...) functions you can enable
logging for each log channel with WEBKIT_DEBUG environment variable:
WEBKIT_DEBUG="WTFLogChannel1 WTFLogChannel2 ... WTFLogChannelN"

You can find log channels in Source/WebCore/platform/Logging.cpp and
Source/WebKit2/Platform/Logging.cpp. For example: Frames, Events, Network, ...


Using the remote inspector
==========================

In order to get the InspectorServer running
you need to set WEBKIT_INSPECTOR_SERVER before
running MiniBrowser, as in:

"WEBKIT_INSPECTOR_SERVER="127.0.0.1:25555" ./nix_build/Release/bin/MiniBrowser"

After that just open a browser, load http://127.0.0.1:25555/ and you will
get a list of webpages available for being inspected. You can also load
directly http://127.0.0.1:25555/webinspector/inspector.html?page=ID if you know
the page id you want to inspect.
