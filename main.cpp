#include "./inc/common_inc.h"
#include "Model.h"

pthread_mutex_t lv_mutex;
static pthread_t threadLvgl;
static Page::Model *model;

static void exitCallback(void);
void *threadLvglHandler(void *);

int main(int argc, char *argv[])
{
    // log init
    logger_init("/mnt/UDISK/logs/", "eMP.log", false, 1024 * 1024 * 1, 10);

    log_info("[Sys] eMP_mainPage begin!");

    // 清除fb0
    system("dd if=/dev/zero of=/dev/fb0");
    // 打开音频通路并设置音量
    system("amixer sset Headphone unmute");
    system("amixer sset \"Headphone volume\" 7");

    // TODO：解决重复挂载的问题
    system("mount /dev/sda /mnt/exUDISK/");
    system("mount /dev/sda1 /mnt/exUDISK/");

    // Init HAL
    HAL::Init();

    // model初始化
    model = new Page::Model(exitCallback, lv_mutex);

    /* Handle LitlevGL tasks (tickless mode) */
    pthread_create(&threadLvgl, NULL, threadLvglHandler, NULL);

    while (1)
    {
        // ...
        usleep(50000);
    }

    return 0;
}

/**
 * @brief LVGL处理线程
 *
 * @return void*
 */
void *threadLvglHandler(void *)
{
    HAL::LVGL_Proc();
}

/**
 * @brief 退出回调函数
 */
static void exitCallback(void)
{
    delete model;

    exit(0);
}