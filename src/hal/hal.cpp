#include "hal.h"

/* Signal handler */
void terminate(int sig_no);
/* set Signal Callback */
void install_sig_handler(void);
/* LVGL tick get */
uint32_t custom_tick_get(void);

void HAL::Init(void)
{
    // LittlevGL init
    lv_init();
    uint32_t rotated = LV_DISP_ROT_NONE;

    // Linux frame buffer device init
    sunxifb_init(rotated);

    // A buffer for LittlevGL to draw the screen's content
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    static lv_color_t *buf;
    buf = (lv_color_t *)sunxifb_alloc(width * height * sizeof(lv_color_t), (char *)"lv_examples");

    if (buf == NULL)
    {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return;
    }

    // Initialize a descriptor for the buffer
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, width * height);

    // Initialize and register a display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = sunxifb_flush;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    disp_drv.rotated = rotated;
#ifndef USE_SUNXIFB_G2D_ROTATE
    if (rotated != LV_DISP_ROT_NONE)
        disp_drv.sw_rotate = 1;
#endif
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv;
    // Basic initialization
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    // Register the driver in LVGL and save the created input device object
    lv_indev_t *evdev_indev = lv_indev_drv_register(&indev_drv);

    // 注册退出回调函数
    install_sig_handler();
}

void terminate(int sig_no)
{
    printf("[Sys] Got signal %d, exiting ...\n", sig_no);
    
    sunxifb_free((void **)&lv_disp_get_default()->driver->draw_buf->buf1, (char *)"lv_examples");
    sunxifb_exit();
    lv_deinit();

    exit(0);
}

void install_sig_handler(void)
{
    signal(SIGBUS, terminate);
    signal(SIGFPE, terminate);
    signal(SIGHUP, terminate);
    signal(SIGILL, terminate);
    signal(SIGINT, terminate);
    signal(SIGIOT, terminate);
    signal(SIGPIPE, terminate);
    signal(SIGQUIT, terminate);
    signal(SIGSEGV, terminate);
    signal(SIGSYS, terminate);
    signal(SIGTERM, terminate);
    signal(SIGTRAP, terminate);
    signal(SIGUSR1, terminate);
    signal(SIGUSR2, terminate);
}

/* Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR` */
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if (start_ms == 0)
    {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = ((uint64_t)tv_start.tv_sec * 1000000 + (uint64_t)tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = ((uint64_t)tv_now.tv_sec * 1000000 + (uint64_t)tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
