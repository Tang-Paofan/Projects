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
        SystemEventMgr->addSytemEvent(SystemEventType::SYS_TEST, this);
    }

protected:
    void
    customEvent(QEvent *event) override
    {
        if (event->type() == SystemEventType::SYS_TEST)
        {
            qDebug() << "SystemTest SystemEventType::SYS_TEST";
        }
        if (event->type() == SystemEventType::SYS_ENABLE)
        {
            qDebug() << "SystemTest SystemEventType::SYS_ENABLE";
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SystemTest tTest;

    SystemEventMgr->postEvent(SystemEvent(SystemEventType::SYS_TEST, ""));
    SystemEventMgr->sendEvent(SystemEvent(SystemEventType::SYS_ENABLE, ""));

    return app.exec();
}
