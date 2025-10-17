#pragma once

#include <dirent.h>
#include "common_inc.h"
#include "View.h"
#include "MediaPlayer.h"
#include "../libs/lvgl/lvgl.h"

namespace Page
{
    class Model
    {
    public:
    private:
        MediaPlayer *_mp;                         // 媒体播放器对象指针
        std::mutex _mutex;                        // 互斥量
        std::thread _threadLvgl;                  // lvgl线程
        std::thread _threadDataProc;              // 数据处理线程
        std::condition_variable _cv;              // 条件变量
        std::atomic<bool> _threadExitFlag{false}; // 线程退出标志位
        // bool _threadExitFlag;
        View _view;                               // View的实例
        lv_timer_t *_timer;                       // LVGL软定时器

        struct
        {
            uint32_t lastContShowTime;
        } _priv; // 零零散散的变量

    private:
        /**
         * @brief LVGL处理线程
         */
        void threadLvglHandler(void);
        /**
         * @brief data处理线程
         */
        void threadDataProcHandler(void);

        void update(void);
        static void onTimerUpdate(lv_timer_t *timer);

        int searchVideo(std::string path);

        // funtion for View
        bool getState(void);
        int getVolume(void);
        void pause(void);
        void play(const char *name = NULL);
        void setCur(int cur);
        int getCur(void);
        int getDuration(void);
        void setVolume(int volume);
        void setSpeed(int speed);
        void setRotate(int angle);
        void setFullScreen(bool isFullScreen);
        void setLoop(bool isLoop);

    public:
        Model(std::function<void(void)> exitCb);
        ~Model();

        // 获取当前可执行文件所在路径
        static std::string getExeDirectory(void);
    };
}