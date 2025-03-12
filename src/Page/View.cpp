#include "View.h"

using namespace Page;

void View::create(Operations &opts)
{
    // 获取View回调函数集
    _opts = opts;

    // 总画布的创建
    contCreate(lv_scr_act());

    // 播放列表画布的创建
    listContCreate(ui.cont);

    // 按钮画布的创建
    btnContCreate(ui.cont);

    // 进度条画布的创建
    sliderContCreate(ui.cont);

    // 为当前屏幕添加事件回调函数
    AttachEvent(lv_scr_act());

    // 为进度条添加事件回调函数
    lv_obj_add_event_cb(ui.sliderCont.slider, sliderEventHandler, LV_EVENT_ALL, this);

    // 动画的创建
    ui.anim_timeline = lv_anim_timeline_create();
    ui.anim_timelineClick = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

#define ANIM_OPA_DEF(start_time, obj) \
    ANIM_DEF(start_time, obj, opa_scale, LV_OPA_COVER, LV_OPA_TRANSP)

    lv_anim_timeline_wrapper_t wrapper[] =
        {
            ANIM_DEF(0, ui.btnCont.cont, height, 20, lv_obj_get_height(ui.btnCont.cont)),
            ANIM_DEF(0, ui.btnCont.cont, width, 20, lv_obj_get_width(ui.btnCont.cont)),

            LV_ANIM_TIMELINE_WRAPPER_END // 这个标志着结构体成员结束，不能省略，在下面函数lv_anim_timeline_add_wrapper的轮询中做判断条件
        };
    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);

    lv_coord_t xOriginal = lv_obj_get_x_aligned(lv_obj_get_child(ui.btnCont.cont, 1));
    lv_coord_t yOriginal = lv_obj_get_y_aligned(lv_obj_get_child(ui.btnCont.cont, 1));

    lv_anim_timeline_wrapper_t wrapperForClick[] =
        {
            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 1), x, xOriginal, xOriginal - 30),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 1)),
            {300, lv_obj_get_child(ui.btnCont.cont, 1), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 2), x, xOriginal, xOriginal - 21),
            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 2), y, yOriginal, yOriginal - 21),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 2)),
            {300, lv_obj_get_child(ui.btnCont.cont, 2), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 3), y, yOriginal, yOriginal - 30),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 3)),
            {300, lv_obj_get_child(ui.btnCont.cont, 3), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 4), x, xOriginal, xOriginal + 21),
            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 4), y, yOriginal, yOriginal - 21),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 4)),
            {300, lv_obj_get_child(ui.btnCont.cont, 4), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 5), x, xOriginal, xOriginal + 30),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 5)),
            {300, lv_obj_get_child(ui.btnCont.cont, 5), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 6), x, xOriginal, xOriginal + 21),
            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 6), y, yOriginal, yOriginal + 21),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 6)),
            {300, lv_obj_get_child(ui.btnCont.cont, 6), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 7), y, yOriginal, yOriginal + 30),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 7)),
            {300, lv_obj_get_child(ui.btnCont.cont, 7), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 8), x, xOriginal, xOriginal - 21),
            ANIM_DEF(0, lv_obj_get_child(ui.btnCont.cont, 8), y, yOriginal, yOriginal + 21),
            ANIM_OPA_DEF(300, lv_obj_get_child(ui.btnCont.cont, 8)),
            {300, lv_obj_get_child(ui.btnCont.cont, 8), (lv_anim_exec_xcb_t)lv_obj_set_shadow_opa_scale, LV_OPA_COVER, LV_OPA_TRANSP, 500, lv_anim_path_ease_out, true},

            LV_ANIM_TIMELINE_WRAPPER_END // 这个标志着结构体成员结束，不能省略，在下面函数lv_anim_timeline_add_wrapper的轮询中做判断条件
        };
    lv_anim_timeline_add_wrapper(ui.anim_timelineClick, wrapperForClick);

    // 开始动画
    appearAnimStart();
}

void View::release()
{
    if (ui.anim_timeline)
    {
        lv_anim_timeline_del(ui.anim_timeline);
        ui.anim_timeline = nullptr;
    }
    if (ui.anim_timelineClick)
    {
        lv_anim_timeline_del(ui.anim_timelineClick);
        ui.anim_timelineClick = nullptr;
    }
    // 移除屏幕手势回调函数
    lv_obj_remove_event_cb(lv_scr_act(), onEvent);

    // 释放内存
    lv_obj_t *listBtn = nullptr;
    while ((listBtn = lv_obj_get_child(ui.listCont.cont, -1)) != nullptr)
    {
        char *video_name = (char *)lv_obj_get_user_data(listBtn);
        if (video_name != nullptr)
            delete[] video_name;

        lv_obj_del(listBtn);
    }
}

