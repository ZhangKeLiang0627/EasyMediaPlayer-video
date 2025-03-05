#
# Makefile
#
CC = /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc
CXX = /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-g++

BIN = easyMediaPlayer

LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= .
BUILD_DIR = ./build
VPATH ?= $(LVGL_DIR)

CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wno-unused-function -Wno-unused-variable
LDFLAGS ?= -lm
CXXFLAGS ?= $(CFLAGS) 

CSRCS ?= 
CXXSRCS ?= 

CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include
CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner
CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner/include 
CFLAGS += -I$(LVGL_DIR)/include


CFLAGS += -pipe -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard -fstack-protector  

LDFLAGS += -L/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/lib
LDFLAGS += -L/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/lib  
LDFLAGS += -ltplayer -lcdx_base -lncurses -lpthread -lstdc++

# Collect the files to compile
CXXSRCS += ./src/mediaPlayer.cpp
MAINSRC += ./src/main.cpp

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
# include $(LVGL_DIR)/src/source.mk

OBJEXT ?= .o
AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
CXXOBJS = $(CXXSRCS:.cpp=$(OBJEXT))
MAINOBJ = $(MAINSRC:.cpp=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(CXXSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS) $(CXXOBJS) $(MAINOBJ)

vpath %.c $(VPATH)
vpath %.cpp $(VPATH)

## MAINOBJ -> OBJFILES

all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

%.o: %.cpp
	@$(CXX)  $(CXXFLAGS) -c $< -o $@
	@echo "CXX $<"
    
default: $(OBJS)
	$(CXX) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(CXXOBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ) $(CXXOBJS)
	rm -r $(BUILD_DIR)
