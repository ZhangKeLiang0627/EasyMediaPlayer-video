#include "View.h"

using namespace Page;

void View::create(Operations &opts)
{
    // 获取View回调函数集
    _opts = opts;

    // 初始化字体
    fontCreate();

    // 总画布的创建
    contCreate(lv_scr_act());
    // 播放列表画布的创建
    listContCreate(ui.cont);
    // 功能按钮画布的创建
    funcContCreate(ui.cont);
    // 按钮画布的创建
    btnContCreate(ui.cont);
    // 音量、亮度条画布的创建
    sliderContCreate(ui.cont);
    // topContCreate
    topContCreate(ui.cont);

    // 添加事件回调函数
    AttachEvent(lv_scr_act());
    lv_obj_add_event_cb(ui.btnCont.btn, buttonEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.funcCont.speedBtn, buttonEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.funcCont.fullScreenBtn, buttonEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.topCont.cancelBtn, buttonEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.topCont.lockBtn, buttonEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.topCont.listBtn, buttonEventHandler, LV_EVENT_ALL, this);

    lv_obj_add_event_cb(ui.btnCont.slider, sliderEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.sliderCont.volumeSlider, sliderEventHandler, LV_EVENT_ALL, this);
    lv_obj_add_event_cb(ui.sliderCont.brightnessSlider, sliderEventHandler, LV_EVENT_ALL, this);

    // 动画的创建
    ui.anim_timeline = lv_anim_timeline_create();
    ui.anim_timelineClick = lv_anim_timeline_create();
    ui.anim_timelineSlider = lv_anim_timeline_create();
    ui.anim_timelineTop = lv_anim_timeline_create();

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

#define ANIM_OPA_DEF(start_time, obj) \
    ANIM_DEF(start_time, obj, opa_scale, LV_OPA_COVER, LV_OPA_TRANSP)

    lv_anim_timeline_wrapper_t wrapper[] =
        {
            ANIM_DEF(0, ui.btnCont.cont, height, 20, lv_obj_get_height(ui.btnCont.cont)),
            ANIM_DEF(0, ui.btnCont.cont, width, 20, lv_obj_get_width(ui.btnCont.cont)),
            ANIM_DEF(100, ui.funcCont.cont, y, 480, lv_obj_get_y_aligned(ui.funcCont.cont)),

            LV_ANIM_TIMELINE_WRAPPER_END // 这个标志着结构体成员结束，不能省略，在下面函数lv_anim_timeline_add_wrapper的轮询中做判断条件
        };
    lv_anim_timeline_add_wrapper(ui.anim_timeline, wrapper);

    lv_anim_timeline_wrapper_t wrapperSlider[] =
        {
            ANIM_DEF(0, ui.sliderCont.cont, x, lv_obj_get_x_aligned(ui.sliderCont.cont), 150),
            ANIM_DEF(0, ui.sliderCont.cont, y, lv_obj_get_y_aligned(ui.sliderCont.cont), -20),
            // ANIM_DEF(0, ui.sliderCont.cont, width, lv_obj_get_width(ui.sliderCont.cont), 80),
            ANIM_DEF(0, ui.sliderCont.cont, height, lv_obj_get_height(ui.sliderCont.cont), 30),

            LV_ANIM_TIMELINE_WRAPPER_END // 这个标志着结构体成员结束，不能省略，在下面函数lv_anim_timeline_add_wrapper的轮询中做判断条件
        };
    lv_anim_timeline_add_wrapper(ui.anim_timelineSlider, wrapperSlider);

    lv_anim_timeline_wrapper_t wrapperTop[] =
        {
            ANIM_DEF(0, ui.topCont.cont, y, -40, lv_obj_get_x_aligned(ui.btnCont.cont)),
            ANIM_DEF(0, ui.topCont.cont, width, 20, lv_obj_get_width(ui.btnCont.cont)),

            LV_ANIM_TIMELINE_WRAPPER_END // 这个标志着结构体成员结束，不能省略，在下面函数lv_anim_timeline_add_wrapper的轮询中做判断条件
        };
    lv_anim_timeline_add_wrapper(ui.anim_timelineTop, wrapperTop);

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
    appearAnimSlider();
    appearAnimTop();
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

    ui.isBtnContCollapsed = reverse;
}

