#pragma once

#include <QtCore/QEvent>

#define EventBuilder(name) const QEvent::Type SYS_##name = static_cast<QEvent::Type>(QEvent::registerEventType())

namespace SystemEventType
{
    EventBuilder(ENABLE);
    EventBuilder(DISENABLE);
}