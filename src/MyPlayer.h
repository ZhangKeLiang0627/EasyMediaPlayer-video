#ifndef _MYPLAYER_H_
#define _MYPLAYER_H_

#include <string>
#include <semaphore.h>
#include <tplayer.h>

using namespace std;

class MediaPlayer;

class MediaPlayer
{
public:

private:
    string sourceUrl;       //播放的视频路径
    TPlayer *mTPlayer;      //播放器
    sem_t sem;      //异步通知信号量
    bool prepareOverFlag;

    friend int CallbackForTPlayer(void *pUserData, int msg,int param0, void *param1);
public:
    MediaPlayer(string *url = nullptr);
    ~MediaPlayer(void);

    void Start(void);
    void Pause(void);
    void SetCurrent(int seekMs);
    int GetCurrent(void);
    int GetDuration(void);
    int GetVolume(void);
    void SetVolume(int volume);
    bool GetState(void);

    bool PlayNewVideo(string &url);

    bool IsPrepareOver(void) const { return prepareOverFlag; }
};




#endif