void View::appearAnimSlider(bool reverse) // 音量条/亮度条动画
{
    lv_anim_timeline_set_reverse(ui.anim_timelineSlider, reverse);
    lv_anim_timeline_start(ui.anim_timelineSlider);

    ui.isSliderContCollapsed = reverse;
}

void View::appearAnimClick(bool reverse) // 按钮动画
{
    lv_anim_timeline_set_reverse(ui.anim_timelineClick, reverse);
    lv_anim_timeline_start(ui.anim_timelineClick);
}

void View::appearAnimTop(bool reverse) // topCont动画
{
    lv_anim_timeline_set_reverse(ui.anim_timelineTop, reverse);
    lv_anim_timeline_start(ui.anim_timelineTop);

    ui.isTopContCollapsed = reverse;
}

void View::AttachEvent(lv_obj_t *obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

// 自定义字体初始化
void View::fontCreate(void)
{
    ui.fontCont.font16.name = "/mnt/UDISK/font/SmileySans.ttf";
    ui.fontCont.font16.weight = 16;
    ui.fontCont.font16.style = FT_FONT_STYLE_NORMAL;
    ui.fontCont.font16.mem = nullptr;
    lv_ft_font_init(&ui.fontCont.font16);

    ui.fontCont.font20.name = "/mnt/UDISK/font/SmileySans.ttf";
    ui.fontCont.font20.weight = 20;
    ui.fontCont.font20.style = FT_FONT_STYLE_NORMAL;
    ui.fontCont.font20.mem = nullptr;
    lv_ft_font_init(&ui.fontCont.font20);
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
    lv_obj_set_style_bg_img_src(cont, "S:./picture/cover/main1.bin", 0);
    lv_obj_set_style_bg_img_opa(cont, LV_OPA_COVER, 0);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    ui.cont = cont;
}

// 按钮画布的创建
void View::btnContCreate(lv_obj_t *obj)
{
    lv_obj_t *btnCont = lv_obj_create(obj);
    lv_obj_remove_style_all(btnCont);
    lv_obj_set_size(btnCont, lv_pct(90), LV_VER_RES / 4);
    lv_obj_clear_flag(btnCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(btnCont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(btnCont, lv_color_hex(0x6a8d6d), 0);
    lv_obj_align(btnCont, LV_ALIGN_BOTTOM_MID, 0, 50);
    lv_obj_set_style_radius(btnCont, 16, LV_PART_MAIN);
    ui.btnCont.cont = btnCont;

    lv_obj_t *btn = btnCreate(btnCont, LV_SYMBOL_PLAY, 10, -25);
    ui.btnCont.btn = btn;

    /* Render octagon explode */
    lv_obj_t *roundRect_1 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_2 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_3 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_4 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_5 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_6 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_7 = roundRectCreate(btnCont, 30, -25);
    lv_obj_t *roundRect_8 = roundRectCreate(btnCont, 30, -25);

    lv_obj_t *slider = sliderCreate(btnCont, nullptr, 30, -15);
    ui.btnCont.slider = slider;

    lv_obj_t *label = lv_label_create(btnCont);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_font(label, ui.fontCont.font16.font, LV_STATE_DEFAULT);
    lv_obj_align(label, LV_ALIGN_TOP_MID, -100, 5);
    lv_label_set_text_fmt(label, "%s", "0:0/0:0");
    ui.btnCont.timeLabel = label;
}

// 功能按键画布的创建
void View::funcContCreate(lv_obj_t *obj)
{
    lv_obj_t *cont = lv_obj_create(obj);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(26), lv_pct(10));
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xff9b5e), 0);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_LEFT, 40, -60);
    lv_obj_set_style_radius(cont, 16, LV_PART_MAIN);

    lv_obj_set_style_shadow_width(cont, 10, 0);
    lv_obj_set_style_shadow_ofs_x(cont, 4, 0);
    lv_obj_set_style_shadow_ofs_y(cont, 2, 0);
    lv_obj_set_style_shadow_color(cont, lv_color_hex(0xe36f47), 0);

    ui.funcCont.cont = cont;

    lv_obj_t *btn = nullptr;
    btn = btnCreate(cont, nullptr, 6, -4, 30, 30);
    ui.funcCont.speedBtn = btn;
    lv_obj_t *label = lv_label_create(ui.funcCont.speedBtn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_font(label, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_center(label);
    lv_label_set_text_fmt(label, "%s", "x1");
    ui.funcCont.speedLabel = label;

    btn = btnCreate(cont, LV_SYMBOL_LOOP, 47, -4, 30, 30);
    ui.funcCont.loopBtn = btn;

    btn = btnCreate(cont, nullptr, 88, -4, 30, 30);
    ui.funcCont.fullScreenBtn = btn;
    label = lv_label_create(ui.funcCont.fullScreenBtn);
    lv_obj_remove_style_all(label);
    lv_obj_set_style_text_font(label, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_center(label);
    lv_label_set_text_fmt(label, "%s", "半");
    ui.funcCont.fullScreenLabel = label;

    // btn = btnCreate(cont, nullptr, 88, -4, 30, 30);
    // ui.funcCont.rotateBtn = btn;
    // label = lv_label_create(ui.funcCont.rotateBtn);
    // lv_obj_remove_style_all(label);
    // lv_obj_set_style_text_font(label, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    // lv_obj_center(label);
    // lv_label_set_text_fmt(label, "%s", "0°");
    // ui.funcCont.rotateLabel = label;
}

// 音量、亮度条画布的创建
void View::sliderContCreate(lv_obj_t *obj)
{
    lv_obj_t *sliderCont = lv_obj_create(obj);
    lv_obj_remove_style_all(sliderCont);
    lv_obj_set_size(sliderCont, lv_pct(30), lv_pct(40));
    lv_obj_clear_flag(sliderCont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(sliderCont, LV_OPA_90, 0);
    lv_obj_set_style_bg_color(sliderCont, lv_color_hex(0xeeeeee), 0);
    lv_obj_align(sliderCont, LV_ALIGN_TOP_RIGHT, -20, 60);
    lv_obj_set_style_radius(sliderCont, 10, LV_PART_MAIN);

    ui.sliderCont.cont = sliderCont;

    ui.sliderCont.volumeSlider = sliderCreate(sliderCont, nullptr, 0, 0);
    lv_obj_set_style_bg_opa(ui.sliderCont.volumeSlider, LV_OPA_TRANSP, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui.sliderCont.volumeSlider, LV_OPA_TRANSP, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui.sliderCont.volumeSlider, 0, LV_PART_KNOB);
    lv_obj_set_size(ui.sliderCont.volumeSlider, lv_pct(40), lv_pct(90));
    lv_obj_align(ui.sliderCont.volumeSlider, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_t *imgVloume = lv_img_create(ui.sliderCont.volumeSlider);
    lv_img_set_src(imgVloume, LV_SYMBOL_VOLUME_MAX);

    ui.sliderCont.brightnessSlider = sliderCreate(sliderCont, nullptr, 0, 0);
    lv_obj_set_style_bg_opa(ui.sliderCont.brightnessSlider, LV_OPA_TRANSP, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui.sliderCont.brightnessSlider, LV_OPA_TRANSP, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(ui.sliderCont.brightnessSlider, 0, LV_PART_KNOB);
    lv_obj_set_size(ui.sliderCont.brightnessSlider, lv_pct(40), lv_pct(90));
    lv_obj_align(ui.sliderCont.brightnessSlider, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_t *imgBrightness = lv_img_create(ui.sliderCont.brightnessSlider);
    lv_img_set_src(imgBrightness, LV_SYMBOL_EYE_OPEN);

    lv_slider_set_range(ui.sliderCont.volumeSlider, 0, 40);
    lv_slider_set_value(ui.sliderCont.volumeSlider, 20, LV_ANIM_OFF);

    lv_slider_set_range(ui.sliderCont.brightnessSlider, 0, 255);
    lv_slider_set_value(ui.sliderCont.brightnessSlider, 255, LV_ANIM_OFF);
}

void View::listContCreate(lv_obj_t *obj)
{
    lv_obj_t *cont = lv_obj_create(obj);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(80), lv_pct(50));
    lv_obj_set_style_bg_opa(cont, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x9cd1bb), LV_PART_MAIN);
    // lv_obj_set_style_bg_img_src(cont, ResourcePool::GetImage("popcat_open"), LV_PART_MAIN); // 设置背景图片
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(cont, 16, LV_PART_MAIN);

    lv_obj_set_style_pad_all(cont, 25, LV_PART_MAIN); // 设置每一个item的宽度
    lv_obj_set_style_pad_row(cont, 20, LV_PART_MAIN); // 设置每一个item的间距

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);       // 设置弹性布局，item竖着排
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);               // 设置画布滚动方向：垂直滚动
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER); // 设置在垂直滚动结束时捕捉子元素的位置：人话：打开菜单第一个item的位置，现在是居中
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_ON); // 设置滚动条是否显示：是

    ui.listCont.cont = cont;
}

