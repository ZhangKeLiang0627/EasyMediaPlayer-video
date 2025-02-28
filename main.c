#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"
#include "./src/MediaPlayer.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <tplayer.h>

TPlayer *mTPlayer;
MediaInfo *mMediaInfo;

bool mPreparedFlag;
bool mLoopFlag;
bool mSetLoop;
bool mComplete;
bool mSeekable;
bool isPlaying = false;

uint8_t bQuit = 0;

// #define LCD_WIDTH 720.0
#define LCD_WIDTH 480.0

lv_obj_t *Btn_Create(lv_obj_t *par, const void *img_src, lv_coord_t y_ofs);
static void button_event_handler(lv_event_t *e);

/* Signal handler */
static void terminate(int sig_no)
{
    printf("Got signal %d, exiting ...\n", sig_no);

    tplayer_exit();
    sunxifb_free((void **)&lv_disp_get_default()->driver->draw_buf->buf1, "lv_examples");
    sunxifb_exit();
    lv_deinit();

    if (mTPlayer != NULL)
    {
        TPlayerDestroy(mTPlayer);
        mTPlayer = NULL;
        printf("TPlayerDestroy() successfully\n");
    }

    printf("destroy tplayer!\n");
    printf("tplaydemo exit!\n");

    exit(0);
}

static void install_sig_handler(void)
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

//* a callback for tplayer.
int CallbackForTPlayer(void *pUserData, int msg, int param0, void *param1)
{
    CEDARX_UNUSE(param1);
    switch (msg)
    {
    case TPLAYER_NOTIFY_PREPARED:
    {
        printf("TPLAYER_NOTIFY_PREPARED,has prepared.\n");
        mPreparedFlag = 1;
        break;
    }

    case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
    {
        printf("TPLAYER_NOTIFY_PLAYBACK_COMPLETE\n");
        mComplete = 1;
        // PowerManagerReleaseWakeLock("tplayerdemo");
        break;
    }

    case TPLAYER_NOTIFY_SEEK_COMPLETE:
    {
        printf("TPLAYER_NOTIFY_SEEK_COMPLETE>>>>info: seek ok.\n");
        break;
    }

    case TPLAYER_NOTIFY_MEDIA_ERROR:
    {
        switch (param0)
        {
        case TPLAYER_MEDIA_ERROR_UNKNOWN:
        {
            printf("erro type:TPLAYER_MEDIA_ERROR_UNKNOWN\n");
            break;
        }
        case TPLAYER_MEDIA_ERROR_UNSUPPORTED:
        {
            printf("erro type:TPLAYER_MEDIA_ERROR_UNSUPPORTED\n");
            break;
        }
        case TPLAYER_MEDIA_ERROR_IO:
        {
            printf("erro type:TPLAYER_MEDIA_ERROR_IO\n");
            break;
        }
        }
        printf("TPLAYER_NOTIFY_MEDIA_ERROR\n");
        printf("error: open media source fail.\n");
        break;
    }

    case TPLAYER_NOTIFY_NOT_SEEKABLE:
    {
        mSeekable = 0;
        printf("info: media source is unseekable.\n");
        break;
    }

    case TPLAYER_NOTIFY_BUFFER_START:
    {
        printf("have no enough data to play\n");
        break;
    }

    case TPLAYER_NOTIFY_BUFFER_END:
    {
        printf("have enough data to play again\n");
        break;
    }

    case TPLAYER_NOTIFY_VIDEO_FRAME:
    {
        // printf("get the decoded video frame\n");
        break;
    }

    case TPLAYER_NOTIFY_AUDIO_FRAME:
    {
        // printf("get the decoded audio frame\n");
        break;
    }

    case TPLAYER_NOTIFY_SUBTITLE_FRAME:
    {
        // printf("get the decoded subtitle frame\n");
        break;
    }
    case TPLAYER_NOTYFY_DECODED_VIDEO_SIZE:
    {
        int w, h, y;
        w = ((int *)param1)[0]; // real decoded video width
        h = ((int *)param1)[1]; // real decoded video height
        printf("*****tplayerdemo:video decoded width = %d,height = %d\n", w, h);
        float divider = 1;
        if (w > LCD_WIDTH)
        {
            divider = w / LCD_WIDTH;
        }
        w = w / divider;
        h = h / divider;
        y = (LCD_WIDTH - h) / 2;
        printf("real set to display rect:w = %d,h = %d\n", w, h);
        TPlayerSetDisplayRect(mTPlayer, 0, y, w, h);
        break;
    }

    default:
    {
        // printf("warning: unknown callback from Tinaplayer. %d\n", msg);
        break;
    }
    }
    return 0;
}

