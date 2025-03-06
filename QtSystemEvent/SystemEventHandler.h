#pragma once

#include "SystemEventType.h"

#include <QObject>
#include <QEvent>
#include <QVariant>
#include <QDebug>

/// @brief 自定义事件，带参数
struct SystemEvent : public QEvent
{
    SystemEvent(QEvent::Type _type, QVariant _des)
        : QEvent(_type), mData(_des)
    {
    }

    QVariant mData;
};

/// @brief 事件处理-中间层
class SystemEventHandler : public QObject
{
public:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == SystemEventType::SYS_ENABLE)
        {
            qDebug() << "SystemEventHandler event" << "SYS_ENABLE" << "eventFilter";
        }

        return QObject::eventFilter(watched, event);
    }

    bool event(QEvent *event) override
    {
        if (event->type() == SystemEventType::SYS_ENABLE)
        {
            qDebug() << "SystemEventHandler event" << "SYS_ENABLE" << "event";
        }

        return QObject::event(event);
    }
};