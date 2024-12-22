#pragma once

#include "defines.h"

#include <functional>

class CRASHREPORTER_EXPORT CrashReporter
{
public:
    /// @brief 注册异常回调, 这个函数只对当前线程生效
    /// @brief 如果新建了一个线程, 最好再线程处理开始时, 调用一次此函数
    /// @param tUserCall 当触发异常回调时, 要触发用户回调, 一般用于显示可交互的UI
    /// @note 注意, 用户回调触发时, 当前线程不变
    /// @example CrashReporter::RegUnhandleException(doSometing());
    static void RegUnhandleException(std::function<void()> tUserCall = nullptr);
};