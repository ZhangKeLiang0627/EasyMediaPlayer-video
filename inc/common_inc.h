#pragma once

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include <cstring>

#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <atomic>

#include "../libs/lvgl/lvgl.h"
#include "../libs/lv_drivers/display/sunxifb.h"
#include "../libs/lv_drivers/indev/evdev.h"
#include "../utils/log/log.h"
#include "MediaPlayer.h"
#include "HAL.h"