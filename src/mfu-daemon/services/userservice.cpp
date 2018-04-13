#include "userservice.h"

#include <database/dbusers.h>
#include <server/rpcserver.h>

#include <QJsonValue>
#include <QWebSocket>

UserService::UserService(QObject *parent)
    : QObject(parent)
{
}

void UserService::registerMethods(RPCServer * server)
{
	// connect methods
	server->addCallback("bool UserService::login(QString,QString)", [=] (const QJsonValue & request, QWebSocket * sendingSocket) {
		const QString username     = request.toObject().value("login").toString();
		const QString passwordHash = request.toObject().value("password").toString();
		return QJsonValue(login(sendingSocket, username, passwordHash));
	}, "Method to login users" );
}

bool UserService::userHasPermission(QWebSocket * sendingSocket, const User::Permission & permission)
{
	if (!loggedInUser_.contains(sendingSocket)) {
		qWarning() << "User not logged in";
		return false;
	}

	const User u = loggedInUser_.value(sendingSocket);
	const bool hasPermission = u.permissions.contains(permission);
	if (!hasPermission) qWarning() << "User has missing permission:" << permission;
	return hasPermission;
}

QList<QWebSocket *> UserService::getLoggedInAdministratorSockets()
{
	QList<QWebSocket *> retval;
	QHashIterator<QWebSocket *, User> it(loggedInUser_);
	while(it.hasNext()) {
		it.next();

		if (it.value().permissions.contains(User::Permission::Admin)) {
			retval << it.key();
		}
	}
	return retval;
}

bool UserService::login(QWebSocket * sendingSocket, const QString & login, const QString & passwordHash)
{
#ifndef QT_NO_DEBUG
	if (login == "test") {
		User fakeUser;
		fakeUser.userId = -2;
		fakeUser.username = "test.user";
		fakeUser.permissions << User::Permission::Admin;
		loggedInUser_.insert(sendingSocket, fakeUser);
		return true;
	}
#endif
	const bool canLogin = DB::loginUser(login, passwordHash);
	if (!canLogin) {
		return false;
	}

	User user = DB::getUser(login);
	loggedInUser_.insert(sendingSocket, user);
	return true;
}