// void View::listContCreate(lv_obj_t *obj)
// {
//     lv_obj_t *listCont = lv_list_create(obj);
//     lv_obj_remove_style_all(listCont);
//     lv_obj_set_size(listCont, lv_pct(80), lv_pct(50));
//     // lv_obj_clear_flag(listCont, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_set_style_bg_opa(listCont, LV_OPA_60, 0);
//     lv_obj_set_style_bg_color(listCont, lv_color_hex(0x6a8d6d), 0);
//     lv_obj_align(listCont, LV_ALIGN_CENTER, 0, 0);
//     lv_obj_set_style_radius(listCont, 16, LV_PART_MAIN);
//     lv_obj_set_style_pad_row(listCont, 20, LV_PART_MAIN);

//     lv_obj_set_flex_flow(listCont, LV_FLEX_FLOW_COLUMN);
//     // lv_obj_set_flex_align(listCont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
//     lv_obj_set_scroll_dir(listCont, LV_DIR_VER);
//     lv_obj_set_scroll_snap_y(listCont, LV_SCROLL_SNAP_NONE);
//     lv_obj_set_scrollbar_mode(listCont, LV_SCROLLBAR_MODE_ON); // 设置滚动条是否显示：是

//     // lv_obj_set_style_bg_opa(listCont, LV_OPA_100, LV_PART_SCROLLBAR);

