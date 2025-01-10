#pragma once

#include "SystemEventHandler.h"
#include <QEvent>

class SystemEventManager
{
public:
    static SystemEventManager* getInstance();

public:
    void addSytemEvent(QEvent::Type event, QObject* obj);

    void postEvent(const SystemEvent& customEvent);

    void sendEvent(const SystemEvent& customEvent);

private:
    explicit SystemEventManager();
    ~SystemEventManager();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

#define SystemEventMgr SystemEventManager::getInstance()