# Collect the files to compile
CSRCS += ./src/MediaPlayer.c
CXXSRCS += ./src/MyPlayer.cpp

CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include
CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner
CFLAGS += -I/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner/include 
CFLAGS += -I$(LVGL_DIR)/src

CFLAGS += -pipe -march=armv7-a -mtune=cortex-a7 -mfpu=neon -mfloat-abi=hard -fstack-protector  

LDFLAGS += -L/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/lib
LDFLAGS += -L/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/lib  
LDFLAGS += -ltplayer -lcdx_base -lncurses -lpthread -lstdc++

