#pragma once

#include <mfu-common/clientdata.h>

class QString;

namespace DB {
ClientData getClient(const QString & macAddress);
bool createUnregisteredClient(const QString & macAddress);
}
