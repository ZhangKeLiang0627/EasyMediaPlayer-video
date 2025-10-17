#include "Model.h"

#define VIDEO_DIR "/mnt/UDISK/video/"
#define SD_VIDEO_DIR "/mnt/exUDISK/video/"

using namespace Page;

/* 支持的视频文件格式 */
static const char *fileType[] = {".avi", ".mkv", ".flv", ".ts", ".mp4", ".webm", "asf", "mpg", ".mpeg", ".mov", ".vob", ".3gp", ".wmv", ".pmp"};

/**
 * @brief Model构造函数
 *
 * @param exitCb
 * @param mutex
 */
Model::Model(std::function<void(void)> exitCb)
{
    _threadExitFlag = false;

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
    uiOpts.setSpeedCb = std::bind(&Model::setSpeed, this, std::placeholders::_1);
    uiOpts.setRotateCb = std::bind(&Model::setRotate, this, std::placeholders::_1);
    uiOpts.setFullScreenCb = std::bind(&Model::setFullScreen, this, std::placeholders::_1);
    uiOpts.setLoopCb = std::bind(&Model::setLoop, this, std::placeholders::_1);

    _view.create(uiOpts);

    // 这里设置一个1000ms的定时器，软定时器，用于在onTimerUpdate里update
    _timer = lv_timer_create(onTimerUpdate, 1000, this);

    // 创建lvgl处理线程，传递this指针
    _threadLvgl = std::thread([](Model *pThis)
                              { pThis->threadLvglHandler(); }, this);

    // 创建data处理线程，传递this指针
    _threadDataProc = std::thread([](Model *pThis)
                                  { pThis->threadDataProcHandler(); }, this);

    // _cv.notify_all(); 
}

Model::~Model()
{
    _threadExitFlag = true;

    // _cv.notify_all(); // 唤醒休眠中的线程，使其立即检查退出标志

    // 等待线程退出，回收资源
    if (_threadLvgl.joinable())
    {
        log_info("[Model] joining _threadLvgl...");
        _threadLvgl.join();
        log_info("[Model] _threadLvgl joined");
    }

    if (_threadDataProc.joinable())
    {
        log_info("[Model] joining _threadDataProc...");
        _threadDataProc.join();
        log_info("[Model] _threadDataProc joined");
    }

    lv_timer_del(_timer);
    _view.release();

    log_info("[Model] ~Model exit!");
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
    if (_mp != nullptr)
    {
        // 更新进度条
        _view.setPlayProgress(getCur() / 1000.0f, getDuration() / 1000.0f);
        // 更新音量
        _view.setVolumeProgress(getVolume(), 40);
        // // TODO：更新亮度
        // _view.setBrightnessProgress(50, 100);
    }
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
    if (dir == NULL)
    {
        // 打开目录失败，直接返回0
        return 0;
    }

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
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _view.addVideoList(ent->d_name, nullptr);
            }
            count++;
        }
    }

    closedir(dir);

    return count;
}

/**
 * @brief LVGL处理线程
 */
void Model::threadLvglHandler(void)
{
    while (!_threadExitFlag)
    {
        log_info("[Model] threadLvglHandler geting lock...");
        std::unique_lock<std::mutex> lock(_mutex);
        log_info("[Model] threadLvglHandler get lock!");
        
        uint32_t ms = lv_task_handler();

        log_info("[Model] lv_task_handler return, ms: %d", ms);

        lock.unlock();

        log_info("[Model] threadLvglHandler unlock!");

        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    log_info("[Model] threadLvglHandler exit!");
}

/**
 * @brief data处理线程
 */
void Model::threadDataProcHandler(void)
{
    _mp = new MediaPlayer(); // 创建播放器
    // 直接播放某视频
    // std::string url = "/mnt/UDISK/video1.mp4";
    // _mp->SetNewVideo(url);
    usleep(50000);

    // 手动添加视频至播放列表
    // _view.addVideoList("video1.mp4", nullptr);
    // _view.addVideoList("video2.mp4", nullptr);
    // _view.addVideoList("video3.mp4", nullptr);

    // 搜索并添加视频至播放列表
    searchVideo(VIDEO_DIR);
    searchVideo(SD_VIDEO_DIR);

    while (!_threadExitFlag)
    {

        usleep(50000);
    }

    delete _mp;
    log_info("[Model] threadDataProcHandler exit!");
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
        if (_mp != nullptr)
            _mp->Start(); // 继续播放
        return;
    }

    std::string videoName(name);
    std::string url;

    // TODO：急需一个更加优雅的方法，来分类与加载视频源

    // 检查VIDEO_DIR
    DIR *dir = opendir(VIDEO_DIR);
    if (dir != NULL)
    {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG && videoName == ent->d_name)
            {
                url = std::string(VIDEO_DIR) + videoName;
                break;
            }
        }
        closedir(dir);
    }

    // 如果没找到，再检查SD_VIDEO_DIR
    if (url.empty())
    {
        dir = opendir(SD_VIDEO_DIR);
        if (dir != NULL)
        {
            struct dirent *ent;
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_type == DT_REG && videoName == ent->d_name)
                {
                    url = std::string(SD_VIDEO_DIR) + videoName;
                    break;
                }
            }
            closedir(dir);
        }
    }

    if (!url.empty() && _mp != nullptr)
    {
        _mp->SetNewVideo(url);
        _mp->Start();
    }
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

/**
 * @brief UI设置倍速回调函数
 */
void Model::setSpeed(int speed)
{
    if (_mp != nullptr)
        _mp->SetSpeed((TplayerPlaySpeedType)speed);
}

/**
 * @brief UI设置翻转屏幕回调函数
 */
void Model::setRotate(int angle)
{
    if (_mp != nullptr)
        _mp->SetRotate((TplayerVideoRotateType)angle);
}

/**
 * @brief UI设置视频是否全屏回调函数
 */
void Model::setFullScreen(bool isFullScreen)
{
    if (_mp != nullptr)
        _mp->SetFullScreen(isFullScreen);
}

/**
 * @brief UI设置视频是否循环播放函数
 */
void Model::setLoop(bool isLoop)
{
    if (_mp != nullptr)
        _mp->SetLoop(isLoop);
}

std::string Model::getExeDirectory(void)
{
    const size_t bufSize = 1024;
    char exePath[bufSize] = {0};

    const ssize_t len = readlink("/proc/self/exe", exePath, bufSize - 1);
    if (len == -1)
    {
        throw std::runtime_error("Failed to read executable path");
    }
    exePath[len] = '\0';

    char *lastSlash = std::strrchr(exePath, '/');
    if (!lastSlash)
    {
        throw std::runtime_error("Invalid executable path format");
    }
    *lastSlash = '\0';

    return std::string(exePath) + '/';
}