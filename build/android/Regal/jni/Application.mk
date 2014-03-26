#seth: need to specify this manually to get the latest
NDK_TOOLCHAIN_VERSION := 4.8

APP_MODULES := Regal_static Regal
APP_STL := stlport_static
APP_PLATFORM := android-9

ifndef APP_OPTIM
ifeq ($(NDK_DEBUG),1)
  $(warning NDK_DEBUG set, enabling debug build: APP_OPTIM := debug)
  APP_OPTIM := debug
endif
endif
