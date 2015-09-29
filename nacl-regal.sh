#!/bin/bash
# Copyright (c) 2012 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Any arguments passed to this script will be passed onto make.  It is
# recommend to pass -j<N> to speed up the build.

# Notes:
#   - Pepper 25 is needed for the ARM build.
#   - Makefile needs to be specified for branches that have GNUmakefiles
#   - MODE=debug for debug-mode build
#   - TOOLCHAIN=glibc or clang-newlib to use and alternative to the default
#     newlib toolchain.
#   - CCACHE=ccache is supported (and recommended)

set -e

echo "Building x86_64"
make -f Makefile SYSTEM=nacl-x86_64 $*

echo "Building i686"
make -f Makefile SYSTEM=nacl-i686 $*

echo "Building ARM"
make -f Makefile SYSTEM=nacl-arm $*

echo "Building pnacl"
make -f Makefile SYSTEM=nacl-pnacl $*