//     ui.listCont.cont = listCont;
// }

void View::topContCreate(lv_obj_t *obj)
{
    lv_obj_t *cont = lv_obj_create(obj);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(90), lv_pct(8));
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_90, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xeeeeee), 0);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_radius(cont, 5, LV_PART_MAIN);
    ui.topCont.cont = cont;

    lv_obj_t *btn = btnCreate(cont, nullptr, 0, 0, 30, 30);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -5, 4);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xff6056), 0); // 设置按钮默认的颜色
    ui.topCont.cancelBtn = btn;
    lv_obj_t *cancelBtnLabel = lv_label_create(ui.topCont.cancelBtn);
    lv_obj_remove_style_all(cancelBtnLabel);
    lv_obj_set_style_text_font(cancelBtnLabel, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cancelBtnLabel, lv_color_hex(0xffffff), 0);
    lv_obj_center(cancelBtnLabel);
    lv_label_set_text_fmt(cancelBtnLabel, "%s", "x");

    btn = btnCreate(cont, nullptr, 0, 0, 40, 30);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -40, 4);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x4ea35a), 0);                // 设置按钮默认的颜色
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x4ea35a), LV_STATE_FOCUSED); // 设置按钮在被聚焦时的颜色
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x646abb), LV_STATE_USER_1);
    ui.topCont.lockBtn = btn;
    lv_obj_t *lockBtnLabel = lv_label_create(ui.topCont.lockBtn);
    lv_obj_remove_style_all(lockBtnLabel);
    lv_obj_set_style_text_font(lockBtnLabel, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lockBtnLabel, lv_color_hex(0xffffff), 0);
    lv_obj_center(lockBtnLabel);
    lv_label_set_text_fmt(lockBtnLabel, "%s", "锁定");
    ui.topCont.lockLabel = lockBtnLabel;

    btn = btnCreate(cont, nullptr, 0, 0, 40, 30);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 5, 4);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe09f00), 0);                // 设置按钮默认的颜色
    lv_obj_set_style_bg_color(btn, lv_color_hex(0xe09f00), LV_STATE_FOCUSED); // 设置按钮在被聚焦时的颜色
    ui.topCont.listBtn = btn;
    lv_obj_t *listBtnLabel = lv_label_create(ui.topCont.listBtn);
    lv_obj_remove_style_all(listBtnLabel);
    lv_obj_set_style_text_font(listBtnLabel, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(listBtnLabel, lv_color_hex(0xffffff), 0);
    lv_obj_center(listBtnLabel);
    lv_label_set_text_fmt(listBtnLabel, "%s", "列表");

    lv_obj_t *videoNameLabel = lv_label_create(cont);
    lv_obj_remove_style_all(videoNameLabel);
    lv_obj_set_style_text_font(videoNameLabel, ui.fontCont.font20.font, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(videoNameLabel, lv_color_black(), 0);
    lv_label_set_long_mode(videoNameLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(videoNameLabel, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(videoNameLabel, "%s", "                     videoName"); // 空格是为了居中
    lv_obj_set_size(videoNameLabel, lv_pct(60), LV_SIZE_CONTENT);

    ui.topCont.videoNameLabel = videoNameLabel;
}

lv_obj_t *View::sliderCreate(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs, lv_coord_t y_ofs, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t *obj = lv_slider_create(par);
    lv_obj_remove_style_all(obj);
    lv_slider_set_mode(obj, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_range(obj, min, max);
    lv_slider_set_value(obj, val, LV_ANIM_OFF);

    lv_obj_set_size(obj, lv_pct(78), lv_pct(20));

    lv_obj_set_style_border_width(obj, 3, LV_PART_KNOB);
    lv_obj_set_style_border_color(obj, lv_color_hex(0xbbbbbb), LV_PART_KNOB);
    lv_obj_set_style_pad_all(obj, 1, LV_PART_KNOB);
    lv_obj_set_style_radius(obj, 10, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, LV_OPA_60, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x445588), LV_PART_KNOB);

    lv_obj_set_style_radius(obj, 8, LV_PART_MAIN);
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

lv_obj_t *View::btnCreate(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs, lv_coord_t y_ofs, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t *obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, w, h);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_align(obj, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);
    lv_obj_set_style_bg_img_src(obj, img_src, 0);

    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_width(obj, w / 1.5f, LV_STATE_PRESSED);                   // 设置button按下时的宽
    lv_obj_set_style_height(obj, h / 1.5f, LV_STATE_PRESSED);                  // 设置button按下时的长
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

    lv_obj_t *obj = lv_obj_create(ui.listCont.cont);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, LV_PCT(98), LV_PCT(23));
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_60, LV_STATE_DEFAULT); // 设置背景透明
    lv_obj_set_style_bg_opa(obj, LV_OPA_80, LV_STATE_PRESSED); // 设置背景透明度(按下时)
    lv_obj_set_style_width(obj, lv_pct(93), LV_STATE_PRESSED); // 设置button按下时的长宽
    // lv_obj_set_style_height(obj, lv_pct(16), LV_STATE_PRESSED);
    lv_obj_set_style_radius(obj, 9, 0); // 按钮画圆角

    lv_obj_set_style_shadow_width(obj, 10, 0);
    lv_obj_set_style_shadow_ofs_x(obj, 5, 0);
    lv_obj_set_style_shadow_ofs_y(obj, 5, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_hex(0xd6dff5), 0);

    lv_obj_t *img = lv_img_create(obj);
    lv_img_set_src(img, LV_SYMBOL_VIDEO);
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t *label = lv_label_create(obj);
    lv_obj_set_size(label, lv_pct(80), LV_SIZE_CONTENT);
    // lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_font(label, ui.fontCont.font20.font, 0);
    lv_label_set_text(label, name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // anim
    static lv_style_transition_dsc_t tran;                                                                       // lv_style_transition_dsc_t用于描述样式过渡的属性。这个数据结构用于定义样式过渡的动画效果，包括过渡的时间、延迟、缓动函数等属性
    static const lv_style_prop_t prop[] = {LV_STYLE_WIDTH, LV_STYLE_HEIGHT, LV_STYLE_BG_OPA, LV_STYLE_PROP_INV}; // lv_style_prop_t枚举类型，用于表示样式的属性。这个枚举类型定义了一系列的样式属性，例如背景颜色、边框宽度、文本颜色等
    lv_style_transition_dsc_init(
        &tran,
        prop,
        lv_anim_path_ease_out,
        150,
        0,
        nullptr);
    lv_obj_set_style_transition(obj, &tran, LV_STATE_PRESSED);
    lv_obj_update_layout(obj);

    return obj;
}

// lv_obj_t *View::listCreate(const char *name, const void *img_src)
// {
//     // lv_obj_t *obj = lv_list_add_btn(ui.listCont.cont, img_src, name);
//     // lv_obj_t *obj = lv_list_add_btn(ui.listCont.cont, LV_SYMBOL_PLAY, name);

//     lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_btn_class, ui.listCont.cont);
//     lv_obj_class_init_obj(obj);
//     lv_obj_set_size(obj, LV_PCT(98), LV_SIZE_CONTENT);
//     lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
//     lv_obj_set_style_radius(obj, 9, 0);                                       // 按钮画圆角
//     lv_obj_set_style_radius(obj, 9, LV_STATE_PRESSED);                        // 按钮画圆角
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xeeeeee), 0);                // 设置按钮默认的颜色
//     lv_obj_set_style_bg_color(obj, lv_color_hex(0xeeeeee), LV_STATE_PRESSED); // 设置按钮在被按下时的颜色
//                                                                               // lv_obj_align_to(obj, ui.listCont.cont, LV_ALIGN_RIGHT_MID, 10, 0);
//     lv_obj_set_style_shadow_width(obj, 10, 0);
//     lv_obj_set_style_shadow_ofs_x(obj, 5, 0);
//     lv_obj_set_style_shadow_ofs_y(obj, 5, 0);
//     lv_obj_set_style_shadow_color(obj, lv_color_hex(0xd6dff5), 0);

