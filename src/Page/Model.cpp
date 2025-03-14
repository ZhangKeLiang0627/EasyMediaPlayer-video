#include "Model.h"

#define VIDEO_DIR "/mnt/UDISK/"

using namespace Page;

/* 支持的视频文件格式 */
static const char *fileType[] = {".avi", ".mkv", ".flv", ".ts", ".mp4", ".webm", "asf", "mpg", ".mpeg", ".mov", ".vob", ".3gp", ".wmv", ".pmp"};

/**
 * @brief Model构造函数
 *
 * @param exitCb
 * @param mutex
 */
Model::Model(std::function<void(void)> exitCb, pthread_mutex_t &mutex)
{
    _threadExitFlag = false;
    _mutex = &mutex;

    // 设置UI回调函数
    Operations uiOpts = {0};

    uiOpts.exitCb = exitCb;
    uiOpts.getStateCb = std::bind(&Model::getState, this);
    uiOpts.pauseCb = std::bind(&Model::pause, this);
    uiOpts.playCb = std::bind(&Model::play, this, std::placeholders::_1);
    uiOpts.setCurCb = std::bind(&Model::setCur, this, std::placeholders::_1);
    uiOpts.getCurCb = std::bind(&Model::getCur, this);
    uiOpts.setVolumeCb = std::bind(&Model::setVolume, this, std::placeholders::_1);
    uiOpts.getVolumeCb = std::bind(&Model::getVolume, this);
    uiOpts.getDurationCb = std::bind(&Model::getDuration, this);

    _view.create(uiOpts);

    // 这里设置一个1000ms的定时器，软定时器，用于在onTimerUpdate里update
    _timer = lv_timer_create(onTimerUpdate, 1000, this);
    // 创建执行线程，传递this指针
    pthread_create(&_pthread, NULL, threadProcHandler, this); 
}

Model::~Model()
{
    _threadExitFlag = true;

    lv_timer_del(_timer);

    _view.release();
}

/**
 * @brief 定时器更新函数
 *
 */
void Model::onTimerUpdate(lv_timer_t *timer)
{
    Model *instance = (Model *)timer->user_data;

    instance->update();
}

/**
 * @brief 更新UI等事务
 *
 */
void Model::update(void)
{
    // 更新进度条
    lv_slider_set_value(_view.ui.sliderCont.slider, getCur() / 1000, LV_ANIM_OFF);
    lv_slider_set_range(_view.ui.sliderCont.slider, 0, getDuration() / 1000);
}

/**
 *@brief 搜索某个目录下的视频文件,支持多种格式
 *@param path 目录路径
 *@return 搜索到的视频个数
 */
int Model::searchVideo(std::string path)
{
    int count = 0;
    bool legalVideo = false;
    std::string filePath;

    struct dirent *ent;
    DIR *dir = opendir(path.c_str());

    for (int i = 0;; i++)
    {
        // readdir函数用于读取目录中的下一个条目
        ent = readdir(dir);
        if (ent == NULL)
            break;

        if (ent->d_type == DT_REG)
        {
            // strrchr函数用于在ent->d_name中从后往前查找字符"."，返回指向该字符的指针
            // 若未找到，返回NULL
            const char *pfile = strrchr(ent->d_name, '.');
            if (pfile != NULL)
            {
                filePath = path + std::string(ent->d_name);

                for (int j = 0; j < sizeof(fileType) / sizeof(fileType[0]); j++)
                {
                    // strcasecmp函数用于比较两个字符串，不区分大小写
                    if (strcasecmp(pfile, fileType[j]) == 0)
                    {
                        printf("[Model] %s file\n", fileType[j]);
                        legalVideo = true;
                        break;
                    }
                }
            }
        }
        if (legalVideo == true)
        {
            legalVideo = false;
            _view.addVideoList(ent->d_name, nullptr);
            count++;
        }
    }

    closedir(dir);

    return count;
}

/**
 * @brief 线程处理函数
 *
 * @return void*
 */
void *Model::threadProcHandler(void *arg)
{
    Model *model = static_cast<Model *>(arg); // 将arg转换为Model指针

    model->_mp = new MediaPlayer(); // 创建播放器
    // 直接播放某视频
    // std::string url = "/mnt/UDISK/video1.mp4";
    // model->_mp->SetNewVideo(url);

    // 手动添加视频至播放列表
    // model->_view.addVideoList("video1.mp4", nullptr);
    // model->_view.addVideoList("video2.mp4", nullptr);
    // model->_view.addVideoList("video3.mp4", nullptr);

    // 搜索并添加视频至播放列表
    model->searchVideo(VIDEO_DIR);

    while (!model->_threadExitFlag)
    {   
        // 获取当前视频的进度和总时长
        // int cur = model->_mp->GetCurrentPos();
        // int total = model->_mp->GetDuration();

        // pthread_mutex_lock(model->_mutex);
        // model->_view.setPlayProgress(cur / 1000, total / 1000);
        // pthread_mutex_unlock(model->_mutex);

        usleep(50000);
    }

    delete model->_mp;
}

/**
 * @brief UI获取视频播放状态回调函数
 */
bool Model::getState(void)
{
    bool state = false;

    if (_mp != nullptr)
        state = _mp->GetState();

    return state;
}

/**
 * @brief UI获取音量回调函数
 */
int Model::getVolume(void)
{
    int volume = 0;

    if (_mp != nullptr)
        volume = _mp->GetVolume();

    return volume;
}

/**
 * @brief UI暂停视频回调函数
 */
void Model::pause(void)
{
    if (_mp != nullptr)
        _mp->Pause();
}

/**
 * @brief UI播放视频回调函数
 */
void Model::play(const char *name)
{
    if (name == NULL)
    {
        _mp->Start(); // 继续播放
        return;
    }

    // 播放新的视频
    std::string url = VIDEO_DIR + std::string(name);
    _mp->SetNewVideo(url);
    _mp->Start();
}

/**
 * @brief UI设置播放时间点回调函数
 */
void Model::setCur(int cur)
{
    if (_mp != nullptr)
        _mp->SetCurrentPos(cur * 1000);
}

/**
 * @brief UI获取播放时间点回调函数
 */
int Model::getCur(void)
{
    if (_mp != nullptr)
        return _mp->GetCurrentPos(); // 返回单位为ms
    else
        return 0;
}

/**
 * @brief UI获取播放时间点回调函数
 */
int Model::getDuration(void)
{
    if (_mp != nullptr)
        return _mp->GetDuration(); // 返回单位为ms
    else
        return 3000; // 否则返回3000ms，不要为0，不然进度条会出现问题
}

/**
 * @brief UI设置音量回调函数
 */
void Model::setVolume(int volume)
{
    if (_mp != nullptr)
        _mp->SetVolume(volume);
}
