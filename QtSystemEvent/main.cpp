#include <iostream>
#include <QApplication>
#include <QDebug>

#include "SystemEventType.h"
#include "SystemEventHandler.h"
#include "SystemEventManager.h"

class SystemTest : public SystemEventHandler
{
public:
    SystemTest()
    {
        SystemEventMgr->addSytemEvent(SystemEventType::SYS_ENABLE, this);
        SystemEventMgr->addSytemEvent(SystemEventType::SYS_DISENABLE, this);

        installEventFilter(this);
    }

protected:
    void
    customEvent(QEvent *event) override
    {
        if (event->type() == SystemEventType::SYS_ENABLE)
        {
            auto p = static_cast<SystemEvent *>(event);
            qDebug() << "SystemTest SystemEventType::SYS_ENABLE" << p->mData;
        }
        if (event->type() == SystemEventType::SYS_DISENABLE)
        {
            auto p = static_cast<SystemEvent *>(event);
            qDebug() << "SystemTest SystemEventType::SYS_DISENABLE" << p->mData;
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SystemTest tTest;
    SystemTest tTest2;

    SystemEventMgr->sendEvent(SystemEvent(SystemEventType::SYS_ENABLE, QStringList{"12", "234"}));
    SystemEventMgr->postEvent(SystemEvent(SystemEventType::SYS_DISENABLE, 1));

    return app.exec();
}
