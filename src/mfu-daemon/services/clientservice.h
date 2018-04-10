#pragma once

#include <mfu-common/clientdataresponse.h>

#include <QHash>
#include <QObject>
#include <QString>

class QWebSocket;
class RPCServer;
class UserService;

class ClientService : public QObject
{
	Q_OBJECT
public:
	ClientService(UserService * userService, QObject *parent = nullptr);

	void registerMethods(RPCServer * server);

private: // rpc methods
	ClientData loginClient(QWebSocket * sendingSocket, const QString & macAddress);

signals: // signals send to clients
	void updatedClientData(const ClientDataResponse & response);
	void clientLoggedIn(const QString & macAddress, bool loggedIn);

private:
	QWebSocket * getClientSocketForMac(const QString & macAddress);

private:
	UserService * userService_;
	QHash<QWebSocket *, ClientData> connectedClients_;
};