//     lv_obj_t *img = lv_img_create(obj);
//     lv_img_set_src(img, LV_SYMBOL_VIDEO);

//     lv_obj_t *label = lv_label_create(obj);
//     // lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
//     lv_obj_set_style_text_font(label, ui.fontCont.font20.font, 0);
//     lv_label_set_text(label, name);
//     lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
//     lv_obj_set_flex_grow(label, 1);

//     lv_obj_set_style_bg_opa(obj, LV_OPA_60, LV_STATE_DEFAULT); // 设置背景透明
//     lv_obj_set_style_bg_opa(obj, LV_OPA_80, LV_STATE_PRESSED); // 设置背景透明度(按下时)
//     lv_obj_set_style_text_color(obj, lv_color_hex(0x282a3a), LV_STATE_DEFAULT);

//     return obj;
// }

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
    lv_obj_align(roundRect, LV_ALIGN_LEFT_MID, x_ofs, y_ofs);

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
 * @brief 更新视频播放进度显示
 */
void View::setPlayProgress(int cur, int total)
{
    lv_slider_set_range(ui.btnCont.slider, 0, total);
    if (!lv_obj_has_state(ui.btnCont.slider, LV_STATE_PRESSED)) // 未按下时设置
        lv_slider_set_value(ui.btnCont.slider, cur, LV_ANIM_OFF);

    lv_label_set_text_fmt(ui.btnCont.timeLabel, "%d:%d/%d:%d", cur / 60, cur % 60, total / 60, total % 60);
}