void View::appearAnimStart(bool reverse) // 开始开场动画
{
    lv_anim_timeline_set_reverse(ui.anim_timeline, reverse);
    lv_anim_timeline_start(ui.anim_timeline);
}

void View::appearAnimClick(bool reverse) // 按钮动画
{
    lv_anim_timeline_set_reverse(ui.anim_timelineClick, reverse);
    lv_anim_timeline_start(ui.anim_timelineClick);
}

void View::AttachEvent(lv_obj_t *obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

// 总画布的创建
void View::contCreate(lv_obj_t *obj)
{
    lv_obj_t *cont = lv_obj_create(obj);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xcccccc), 0);
    lv_obj_set_style_bg_img_src(cont, "S:./res/icon/main1.bin", 0);
    lv_obj_set_style_bg_img_opa(cont, LV_OPA_COVER, 0);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    ui.cont = cont;
}

// 按钮画布的创建
void View::btnContCreate(lv_obj_t *obj)
{
    lv_obj_t *btnCont = lv_obj_create(obj);
    lv_obj_remove_style_all(btnCont);
    lv_obj_set_size(btnCont, lv_pct(70), LV_VER_RES / 4);
    lv_obj_clear_flag(btnCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(btnCont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btnCont, lv_color_hex(0x6a8d6d), 0);
    lv_obj_align(btnCont, LV_ALIGN_BOTTOM_MID, 0, 42);
    lv_obj_set_style_radius(btnCont, 16, LV_PART_MAIN);
    ui.btnCont.cont = btnCont;

    lv_obj_t *btn = btnCreate(btnCont, LV_SYMBOL_PLAY, -20);
    lv_obj_add_event_cb(btn, buttonEventHandler, LV_EVENT_ALL, this);
    ui.btnCont.btn = btn;

    /* Render octagon explode */
    lv_obj_t *roundRect_1 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_2 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_3 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_4 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_5 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_6 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_7 = roundRectCreate(btnCont, 0, -20);
    lv_obj_t *roundRect_8 = roundRectCreate(btnCont, 0, -20);
}

void View::sliderContCreate(lv_obj_t *obj)
{
    lv_obj_t *sliderCont = lv_obj_create(obj);
    lv_obj_remove_style_all(sliderCont);
    lv_obj_set_size(sliderCont, lv_pct(90), LV_VER_RES / 4);
    lv_obj_clear_flag(sliderCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(sliderCont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(sliderCont, lv_color_hex(0x6a8d6d), 0);
    lv_obj_align(sliderCont, LV_ALIGN_TOP_MID, 0, -42);
    lv_obj_set_style_radius(sliderCont, 16, LV_PART_MAIN);

    ui.sliderCont.cont = sliderCont;

    ui.sliderCont.slider = sliderCreate(sliderCont, nullptr, 0, 20);

}

void View::listContCreate(lv_obj_t *obj)
{
    lv_obj_t *listCont = lv_list_create(obj);
    lv_obj_remove_style_all(listCont);
    lv_obj_set_size(listCont, lv_pct(80), lv_pct(50));
    // lv_obj_clear_flag(listCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(listCont, LV_OPA_60, 0);
    lv_obj_set_style_bg_color(listCont, lv_color_hex(0x6a8d6d), 0);
    lv_obj_align(listCont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(listCont, 16, LV_PART_MAIN);

    lv_obj_set_style_pad_row(listCont, 20, LV_PART_MAIN);
    lv_obj_set_flex_flow(listCont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(listCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scroll_dir(listCont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(listCont, LV_SCROLL_SNAP_CENTER);

    ui.listCont.cont = listCont;
}

lv_obj_t *View::sliderCreate(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs, lv_coord_t y_ofs, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t *obj = lv_slider_create(par);
    lv_obj_remove_style_all(obj);
    lv_slider_set_mode(obj, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_range(obj, min, max);
    lv_slider_set_value(obj, val, LV_ANIM_OFF);

    lv_obj_set_size(obj, lv_pct(85), lv_pct(20));

    lv_obj_set_style_border_width(obj, 4, LV_PART_KNOB);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xbbbbbb), LV_PART_KNOB);
    lv_obj_set_style_pad_all(obj, 6, LV_PART_KNOB);
    lv_obj_set_style_radius(obj, 10, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, LV_OPA_60, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x445588), LV_PART_KNOB);

    lv_obj_set_style_radius(obj, 5, LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x3c9ba6), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_set_style_radius(obj, 8, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xa4d9b2), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_align(obj, LV_ALIGN_CENTER, x_ofs, y_ofs);

    if (img_src != nullptr)
    {
        lv_obj_t *img = lv_img_create(obj);
        lv_obj_align(img, LV_ALIGN_LEFT_MID, 10, 0);
        lv_img_set_src(img, img_src);
        // lv_obj_set_style_img_opa(img, LV_OPA_50, LV_PART_MAIN);
        // lv_obj_set_style_img_recolor_opa(img, LV_OPA_COVER, LV_PART_MAIN);
        // lv_obj_set_style_img_recolor(img, lv_color_white(), LV_PART_MAIN);
    }

    return obj;
}

lv_obj_t *View::btnCreate(lv_obj_t *par, const void *img_src, lv_coord_t y_ofs)
{
    lv_obj_t *obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 180, 50);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_CENTER, 0, y_ofs);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, 75, LV_STATE_PRESSED);                         // 设置button按下时的宽
    lv_obj_set_style_height(obj, 25, LV_STATE_PRESSED);                        // 设置button按下时的长
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x356b8c), 0);                 // 设置按钮默认的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x242947), LV_STATE_PRESSED);  // 设置按钮在被按下时的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf2daaa), LV_STATE_FOCUSED);  // 设置按钮在被聚焦时的颜色
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xa99991), LV_STATE_DISABLED); // 设置按钮失能时的颜色
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

