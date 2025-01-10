#pragma once

#include <QtCore/QEvent>

static int s_SystemEventIndex = 1;
#define EventBuilder(name) const QEvent::Type SYS_##name = static_cast<QEvent::Type>(QEvent::registerEventType());

namespace SystemEventType
{
EventBuilder(ENABLE)
EventBuilder(DISENABLE)

EventBuilder(TEST)
}