/**
 * @brief 更新音量显示
 */
void View::setVolumeProgress(int cur, int total)
{
    lv_slider_set_range(ui.sliderCont.volumeSlider, 0, total);
    if (!lv_obj_has_state(ui.sliderCont.volumeSlider, LV_STATE_PRESSED)) // 未按下时设置
        lv_slider_set_value(ui.sliderCont.volumeSlider, cur, LV_ANIM_OFF);
}

/**
 * @brief 更新亮度显示
 */
void View::setBrightnessProgress(int cur, int total)
{
    lv_slider_set_range(ui.sliderCont.brightnessSlider, 0, total);
    if (!lv_obj_has_state(ui.sliderCont.brightnessSlider, LV_STATE_PRESSED)) // 未按下时设置
        lv_slider_set_value(ui.sliderCont.brightnessSlider, cur, LV_ANIM_OFF);
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
        if (obj == instance->ui.btnCont.btn)
        {
            instance->appearAnimClick();

            if (instance->_isPlaying == false)
            {
                // if video load is not empty
                if (strcmp(lv_label_get_text(instance->ui.topCont.videoNameLabel), "videoName") != 0)
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
            }
            else
            {
                instance->_isPlaying = false;
                lv_obj_set_style_bg_img_src(obj, LV_SYMBOL_PLAY, 0);

                // lv_obj_set_style_bg_img_opa(instance->ui.cont, LV_OPA_COVER, 0);
                // lv_obj_clear_flag(instance->ui.listCont.cont, LV_OBJ_FLAG_HIDDEN);

                // lv_disp_get_default()->driver->screen_transp = 0;
                // lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
                // lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
                // lv_obj_report_style_change(&style_scr_act);

                lv_label_set_text_fmt(instance->ui.funcCont.speedLabel, "x%d", 1);

                if (instance->_opts.pauseCb != nullptr)
                    instance->_opts.pauseCb(); // 暂停播放
            }
        }

        if (obj == instance->ui.funcCont.speedBtn)
        {
            static int index = 0;

            if (instance->_opts.setSpeedCb != nullptr)
            {
                if (instance->_opts.getStateCb())
                {
                    // 如果在播放
                    index += 1;
                    index = index > 4 ? 0 : index;
                    instance->_opts.setSpeedCb(4 - index); // 设置倍速
                }
            }
            else
            {
                index = 0;
            }
            lv_label_set_text_fmt(instance->ui.funcCont.speedLabel, "x%d", 1 << index);
        }
        else if (obj == instance->ui.funcCont.fullScreenBtn)
        {
            static bool state = false;

            if (instance->_opts.setFullScreenCb != nullptr)
            {
                // 当前未播放
                if (!instance->_opts.getStateCb())
                {
                    state = !state;
                    instance->_opts.setFullScreenCb(state);
                }
            }

            lv_label_set_text_fmt(instance->ui.funcCont.fullScreenLabel, "%s", state ? "全" : "半");
        }
        else if (obj == instance->ui.topCont.lockBtn)
        {
            bool state = lv_obj_has_state(obj, LV_STATE_USER_1);
            if (state)
            {
                lv_obj_clear_state(obj, LV_STATE_USER_1);
                instance->appearAnimStart(false);
            }
            else
            {
                lv_obj_add_state(obj, LV_STATE_USER_1);
                instance->appearAnimStart(true);
            }
            lv_label_set_text_fmt(instance->ui.topCont.lockLabel, "%s", state ? "解锁" : "锁定");
        }
        else if (obj == instance->ui.topCont.listBtn)
        {
            instance->_isPlaying = false;
            lv_obj_set_style_bg_img_src(instance->ui.btnCont.btn, LV_SYMBOL_PLAY, 0);

            lv_obj_set_style_bg_img_opa(instance->ui.cont, LV_OPA_COVER, 0);
            lv_obj_clear_flag(instance->ui.listCont.cont, LV_OBJ_FLAG_HIDDEN);

            lv_disp_get_default()->driver->screen_transp = 0;
            lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
            lv_style_set_bg_opa(&style_scr_act, LV_OPA_COVER);
            lv_obj_report_style_change(&style_scr_act);

            lv_label_set_text_fmt(instance->ui.funcCont.speedLabel, "x%d", 1);

            if (instance->_opts.pauseCb != nullptr)
                instance->_opts.pauseCb(); // 暂停播放
        }
        else if (obj == instance->ui.topCont.cancelBtn)
        {
            instance->_opts.exitCb();
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
        if (obj == instance->ui.btnCont.slider)
        {
            int cur = lv_slider_get_value(obj);
            int total = lv_slider_get_max_value(obj);
            lv_label_set_text_fmt(instance->ui.btnCont.timeLabel, "%d:%d/%d:%d", cur / 60, cur % 60, total / 60, total % 60);
        }
        // lv_indev_wait_release(lv_indev_get_act());
        if (obj == instance->ui.sliderCont.brightnessSlider)
        {
            char cmd[512];
            int value = lv_slider_get_value(obj);

            value = value >= 10 ? value : 10;

            memset(cmd, sizeof(cmd), 0);
            snprintf(cmd, sizeof(cmd) - 1, "cd /sys/kernel/debug/dispdbg; echo lcd0 > name; echo setbl > command; echo %d > param; echo 1 > start\n", value);
            system(cmd);
            printf("[Brightness] setValueCmd: %s", cmd);
        }
    }
    if (code == LV_EVENT_RELEASED)
    {
        if (obj == instance->ui.btnCont.slider)
        {
            int cur = lv_slider_get_value(obj);

            if (instance->_opts.setCurCb != nullptr)
                instance->_opts.setCurCb(cur);
        }

        if (obj == instance->ui.sliderCont.volumeSlider)
        {
            int cur = lv_slider_get_value(obj);
            if (instance->_opts.setVolumeCb != nullptr)
            {
                instance->_opts.setVolumeCb(cur);
            }
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
            lv_label_set_text_fmt(instance->ui.topCont.videoNameLabel, "%s", videoName);
        }
    }
}

