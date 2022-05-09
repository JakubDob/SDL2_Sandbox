#pragma once

#ifndef __ANDROID__
#define RENDER_DEBUG
#endif

#if defined __ANDROID__ && !defined(LOGI)
#include <android/log.h>
#define LOGI(...)                                                   \
    ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", \
                               __VA_ARGS__))
#else
#define LOGI(...) (__VA_ARGS__)
#endif
