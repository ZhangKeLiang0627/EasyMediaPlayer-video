#include "MyPlayer.h"

int CallbackForTPlayer(void *pUserData, int msg,int param0, void *param1);

MediaPlayer::MediaPlayer(string *url)
{
    prepareOverFlag = false;
    sem_init(&sem, 0, 0);       //初始化信号量

    mTPlayer = TPlayerCreate(CEDARX_PLAYER);        //创建播放器
    TPlayerSetNotifyCallback(mTPlayer, CallbackForTPlayer , this);          //设置消息回调函数

    if(url != nullptr)
    {
        sourceUrl = *url;
        PlayNewVideo(sourceUrl);
    }
}

MediaPlayer::~MediaPlayer(void)
{
    TPlayerReset(mTPlayer);
    TPlayerDestroy(mTPlayer);

    sem_destroy(&sem);
}

/**
 * @brief 播放新的视频
 */
bool MediaPlayer::PlayNewVideo(string &url)
{
    prepareOverFlag = false;
    sourceUrl = url;

    TPlayerReset(mTPlayer);         //复位播放器
    TPlayerSetDataSource(mTPlayer, sourceUrl.c_str(), NULL);        //设置文件url
    TPlayerPrepareAsync(mTPlayer);      //解析头部信息

    struct timespec timeout = {.tv_sec = 3, .tv_nsec = 0};

    int ret = sem_timedwait(&sem, &timeout);     //等待信号量,超时时间3 s
    if(ret != 0)
    {
        printf("MediaPlayer prepare failed, url=%s\n", sourceUrl.c_str());
        return false;
    }

    TPlayerSetHoldLastPicture(mTPlayer, 0);     //不保留最后一帧
    TPlayerSetLooping(mTPlayer, true);          //循环播放
    prepareOverFlag = true;

    return true;
}

/**
 * @brief 开始播放
 */
void MediaPlayer::Start(void)
{
    if(prepareOverFlag !=false)
        TPlayerStart(mTPlayer);
}

/**
 * @brief 暂停播放
 */
void MediaPlayer::Pause(void)
{
    if(prepareOverFlag !=false)
        TPlayerPause(mTPlayer);
}

/**
 * @brief 设置播放时间点
 * @param seekMs 播放时间点 ms
 */
void MediaPlayer::SetCurrent(int seekMs)
{
    if(prepareOverFlag !=false)
        TPlayerSeekTo(mTPlayer, seekMs);
}

/**
 * @brief 获取当前播放时间点
 * @retval 当前播放的时间点 ms
 */
int MediaPlayer::GetCurrent(void)
{
    int ms = 0;

    if(prepareOverFlag !=false)
        TPlayerGetCurrentPosition(mTPlayer, &ms);

    return ms;
}

/**
 * @brief 获取播放总时长
 * @retval 播放总时长 ms
 */
int MediaPlayer::GetDuration(void)
{
    int ms = 3000;

    if(prepareOverFlag !=false)
        TPlayerGetDuration(mTPlayer, &ms);

    return ms;
}

/**
 * @brief 获取当前音量
 * @retval 当前音量大小
 */
int MediaPlayer::GetVolume(void)
{
    int volume = 0;

    if(prepareOverFlag !=false)
        volume = TPlayerGetVolume(mTPlayer);

    return volume;
}

/**
 * @brief 设置当前音量
 * @param volume 设置的音量大小
 */
void MediaPlayer::SetVolume(int volume)
{
    if(prepareOverFlag !=false)
        TPlayerSetVolume(mTPlayer,  volume);
}

/**
 * @brief 获取当前视频播放状态
 * @retval  false 视频未播放
 * @retval true 视频正在播放
 */
bool MediaPlayer::GetState(void)
{
    bool state = false;
    if(prepareOverFlag !=false)
        state = TPlayerIsPlaying(mTPlayer);

    return state;
}

/**
 * @brief  TPlayer消息回调函数
 * @param pUserData 用户数据，设置回调函数时传入
 * @param msg       消息类型
 * @param param0 参赛0
 * @param param1 参数1
 */
int CallbackForTPlayer(void *pUserData, int msg,int param0, void *param1)
{
    MediaPlayer *player = static_cast<MediaPlayer *>(pUserData);

    switch(msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
            sem_post(&player->sem);     //发送信号量
        break;

        default:;
    }

    return 0;
}