lv_obj_t *View::listCreate(const char *name, const void *img_src)
{
    // lv_obj_t *obj = lv_list_add_btn(ui.listCont.cont, img_src, name);
    // lv_obj_t *obj = lv_list_add_btn(ui.listCont.cont, LV_SYMBOL_PLAY, name);

    lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_btn_class, ui.listCont.cont);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

    lv_obj_t *img = lv_img_create(obj);
    lv_img_set_src(img, LV_SYMBOL_PLAY);

    lv_obj_t *label = lv_label_create(obj);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_label_set_text(label, name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);

    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_STATE_DEFAULT); // 设置背景透明
    lv_obj_set_style_bg_opa(obj, LV_OPA_30, LV_STATE_PRESSED);     // 设置背景透明度(按下时)
    lv_obj_set_style_text_color(obj, lv_color_hex(0x282a3a), LV_STATE_DEFAULT);

    return obj;
}

lv_obj_t *View::roundRectCreate(lv_obj_t *par, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    /* Render octagon explode */
    lv_obj_t *roundRect = lv_obj_create(par);
    lv_obj_remove_style_all(roundRect);
    lv_obj_set_size(roundRect, 10, 10);
    lv_obj_set_style_radius(roundRect, 2, 0);

    lv_obj_set_style_shadow_width(roundRect, 10, 0);
    lv_obj_set_style_shadow_ofs_x(roundRect, 1, 0);
    lv_obj_set_style_shadow_ofs_y(roundRect, 1, 0);
    lv_obj_set_style_shadow_color(roundRect, lv_color_hex(0x5d8c3d), 0);
    lv_obj_set_style_shadow_spread(roundRect, 1, 0);
    lv_obj_set_style_shadow_opa(roundRect, LV_OPA_TRANSP, 0);

    lv_obj_set_style_bg_color(roundRect, lv_color_hex(0x88d35e), 0);
    lv_obj_set_style_bg_opa(roundRect, LV_OPA_TRANSP, 0);
    lv_obj_align(roundRect, LV_ALIGN_CENTER, x_ofs, y_ofs);

    return roundRect;
}

/**
 * @brief 添加一个视频到列表
 * @param name 视频文件名称
 * @param img_src 视频文件封面
 */
void View::addVideoList(const char *name, const void *img_src)
{
    lv_obj_t *obj = listCreate(name, img_src);

    int len = strlen(name) + 1;
    char *video_name = new char[len];
    strcpy(video_name, name);

    lv_obj_set_user_data(obj, video_name);

    printf("[View] videoName:%s\n", video_name);

    lv_obj_add_event_cb(obj, listBtnEventHandler, LV_EVENT_SHORT_CLICKED, this);
}

