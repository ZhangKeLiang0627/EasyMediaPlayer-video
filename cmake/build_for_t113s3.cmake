#
# cross compile env define
#

SET(CMAKE_SYSTEM_NAME Linux)
# 配置库的安装路径
SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

SET(CMAKE_SYSTEM_PROCESSOR "arm")
SET(CMAKE_HOST_SYSTEM_PROCESSOR "arm")

# 工具链地址
SET(TOOLCHAIN_DIR  "/home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/")

# 设置头文件所在目录
include_directories(
    /home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include
    /home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner
    /home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/include/allwinner/include
    /home/hugokkl/tina-sdk/out/t113-pi/compile_dir/target/freetype-2.13.2/include
)

set(CMAKE_PREFIX_PATH /usr)

# 设置第三方库所在位置
link_directories(
    /home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/lib
    /home/hugokkl/tina-sdk/out/t113-pi/staging_dir/target/usr/lib 
)

add_compile_options(
    -pipe 
    -march=armv7-a 
    -mtune=cortex-a7 
    -mfpu=neon 
    -mfloat-abi=hard 
    -fstack-protector
)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# allwinner t113s3
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}arm-openwrt-linux-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}arm-openwrt-linux-g++)
