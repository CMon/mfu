#include "dbusers.h"

#include <database/database.h>

#include <QSqlQuery>
#include <QVariant>

bool DB::loginUser(const QString & login, const QString & passwordHash)
{
	TRANSACTION;

	QSqlQuery query(t.db);
	query.prepare("SELECT passwordHash FROM users WHERE login = :login");
	query.bindValue(":login", login);
	execQuery(query);
	bool loggedIn = false;
	if (query.next()) {
		const QString dbPass = query.value(0).toString();
		loggedIn = passwordHash == dbPass;
	}
	t.commit();
	return loggedIn;
}

User DB::getUser(const QString & login)
{
	TRANSACTION;

	QSqlQuery query(t.db);
	query.prepare("SELECT userId, login FROM users WHERE login = :login");
	query.bindValue(":login", login);
	execQuery(query);
	User u;
	if (query.next()) {
		u.userId = query.value(0).toInt();
		u.username = query.value(1).toString();

		QSqlQuery permissionQuery(t.db);
		permissionQuery.prepare("SELECT permission FROM permissions WHERE userId = :userId");
		permissionQuery.bindValue(":userId", u.userId);
		execQuery(permissionQuery);
		while (permissionQuery.next()) {
			u.permissions << User::Permission(permissionQuery.value(0).toInt());
		}
	}
	t.commit();
	return u;
}

bool DB::addUser(const QString & login, const QString & passwordHash, const QSet<User::Permission> & permissions)
{
	User u = getUser(login);
	if (!u.username.isEmpty()) return false;

	TRANSACTION;

	QSqlQuery queryUser(t.db);
	queryUser.prepare("INSERT INTO users "
	                  "(login, passwordHash) "
	                  "VALUES "
	                  "(:login, :passwordHash)");
	queryUser.bindValue(":login", login);
	queryUser.bindValue(":passwordHash", passwordHash);
	if (!execQuery(queryUser)) return false;

	u = getUser(login);
	if (u.userId == -1) return false;

	QSqlQuery queryPermission(t.db);
	queryPermission.prepare("INSERT INTO permissions "
	                        "(userId, permission) "
	                        "VALUES "
	                        "(:userId, :permission)");
	queryPermission.bindValue(":userId", u.userId);
	for (const User::Permission & perm : permissions) {
		queryPermission.bindValue(":permission", int(perm));
		if (!execQuery(queryPermission)) return false;
	}

	return t.commit();
}
