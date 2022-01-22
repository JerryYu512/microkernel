# version
RELEASE_VERSION = 1.0.0
PLAT_NAME = $(shell uname -m)
OS_NAME = $(shell uname)
ENABLE_STATIC_LIB = y

ifneq ($(VERSION),)
	RELEASE_VERSION = $(VERSION)
endif

MAKE_INSTALL_DIR = install

# 调试模式使能
DEBUG = n

# 使用的加密套件
# WITH_OPENSSL
# WITH_MBEDTLS
# MBEDTLS_SSL_CACHE_C

# make path
export MAKE := "/usr/bin/make"

# make flag
MAKEFLAG := -s --no-print-directory

# compile marcro
export CC := $(TOOL_PREFIX)gcc
export CXX := $(TOOL_PREFIX)g++
AR := $(TOOL_PREFIX)ar rcs
SHAREDFLG := -shared
# 编译时间
COMPILE_TIME := $(shell date +"%A %Y-%m-%d %H:%M:%S %z")
BUILD_VERSION := $(shell date +"%Y%m%d%H%M")

# compile flags
INC := -I/usr/include -I/usr/local/include -I$(ROOT_DIR)/include -I$(ROOT_DIR)/$(TARGET) -I$(ROOT_DIR) -Ibuild

LIBS_PATH := -L$(ROOT_DIR)/lib -L/usr/local/lib -L/usr/lib

ST_LIBS_UT = #-lgtest

SO_LIBS = -lz -lssl -lcrypto -lpthread -ldl -lrt # -lresolv

DMARCROS := -DLANGUAGE_ZH -DWITH_OPENSSL -DWITH_ZLIB -DSOFT_VERSION=\"$(RELEASE_VERSION)\" \
			-DBUILD_VERSION="\"$(BUILD_VERSION)"\"

# 调试模式
ifeq ($(DEBUG), n)
DMARCROS += -DNDEBUG
CCFLAG = -O3
MODE = release
else
DMARCROS += -DDEBUG
CCFLAG = -g
MODE = debug
endif

DMARCROS += -D__const__= -pipe -W -Wall -Wno-unused-parameter \
			-fPIC -fno-omit-frame-pointer -Wno-implicit-fallthrough \
			-fstack-protector-all -Wno-deprecated-declarations \
			-Wno-class-memaccess \
			-Wno-unused-result -Wno-maybe-uninitialized

# -ggdb
CCFLAG += -c $(INC) -Wall -std=c++11 $(DMARCROS)
OBJCCFLAG := $(CCFLAG)