int main(int argc, char *argv[])
{

    printf("[Sys] lv_myPlayer begin!\n");

    if (argc == 1)
    {
        printf("[Sys] Please input file name!\n");
        return -1;
    }

    // clear fb0
    system("dd if=/dev/zero of=/dev/fb0");

    system("amixer sset Headphone unmute");
    system("amixer sset \"Headphone volume\" 2");

    /*LittlevGL init*/
    lv_init();

    uint32_t rotated = LV_DISP_ROT_NONE;

    /*Linux frame buffer device init*/
    sunxifb_init(rotated);

    /*A buffer for LittlevGL to draw the screen's content*/
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    static lv_color_t *buf;
    buf = (lv_color_t *)sunxifb_alloc(width * height * sizeof(lv_color_t), "lv_examples");

    if (buf == NULL)
    {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return 0;
    }

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, width * height);

    /*Initialize and register a display driver*/
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
    lv_indev_drv_init(&indev_drv);          /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER; /*See below.*/
    indev_drv.read_cb = evdev_read;         /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t *evdev_indev = lv_indev_drv_register(&indev_drv);

    // tplayer初始化
    install_sig_handler();

    mTPlayer = TPlayerCreate(CEDARX_PLAYER);
    TPlayerSetDebugFlag(mTPlayer, false);
    if (mTPlayer == NULL)
    {
        printf("can not create tplayer, quit.\n");
        TPlayerDestroy(mTPlayer);
        return -1;
    }
    TPlayerSetNotifyCallback(mTPlayer, CallbackForTPlayer, (void *)NULL);
    TPlayerSetLooping(mTPlayer, 1);
    TPlayerSetDataSource(mTPlayer, argv[1], NULL);
    TPlayerPrepare(mTPlayer);
    // TPlayerStart(mTPlayer);

    // 创建画布
    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xcccccc), 0);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *btnCont = lv_obj_create(cont);
    lv_obj_remove_style_all(btnCont);
    lv_obj_set_size(btnCont, lv_pct(70), LV_VER_RES / 4);
    lv_obj_clear_flag(btnCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(btnCont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btnCont, lv_color_hex(0x6a8d6d), 0);
    lv_obj_align(btnCont, LV_ALIGN_BOTTOM_MID, 0, 42);

    lv_obj_set_style_radius(btnCont, 16, LV_PART_MAIN);

    lv_obj_t *btn = Btn_Create(btnCont, LV_SYMBOL_PLAY, -20);

    lv_obj_add_event_cb(btn, button_event_handler, LV_EVENT_ALL, NULL);

    /*Handle LitlevGL tasks (tickless mode)*/
    while (1)
    {
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
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

lv_obj_t *Btn_Create(lv_obj_t *par, const void *img_src, lv_coord_t y_ofs)
{
    lv_obj_t *obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 180, 50);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_CENTER, 0, y_ofs);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, 75, LV_STATE_PRESSED); // 设置button按下时的长宽
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x356b8c), 0);                 // 设置按钮默认的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x242947), LV_STATE_PRESSED);  // 设置按钮在被按下时的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf2daaa), LV_STATE_FOCUSED);  // 设置按钮在被聚焦时的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xa99991), LV_STATE_DISABLED); // 设置按钮在被聚焦时的颜色
    lv_obj_set_style_radius(obj, 9, 0);                                        // 按钮画圆角

    static lv_style_transition_dsc_t tran;
    static const lv_style_prop_t prop[] = {LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(
        &tran,
        prop,
        lv_anim_path_ease_out,
        150,
        0,
        NULL);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_FOCUSED);

    lv_obj_update_layout(obj);

    return obj;
}

static void button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_current_target(e);

    /* Transparent background style */
    static lv_style_t style_scr_act;
    if (style_scr_act.prop_cnt == 0)
    {
        lv_style_init(&style_scr_act);
        lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
        lv_obj_add_style(lv_scr_act(), &style_scr_act, 0);
    }

    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (isPlaying == false)
        {
            isPlaying = true;
            lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PAUSE, 0);

            lv_disp_get_default()->driver->screen_transp = 1;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_TRANSP);
            /* Empty the buffer, not emptying will cause the UI to be opaque */
            lv_memset_00(lv_disp_get_default()->driver->draw_buf->buf_act,
                         lv_disp_get_default()->driver->draw_buf->size * sizeof(lv_color32_t));
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_TRANSP);
            lv_obj_report_style_change(&style_scr_act);

            TPlayerStart(mTPlayer);
        }
        else
        {
            isPlaying = false;
            lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLAY, 0);

            lv_disp_get_default()->driver->screen_transp = 0;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
            lv_obj_report_style_change(&style_scr_act);

            TPlayerPause(mTPlayer);
        }
    }
}