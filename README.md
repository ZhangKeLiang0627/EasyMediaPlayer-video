# LVGL media player for Allwinner T113S3

# eMP_video

## 环境

```shell
# 请自行修改 build.sh 文件的 STAGING_DIR 环境变量
export STAGING_DIR=/home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target
```

```makefile
# 请自行修改 Makefile 文件的 编译器变量 CC 和 CXX
CC = /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc
CXX = /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-g++
```

## 编译

```shell
./build.sh
```

## 运行

可执行文件为：`eMP_video`