void View::onEvent(lv_event_t *event)
{
    View *instance = (View *)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t *obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (obj == lv_scr_act())
    {
        if (code == LV_EVENT_GESTURE)
        {
            switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
            {
            case LV_DIR_LEFT:
                printf("[View] LV_DIR_LEFT!\n");
                if (!instance->ui.isSliderContCollapsed)
                    instance->appearAnimSlider(true);

                break;
            case LV_DIR_RIGHT:
                printf("[View] LV_DIR_RIGHT!\n");
                if (instance->ui.isSliderContCollapsed)
                    instance->appearAnimSlider(false);

                break;
            case LV_DIR_TOP:
                printf("[View] LV_DIR_TOP!\n");
                if (!instance->ui.isTopContCollapsed)
                    instance->appearAnimTop(true);

                break;
            case LV_DIR_BOTTOM:
                printf("[View] LV_DIR_BOTTOM!\n");
                if (instance->ui.isTopContCollapsed)
                    instance->appearAnimTop(false);

                // instance->_opts.exitCb();
                break;

            default:
                break;
            }
        }

        if (code == LV_EVENT_SHORT_CLICKED)
        {
            // TODO ...
            // static bool state = false;

            // if (state)
            //     instance->appearAnimTop(false);
            // else
            //     instance->appearAnimTop(true);

            // state = !state;
        }
    }
}