/**
 * @brief 设置视频播放进度显示
 */
void View::setPlayProgress(int cur, int total)
{
}

void View::buttonEventHandler(lv_event_t *event)
{
    View *instance = (View *)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_current_target(event);

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
        instance->appearAnimClick();

        if (instance->_isPlaying == false)
        {
            instance->_isPlaying = true;
            lv_obj_set_style_bg_img_src(obj, LV_SYMBOL_PAUSE, 0);

            lv_obj_set_style_bg_img_opa(instance->ui.cont, LV_OPA_TRANSP, 0);
            lv_obj_add_flag(instance->ui.listCont.cont, LV_OBJ_FLAG_HIDDEN);

            lv_disp_get_default()->driver->screen_transp = 1;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_TRANSP);
            /* Empty the buffer, not emptying will cause the UI to be opaque */
            lv_memset_00(lv_disp_get_default()->driver->draw_buf->buf_act,
                         lv_disp_get_default()->driver->draw_buf->size * sizeof(lv_color32_t));
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_TRANSP);
            lv_obj_report_style_change(&style_scr_act);

            if (instance->_opts.playCb != nullptr)
                instance->_opts.playCb(NULL); // 继续播放
        }
        else
        {
            instance->_isPlaying = false;
            lv_obj_set_style_bg_img_src(obj, LV_SYMBOL_PLAY, 0);

            lv_obj_set_style_bg_img_opa(instance->ui.cont, LV_OPA_COVER, 0);
            lv_obj_clear_flag(instance->ui.listCont.cont, LV_OBJ_FLAG_HIDDEN);

            lv_disp_get_default()->driver->screen_transp = 0;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
            lv_obj_report_style_change(&style_scr_act);

            if (instance->_opts.pauseCb != nullptr)
                instance->_opts.pauseCb(); // 暂停播放
        }
    }
}

void View::sliderEventHandler(lv_event_t *event)
{
    View *instance = (View *)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t *obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // lv_indev_wait_release(lv_indev_get_act());
    }
    if (code == LV_EVENT_RELEASED)
    {
        int cur = lv_slider_get_value(obj);
        printf("[View] Get slider value: %d\n", cur);

        if (instance->_opts.setCurCb != nullptr)
        {
            instance->_opts.setCurCb(cur);
        }
    }
}

void View::listBtnEventHandler(lv_event_t *event)
{
    View *instance = (View *)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *obj = lv_event_get_current_target(event);
    const char *videoName = (const char *)lv_obj_get_user_data(obj);

    printf("[View] Cb videoName:%s\n", videoName);

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
        instance->_isPlaying = true;

        lv_obj_set_style_bg_img_src(instance->ui.btnCont.btn, LV_SYMBOL_PAUSE, 0);
        lv_obj_set_style_bg_img_opa(instance->ui.cont, LV_OPA_TRANSP, 0);
        lv_obj_add_flag(instance->ui.listCont.cont, LV_OBJ_FLAG_HIDDEN);

        lv_disp_get_default()
            ->driver->screen_transp = 1;
        lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_TRANSP);
        /* Empty the buffer, not emptying will cause the UI to be opaque */
        lv_memset_00(lv_disp_get_default()->driver->draw_buf->buf_act,
                     lv_disp_get_default()->driver->draw_buf->size * sizeof(lv_color32_t));
        lv_style_set_bg_opa(&style_scr_act, LV_OPA_TRANSP);
        lv_obj_report_style_change(&style_scr_act);

        if (instance->_opts.playCb != nullptr)
        {
            instance->_opts.playCb(videoName); // 新视频播放
        }
    }
}

void View::onEvent(lv_event_t *event)
{
    View *instance = (View *)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t *obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_GESTURE)
    {
        switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
        {
        case LV_DIR_LEFT:
            printf("[View] LV_DIR_LEFT!\n");

            break;
        case LV_DIR_RIGHT:
            printf("[View] LV_DIR_RIGHT!\n");

            break;
        case LV_DIR_TOP:
            printf("[View] LV_DIR_TOP!\n");

            break;
        case LV_DIR_BOTTOM:
            printf("[View] LV_DIR_BOTTOM!\n");
            instance->_opts.exitCb();
            break;

        default:
            break;
        }
    }
}