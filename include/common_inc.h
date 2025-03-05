#pragma once

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string> 

#include "../libs/lvgl/lvgl.h"
#include "../libs/lv_drivers/display/sunxifb.h"
#include "../libs/lv_drivers/indev/evdev.h"
#include "MediaPlayer.h"
#include "HAL.h"

extern pthread_mutex_t lv_mutex;
