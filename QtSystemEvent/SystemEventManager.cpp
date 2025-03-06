#include "SystemEventManager.h"
#include <QApplication>
#include <set>
#include <unordered_map>

class SystemEventManager::Impl
{
public:
    std::unordered_map<QEvent::Type, std::set<QObject *>> mSystemEventMap_;
};

SystemEventManager::SystemEventManager()
    : impl_(std::make_unique<Impl>())
{
}

SystemEventManager::~SystemEventManager()
{
}

void SystemEventManager::addSytemEvent(QEvent::Type event, QObject *obj)
{
    impl_->mSystemEventMap_[event].emplace(obj);
}

SystemEventManager *SystemEventManager::getInstance()
{
    static SystemEventManager ins;
    return &ins;
}

void SystemEventManager::postEvent(const SystemEvent &customEvent)
{
    auto it = impl_->mSystemEventMap_.find(customEvent.type());
    if (it != impl_->mSystemEventMap_.end())
    {
        for (const auto &obj : it->second)
        {
            qApp->postEvent(obj, new SystemEvent(customEvent));
        }
    }
}

void SystemEventManager::sendEvent(const SystemEvent &customEvent)
{
    auto it = impl_->mSystemEventMap_.find(customEvent.type());
    if (it != impl_->mSystemEventMap_.end())
    {
        for (const auto &obj : it->second)
        {
            qApp->sendEvent(obj, new SystemEvent(customEvent));
        }
    }
}