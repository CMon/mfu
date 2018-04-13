#pragma once

#include <mfu-common/clientdata.h>

class QString;

namespace DB {
ClientData getClient(const QString & macAddress);
QList<ClientData> getAllClients();
bool updateClient(const ClientData & clientData);
bool createUnregisteredClient(const QString & macAddress);
}
