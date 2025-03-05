#pragma once

#include <string>
#include <functional>
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
        MediaPlayer *_mp;        // 媒体播放器对象指针
        pthread_t _pthread;      // 数据处理线程
        bool _threadExitFlag;    // 线程退出标志位
        pthread_mutex_t *_mutex; // 互斥量
        View _view;              // View的实例

    private:
        static void *threadProcHandler(void *);
        int searchVideo(std::string path);

        bool getState(void);
        int getVolume(void);
        void pause(void);
        void play(const char *name = NULL);
        void setCur(int cur);
        void setVolume(int volume);

    public:
        Model(std::function<void(void)> exitCb, pthread_mutex_t &mutex);
        ~Model();
    };
}