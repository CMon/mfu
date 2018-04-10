#pragma once

#include <mfu-common/clientdataresponse.h>
#include <mfu-daemon/user.h>

#include <QHash>
#include <QObject>

class RPCServer;
class QWebSocket;

class UserService : public QObject
{
	Q_OBJECT
public:
	explicit UserService(QObject *parent = nullptr);

	void registerMethods(RPCServer * server);

	QList<QWebSocket *> getLoggedInAdministratorSockets();

public: // rpc methods
	bool login(QWebSocket * sendingSocket, const QString & username, const QString & passwordHash);

private:
	QHash<QWebSocket *, User> loggedInUser_;
};
