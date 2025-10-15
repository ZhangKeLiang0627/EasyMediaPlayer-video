cmake_minimum_required(VERSION 3.10)
project(eMP_video)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# 编译选项
# Default to C99
set(CMAKE_C_STANDARD 99)
# Default to C++14
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 系统信息打印
message(STATUS "System : ${CMAKE_SYSTEM_NAME}")
message(STATUS "Target system processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Host architecture: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# 基础编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g0 -Wall -Wno-unused-function -Wno-unused-variable -Wno-return-type -Wno-sign-compare -Wno-memset-transposed-args")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")

set(COMMON_LIBRARIES
    pthread
    stdc++fs
    stdc++
    # cdx_base
    # ncurses
    m
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# 检查本地/交叉编译 
find_package(PkgConfig REQUIRED)
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    message(STATUS "Local-compilation 💻")
    
    set(LV_DRIVERS_USE_WAYLAND ON CACHE BOOL "Enable Wayland for local build")

    # 添加本地编译宏定义
    add_definitions(-DLV_USE_DRAW_SDL=0 -DUSE_SDL=1 -DUSE_MOUSE=1)
    
    # 使用pkg-config查找SDL2
    find_package(SDL2 REQUIRED)
    list(APPEND COMMON_LIBRARIES ${SDL2_LIBRARIES})

    # 添加Freetype头文件路径
    include_directories(/usr/include/freetype2)

    add_definitions(-DPROJECT_PATH="${PROJECT_SOURCE_DIR}")
    add_definitions(-DRESOURCES_PATH="${PROJECT_SOURCE_DIR}")
else()
    # 交叉编译时可以添加特定设置
    message(STATUS "Cross-compilation 🧩")
    
    add_definitions(
        -DLV_USE_DRAW_SDL=0  
        -DUSE_SDL=0         
        # -DLV_CONF_INCLUDE_SIMPLE
        # -DUSE_SUNXIFB_DOUBLE_BUFFER
        -DUSE_SUNXIFB_CACHE
        -DUSE_SUNXIFB_G2D
        -DUSE_SUNXIFB_G2D_ROTATE
        -DCONF_G2D_VERSION_NEW
    )

    list(APPEND COMMON_LIBRARIES tplayer)


    add_definitions(-DPROJECT_PATH="${PROJECT_SOURCE_DIR}")
    add_definitions(-DRESOURCES_PATH="${PROJECT_SOURCE_DIR}")

endif()

# 查找源文件
file(GLOB_RECURSE C_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.c"
)
file(GLOB_RECURSE CXX_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp"
)
set(MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

set(ALL_SRCS
    ${C_SRCS}
    ${CXX_SRCS}
    ${MAIN_SRC}
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${ALL_SRCS})

# 添加子CMakeLists.txt
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/lvgl)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/lv_drivers)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog)

# 链接库
target_link_libraries(${PROJECT_NAME}
    lvgl
    lvgl::drivers
    freetype
    spdlog
    ${COMMON_LIBRARIES}
)
