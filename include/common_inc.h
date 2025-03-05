#pragma once

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string> 

#include "../lvgl/lvgl.h"
#include "../lv_drivers/display/sunxifb.h"
#include "../lv_drivers/indev/evdev.h"
#include "MediaPlayer.h"
#include "hal.h"

