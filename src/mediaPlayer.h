#ifndef _MEDIAPLAYER_H_
#define _MEDIAPLAYER_H_

#include <string>
#include <semaphore.h>
#include <tplayer.h>

#define LCD_WIDTH 480.0

class MediaPlayer
{
public:
private:
    TPlayer *mTPlayer;       // 播放器
    std::string _sourceUrl;  // 播放的视频路径
    sem_t _sem;              // 异步通知信号量
    bool _prepareFinishFlag; // 音视频是否准备标志位

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
    int SetDisplayArea(int x, int y, unsigned int width, unsigned int height);
    int SetRotate(TplayerVideoRotateType rotateDegree);
    MediaInfo *GetMediaInfo(void)
    {
        return TPlayerGetMediaInfo(mTPlayer);
    }

    bool SetNewVideo(std::string &url);
    bool IsPrepareFinish(void) const { return _prepareFinishFlag; }
};

#endif
