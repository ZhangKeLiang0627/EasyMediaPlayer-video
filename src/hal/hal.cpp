#include "hal.h"

/* File system funtion */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

/* Signal handler */
void signalExitCallback(int signal);
/* set Signal Callback */
void install_signal_handler(void);
/* LVGL tick get */
uint32_t custom_tick_get(void);

/**
 * @brief 硬件抽象层初始化
 *
 */
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

    // file system initialization
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    fs_drv.letter = 'S';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    lv_fs_drv_register(&fs_drv);

    // 注册退出回调函数
    install_signal_handler();
}

/**
 * @brief LVGL处理函数
 *
 */
void HAL::LVGL_Proc(void)
{
    for (;;)
    {
        pthread_mutex_lock(&lv_mutex);
        uint32_t ms = lv_task_handler();
        pthread_mutex_unlock(&lv_mutex);
        usleep(ms * 1000);
    }
}

/**
 * @brief 系统退出回调函数
 *
 * @param signal
 */
void signalExitCallback(int signal)
{
    printf("[Sys] Got signal %d, exiting ...\n", signal);

    sunxifb_free((void **)&lv_disp_get_default()->driver->draw_buf->buf1, (char *)"lv_examples");
    sunxifb_exit();
    lv_deinit();

    exit(0);
}

/**
 * @brief 系统退出信号绑定
 *
 */
void install_signal_handler(void)
{
    signal(SIGBUS, signalExitCallback);
    signal(SIGFPE, signalExitCallback);
    signal(SIGHUP, signalExitCallback);
    signal(SIGILL, signalExitCallback);
    signal(SIGINT, signalExitCallback);
    signal(SIGIOT, signalExitCallback);
    signal(SIGPIPE, signalExitCallback);
    signal(SIGQUIT, signalExitCallback);
    signal(SIGSEGV, signalExitCallback);
    signal(SIGSYS, signalExitCallback);
    signal(SIGTERM, signalExitCallback);
    signal(SIGTRAP, signalExitCallback);
    signal(SIGUSR1, signalExitCallback);
    signal(SIGUSR2, signalExitCallback);
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

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return pointer to FIL struct or NULL in case of fail
 */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    char smode[4] = {0};
    FILE *fp = NULL;

    if (mode == LV_FS_MODE_WR)
        strcpy(smode, "wb");
    else if (mode == LV_FS_MODE_RD)
        strcpy(smode, "rb");
    else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
        strcpy(smode, "wb+");
    else
        return NULL;

    fp = fopen(path, smode);

    return fp;
}

/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    FILE *fp = (FILE *)file_p;

    if (0 == fclose(fp))
        res = LV_FS_RES_OK;
    else
        res = LV_FS_RES_FS_ERR;

    return res;
}

/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    FILE *fp = (FILE *)file_p;

    *br = fread(buf, (size_t)1, (size_t)btr, fp);
    if (*br == btr)
        res = LV_FS_RES_OK;
    else
        res = LV_FS_RES_FS_ERR;

    return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable. (opened with fs_open )
 * @param pos the new position of read write pointer
 * @param whence only LV_SEEK_SET is supported
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    FILE *fp = (FILE *)file_p;

    if (0 == fseek(fp, pos, whence))
        res = LV_FS_RES_OK;
    else
        res = LV_FS_RES_FS_ERR;

    return res;
}

/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a FIL variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    (void)drv;
    FILE *fp = (FILE *)file_p;

    *pos_p = ftell(fp);

    return LV_FS_RES_OK;
}