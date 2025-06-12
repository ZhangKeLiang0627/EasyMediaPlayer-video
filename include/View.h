#pragma once

#include "../libs/lvgl/lvgl.h"
#include "lv_obj_ext_func.h"
#include "lv_anim_timeline_wrapper.h"
#include <functional>

namespace Page
{
    using ExitCb = std::function<void(void)>;
    using GetPlayStateCb = std::function<bool(void)>;
    using PauseCb = std::function<void(void)>;
    using PlayCb = std::function<void(const char *)>;
    using SetCurCb = std::function<void(int)>;
    using GetCurCb = std::function<int(void)>;
    using GetDurationCb = std::function<int(void)>;
    using GetVolumeCb = std::function<int(void)>;
    using SetVolumeCb = std::function<void(int)>;
    using SetSpeedCb = std::function<void(int)>;
    using SetRotateCb = std::function<void(int)>;
    using SetFullScreenCb = std::function<void(bool)>;

    struct Operations
    {
        ExitCb exitCb;
        GetPlayStateCb getStateCb;       // 获取播放状态
        PauseCb pauseCb;                 // 当前视频暂停回调函数
        PlayCb playCb;                   // 视频播放回调函数
        SetCurCb setCurCb;               // 视频设置进度回调函数
        GetCurCb getCurCb;               // 获取视频进度回调函数
        GetDurationCb getDurationCb;     // 获取视频总长度回调函数
        GetVolumeCb getVolumeCb;         // 获取视频音量回调函数
        SetVolumeCb setVolumeCb;         // 设置视频音量回调函数
        SetSpeedCb setSpeedCb;           // 设置视频倍速回调函数
        SetRotateCb setRotateCb;         // 设置翻转屏幕回调函数
        SetFullScreenCb setFullScreenCb; // 设置视频是否全屏回调函数
    };

    class View
    {
    private:
        Operations _opts; // View回调函数集
        bool _isPlaying = false;

    public:
        struct
        {
            lv_obj_t *cont;
            struct
            {
                lv_ft_info_t font16; // 自定义字体
                lv_ft_info_t font20; // 自定义字体
            } fontCont;
            struct
            {
                lv_obj_t *cont;
                lv_obj_t *btn;       // 播放键 / 暂停键
                lv_obj_t *slider;    // 进度条
                lv_obj_t *timeLabel; // 时间戳
            } btnCont;

            struct
            {
                lv_obj_t *cont;
                lv_obj_t *speedBtn;      // 倍速
                lv_obj_t *rotateBtn;     // 翻转屏幕
                lv_obj_t *loopBtn;       // 循环
                lv_obj_t *fullScreenBtn; // 视频全屏
                lv_obj_t *funcBtn;       // Function Button

                lv_obj_t *speedLabel;      // 倍速标签
                lv_obj_t *rotateLabel;     // 翻转屏幕标签
                lv_obj_t *fullScreenLabel; // 视频全屏标签
            } funcCont;
            struct
            {
                lv_obj_t *cont;
                lv_obj_t *volumeSlider;     // 音量条
                lv_obj_t *brightnessSlider; // 亮度条
            } sliderCont;
            struct
            {
                lv_obj_t *cont;
                lv_obj_t *cancelBtn; // to cancel this app
                lv_obj_t *lockBtn;   // to lock the screen
                lv_obj_t *listBtn;   // 播放列表

                lv_obj_t *videoNameLabel; // to show the video name which is playing
                lv_obj_t *lockLabel;      // to show the lock screen label
            } topCont;

            struct
            {
                lv_obj_t *cont;
                // lv_obj_t *list;
            } listCont;

            lv_anim_timeline_t *anim_timeline;
            lv_anim_timeline_t *anim_timelineClick;
            lv_anim_timeline_t *anim_timelineSlider;
            lv_anim_timeline_t *anim_timelineTop;

            bool isSliderContCollapsed = true;
            bool isTopContCollapsed = true;
            bool isBtnContCollapsed = false;
        } ui;

        void create(Operations &opts);
        void release(void);
        void appearAnimStart(bool reverse = false);
        void appearAnimSlider(bool reverse = false);
        void appearAnimClick(bool reverse = false);
        void appearAnimTop(bool reverse = false);
        void addVideoList(const char *name, const void *img_src);
        void setPlayProgress(int cur, int total);
        void setVolumeProgress(int cur, int total);
        void setBrightnessProgress(int cur, int total);

    private:
        void AttachEvent(lv_obj_t *obj);

        void contCreate(lv_obj_t *obj);
        void btnContCreate(lv_obj_t *obj);
        void sliderContCreate(lv_obj_t *obj);
        void listContCreate(lv_obj_t *obj);
        void funcContCreate(lv_obj_t *obj);
        void topContCreate(lv_obj_t *obj);
        void fontCreate(void);

        static void onEvent(lv_event_t *event);
        static void buttonEventHandler(lv_event_t *event);
        static void listBtnEventHandler(lv_event_t *event);
        static void sliderEventHandler(lv_event_t *event);

        lv_obj_t *roundRectCreate(lv_obj_t *par, lv_coord_t x_ofs, lv_coord_t y_ofs);
        lv_obj_t *btnCreate(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs, lv_coord_t y_ofs, lv_coord_t w = 50, lv_coord_t h = 50);
        lv_obj_t *sliderCreate(lv_obj_t *par, const void *img_src, lv_coord_t x_ofs = 0, lv_coord_t y_ofs = 0, int32_t min = 0, int32_t max = 255, int32_t val = 0);
        lv_obj_t *listCreate(const char *name, const void *img_src);
    };

}