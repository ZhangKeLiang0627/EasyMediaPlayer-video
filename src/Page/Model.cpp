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
    uiOpts.getVolumeCb = std::bind(&Model::getVolume, this);
    uiOpts.pauseCb = std::bind(&Model::pause, this);
    uiOpts.playCb = std::bind(&Model::play, this, std::placeholders::_1);
    uiOpts.setCurCb = std::bind(&Model::setCur, this, std::placeholders::_1);
    uiOpts.setVolumeCb = std::bind(&Model::setVolume, this, std::placeholders::_1);

    _view.create(uiOpts);

    pthread_create(&_pthread, NULL, threadProcHandler, this); // 创建执行线程，传递this指针
}

Model::~Model()
{
    _threadExitFlag = true;

    _view.release();
}

/**
 *@brief 搜索某个目录下的视频文件,支持多种格式
 *@param path 目录路径
 *@return 搜索到的视频个数
 */
int Model::searchVideo(std::string path)
{
    int cnt = 0;
    int i;
    bool legalVideo = false;
    std::string filePath;

    struct dirent *ent;
    DIR *dir = opendir(path.c_str());

    for (i = 0;; i++)
    {
        ent = readdir(dir);
        if (ent == NULL)
            break;

        if (ent->d_type == DT_REG)
        {
            const char *pfile = strrchr(ent->d_name, '.');
            if (pfile != NULL)
            {
                filePath = path + std::string(ent->d_name);

                for (int j = 0; j < sizeof(fileType) / sizeof(fileType[0]); j++)
                {
                    if (strcasecmp(pfile, fileType[j]) == 0)
                    {
                        printf("%s file\n", fileType[j]);
                        legalVideo = true;
                        break;
                    }
                }
            }
        }
        if (legalVideo == true)
        {
            legalVideo = false;

            pthread_mutex_lock(_mutex);
            _view.addVideoList(ent->d_name);
            pthread_mutex_unlock(_mutex);

            cnt++;
        }

        usleep(50000);
    }

    closedir(dir);

    return cnt;
}

/**
 * @brief 线程处理函数
 *
 * @return void*
 */
void *Model::threadProcHandler(void *arg)
{
    Model *model = static_cast<Model *>(arg); // 将arg转换为Model指针
    usleep(50000);

    model->_mp = new MediaPlayer(); // 创建播放器
    std::string url = "/mnt/UDISK/video1.mp4";
    model->_mp->SetNewVideo(url);

    while (!model->_threadExitFlag)
    {
        int cur = model->_mp->GetCurrentPos();
        int total = model->_mp->GetDuration();

        pthread_mutex_lock(model->_mutex);
        model->_view.setPlayProgress(cur / 1000, total / 1000);
        pthread_mutex_unlock(model->_mutex);

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
 * @brief UI设置音量回调函数
 */
void Model::setVolume(int volume)
{
    if (_mp != nullptr)
        _mp->SetVolume(volume);
}
