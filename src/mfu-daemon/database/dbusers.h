#pragma once

#include <mfu-daemon/user.h>

class QString;

namespace DB {
bool loginUser(const QString & login, const QString & passwordHash);
User getUser(const QString & login);
bool addUser(const QString & login, const QString & passwordHash, const QSet<User::Permission> & permissions);
}
