cmake_minimum_required(VERSION 3.10)
project(demo C)

# 设置编译器
set(CMAKE_C_COMPILER /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc)
set(CMAKE_CXX_COMPILER /home/hugokkl/tina-sdk/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-g++)

# 设置编译选项
add_compile_options(
    -O3 -g0
    -Wall -Wshadow -Wundef -Wmissing-prototypes
    -Wno-discarded-qualifiers -Wall -Wextra
    -Wno-unused-function -Wno-error=strict-prototypes
    -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp
    -Wuninitialized -Wmaybe-uninitialized
    -Wno-unused-parameter -Wno-missing-field-initializers
    -Wtype-limits -Wsizeof-pointer-memaccess
    -Wno-format-nonliteral -Wno-cast-qual
    -Wunreachable-code -Wno-switch-default
    -Wreturn-type -Wmultichar -Wformat-security
    -Wno-ignored-qualifiers -Wno-error=pedantic
    -Wno-sign-compare -Wno-error=missing-prototypes
    -Wdouble-promotion -Wclobbered -Wdeprecated
    -Wempty-body -Wtype-limits -Wshift-negative-value
    -Wstack-usage=2048 -Wno-unused-value
)

# 设置项目根目录
set(LVGL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 包含头文件目录
include_directories(${LVGL_DIR})
include_directories(${LVGL_DIR}/lvgl)
include_directories(${LVGL_DIR}/lvgl/demos)

# 获取 demos 源文件
file(GLOB_RECURSE LVGL_DEMOS_SOURCES 
    ${LVGL_DIR}/lvgl/demos/*.c
    ${LVGL_DIR}/lvgl/demos/*/*.c
)

# 包含 LVGL 相关的源文件
include(${LVGL_DIR}/lvgl/lvgl.cmake)
include(${LVGL_DIR}/lv_drivers/lv_drivers.cmake)

# 添加调试信息
message(STATUS "LVGL sources: ${LVGL_SOURCES}")
message(STATUS "LV_DRIVERS sources: ${LV_DRIVERS_SOURCES}")
message(STATUS "LVGL_DEMOS sources: ${LVGL_DEMOS_SOURCES}")

# 添加主程序源文件
set(MAIN_SRC ./main.c)

# 创建可执行文件
add_executable(demo ${MAIN_SRC} ${LVGL_SOURCES} ${LV_DRIVERS_SOURCES} ${LVGL_DEMOS_SOURCES})

# 链接数学库
target_link_libraries(demo m)