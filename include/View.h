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
    using GetVolumeCb = std::function<int(void)>;
    using SetVolumeCb = std::function<void(int)>;

    struct Operations
    {
        ExitCb exitCb;
        GetPlayStateCb getStateCb; // 获取播放状态
        PauseCb pauseCb;           // 当前视频暂停回调函数
        PlayCb playCb;             // 视频播放回调函数
        SetCurCb setCurCb;         // 视频设置进度回调函数
        GetVolumeCb getVolumeCb;   // 获取视频音量回调函数
        SetVolumeCb setVolumeCb;   // 设置视频音量回调函数
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
                lv_obj_t *cont;
                lv_obj_t *btn;
            } btnCont;

            lv_anim_timeline_t *anim_timeline;
            lv_anim_timeline_t *anim_timelineClick;
        } ui;

        void create(Operations &opts);
        void release(void);
        void appearAnimStart(bool reverse = false);
        void appearAnimClick(bool reverse = false);
        void addVideoList(const char *name);
        void setPlayProgress(int cur, int total);

    private:
        void AttachEvent(lv_obj_t *obj);

        static void onEvent(lv_event_t *event);
        static void buttonEventHandler(lv_event_t *event);

        lv_obj_t *roundRectCreate(lv_obj_t *par, lv_coord_t x_ofs, lv_coord_t y_ofs);
        lv_obj_t *btnCreate(lv_obj_t *par, const void *img_src, lv_coord_t y_ofs);
    };

}