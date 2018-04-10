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
	const bool canLogin = DB::loginUser(login, passwordHash);
	if (!canLogin) {
		return false;
	}

	User user = DB::getUser(login);
	loggedInUser_.insert(sendingSocket, user);
	return true;
}
