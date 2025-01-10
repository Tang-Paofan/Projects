#pragma once

#include "SystemEventType.h"

#include <QObject>
#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <memory>

struct SystemEvent
{
    SystemEvent(QEvent::Type _type, QVariant _des)
        : mEventType_(_type), mDes_(_des)
    {
    }

    QEvent::Type mEventType_;
    QVariant mDes_;
};

class SystemEventHandler : public QObject
{
public:
    bool event(QEvent *event) override
    {
        if (event->type() == SystemEventType::SYS_TEST)
        {
            qDebug() << "SystemEventHandler event" << "SYS_TEST";
        }
        if (event->type() == SystemEventType::SYS_ENABLE)
        {
            qDebug() << "SystemEventHandler event" << "SYS_ENABLE";
        }

        return QObject::event(event);
    }
};