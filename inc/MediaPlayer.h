#ifndef _MEDIAPLAYER_H_
#define _MEDIAPLAYER_H_

#ifdef __arm__
/* 32位ARM架构 / arm */

#include <string>
#include <semaphore.h>
#include <tplayer.h>

#define LCD_WIDTH 480.0

class MediaPlayer
{
public:
private:
    TPlayer *mTPlayer;            // 播放器
    std::string _sourceUrl;       // 播放的视频路径
    sem_t _sem;                   // 异步通知信号量
    bool _prepareFinishFlag;      // 音视频是否准备标志位
    bool _fullScreenFlag = false; // is video full of screen flag

    friend int CallbackForTPlayer(void *pUserData, int msg, int param0, void *param1);

public:
    MediaPlayer(std::string *url = nullptr);
    ~MediaPlayer(void);

    void Start(void);
    void Pause(void);
    void SetCurrentPos(int seekMs);
    int GetCurrentPos(void);
    int GetDuration(void);
    int GetVolume(void);
    void SetVolume(int volume);
    bool GetState(void);
    void SetLoop(bool isLoop);
    bool SetDisplayArea(int x, int y, unsigned int width, unsigned int height);
    bool SetRotate(TplayerVideoRotateType rotateDegree);
    bool SetSpeed(TplayerPlaySpeedType speed);
    bool SetFullScreen(bool isFullScreen);
    bool GetFullScreen(void);
    MediaInfo *GetMediaInfo(void)
    {
        return TPlayerGetMediaInfo(mTPlayer);
    }

    bool SetNewVideo(std::string &url);
    bool IsPrepareFinish(void) const { return _prepareFinishFlag; }
};

#else

// 提供空的实现

#include <string>
#include <cstdint>

typedef enum TplayerVideoRotateType
{
    TPLAYER_VIDEO_ROTATE_DEGREE_0 = 0,     /*do not rotate*/
    TPLAYER_VIDEO_ROTATE_DEGREE_90 = 90,   /*rotate 90 degree clockwise*/
    TPLAYER_VIDEO_ROTATE_DEGREE_180 = 180, /*rotate 180 degree clockwise*/
    TPLAYER_VIDEO_ROTATE_DEGREE_270 = 270, /*rotate 270 degree clockwise*/
} TplayerVideoRotateType;

typedef enum TplayerPlaySpeedType
{
    PLAY_SPEED_FAST_FORWARD_16 = 0,  /*fast forward 16 times*/
    PLAY_SPEED_FAST_FORWARD_8 = 1,   /*fast forward 8 times*/
    PLAY_SPEED_FAST_FORWARD_4 = 2,   /*fast forward 4 times*/
    PLAY_SPEED_FAST_FORWARD_2 = 3,   /*fast forward 2 times*/
    PLAY_SPEED_1 = 4,                /*normal play*/
    PLAY_SPEED_FAST_BACKWARD_2 = 5,  /*fast backward 2 times*/
    PLAY_SPEED_FAST_BACKWARD_4 = 6,  /*fast backward  4 times*/
    PLAY_SPEED_FAST_BACKWARD_8 = 7,  /*fast backward  8 times*/
    PLAY_SPEED_FAST_BACKWARD_16 = 8, /*fast backward  16 times*/
} TplayerPlaySpeedType;

struct MediaInfo
{
    int64_t nFileSize;
    int nDurationMs;

    // ...just an empty structure
};

#define LCD_WIDTH 480.0

class MediaPlayer
{
public:
    MediaPlayer(std::string *url = nullptr) {}
    ~MediaPlayer(void) {}

    void Start(void) {}
    void Pause(void) {}
    void SetCurrentPos(int seekMs) {}
    int GetCurrentPos(void) { return 0; }
    int GetDuration(void) { return 3000; }
    int GetVolume(void) { return 0; }
    void SetVolume(int volume) {}
    bool GetState(void) { return false; }
    void SetLoop(bool isLoop) {}
    bool SetDisplayArea(int x, int y, unsigned int width, unsigned int height) { return false; }
    bool SetRotate(TplayerVideoRotateType rotateDegree) { return false; }
    bool SetSpeed(TplayerPlaySpeedType speed) { return false; }
    bool SetFullScreen(bool isFullScreen) { return false; }
    bool GetFullScreen(void) { return _fullScreenFlag; }
    MediaInfo *GetMediaInfo(void)
    {
        static MediaInfo emptyInfo;
        return &emptyInfo;
    }
    bool SetNewVideo(std::string &url) { return false; }
    bool IsPrepareFinish(void) const { return _prepareFinishFlag; }

private:
    std::string _sourceUrl;
    bool _prepareFinishFlag;
    bool _fullScreenFlag = false;
};

#endif /* 32位ARM架构 / arm */

#endif /* _MEDIAPLAYER_H_ */
