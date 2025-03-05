#include "../include/common_inc.h"

MediaPlayer mp;

bool isPlaying = false;

lv_obj_t *Btn_Create(lv_obj_t *par, const void *img_src, lv_coord_t y_ofs);
static void button_event_handler(lv_event_t *e);

int main(int argc, char *argv[])
{   
    // 请输入需要播放的文件名称
    printf("[Sys] EasyMediaPlayer begin!\n");
    if (argc == 1)
    {
        printf("[Sys] Please input file name!\n");
        return -1;
    }
    // 清除fb0
    system("dd if=/dev/zero of=/dev/fb0");
    
    // 打开音频通路并设置音量
    system("amixer sset Headphone unmute");
    system("amixer sset \"Headphone volume\" 2");

    // Init HAL
    HAL::Init();

    // tplayer初始化
    std::string videoPath = argv[1];
    mp.SetNewVideo(videoPath);

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

    /* Handle LitlevGL tasks (tickless mode) */
    while (1)
    {
        lv_task_handler();
        usleep(5000);
    }

    return 0;
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

            mp.Start();
        }
        else
        {
            isPlaying = false;
            lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLAY, 0);

            lv_disp_get_default()->driver->screen_transp = 0;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
            lv_obj_report_style_change(&style_scr_act);

            mp.Pause();
        }
    }
}