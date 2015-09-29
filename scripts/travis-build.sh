#!/bin/bash

set -x
set -e

if [[ $SYSTEM =~ nacl* ]]; then
  export NACL_SDK_ROOT=$PWD/nacl_sdk/pepper_canary
  export PATH=$PATH:$NACL_SDK_ROOT/toolchain/linux_x86_newlib/bin
  export PATH=$PATH:$NACL_SDK_ROOT/toolchain/linux_arm_newlib/bin
fi

make -j4

make test
