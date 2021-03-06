#include "clientservice.h"

#include <database/dbclients.h>
#include <mfu-daemon/services/userservice.h>
#include <server/rpcserver.h>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QWebSocket>

ClientService::ClientService(UserService * userService, QObject * parent)
    : QObject(parent)
    , userService_(userService)
{
}

void ClientService::registerMethods(RPCServer * server)
{
	// connect methods
	server->addCallback("ClientData ClientService::loginClient(QString)", [=] (const QJsonValue & request, QWebSocket * sendingSocket) {
		const QString macAddress = request.toObject().value("macAddress").toString();
		ClientDataResponse cdr(loginClient(sendingSocket, macAddress));
		return QJsonValue(cdr.toJson());
	}, "login the requesting client");

	server->addCallback("bool ClientService::updateClient(ClientData)", [=] (const QJsonValue & request, QWebSocket * sendingSocket) {
		ClientDataResponse cdr;
		cdr.fromJson(request.toObject());
		return QJsonValue(updateClient(sendingSocket, cdr.toClientData()));
	}, "update the given client, you have to be logged in as Administrator for this to work");

	server->addCallback("QList<ClientData> ClientService::getAllClients()", [=] (const QJsonValue &, QWebSocket * sendingSocket) {
		QList<ClientData> clients = getAllClients(sendingSocket);
		QJsonArray list;
		for(const ClientData & cd : clients) {
			list << ClientDataResponse(cd).toJson();
		}
		return QJsonValue(list);
	}, "returns a list of all given clients, you have to be logged in as Administrator for this to work");


	// connect notifications
	connect(this, &ClientService::updatedClientData, server, [server, this] (const ClientDataResponse & cdr) {
		QList<QWebSocket *> recipients;
		QWebSocket * clientSocket =  getClientSocketForMac(cdr.macAddress());
		if (clientSocket) recipients << clientSocket;
		recipients.append(userService_->getLoggedInAdministratorSockets());
		server->sendNotification("ClientService::updatedClientData(ClientData)", QJsonValue(cdr.toJson()), recipients);
	});

	connect(this, &ClientService::clientLoggedIn, server, [server, this] (const QString & macAddress, bool loggedIn) {
		const QList<QWebSocket *> recipients = userService_->getLoggedInAdministratorSockets();
		QJsonObject mac;
		mac.insert("macAddress", macAddress);
		mac.insert("loggedIn", loggedIn);
		server->sendNotification("ClientService::clientLoggedIn(QString,bool)", QJsonValue(mac), recipients);
	});
}

ClientData ClientService::loginClient(QWebSocket * sendingSocket, const QString & macAddress)
{
	if (connectedClients_.contains(sendingSocket)) return ClientData();

	ClientData cd = DB::getClient(macAddress);
	if (cd.macAddress().isEmpty()) {
		DB::createUnregisteredClient(macAddress);
		cd = DB::getClient(macAddress);
		emit updatedClientData(cd);
	}
	cd.setLoggedIn(true);
	connectedClients_[sendingSocket] = cd;

	emit clientLoggedIn(macAddress, cd.isLoggedIn());

	connect(sendingSocket, &QWebSocket::disconnected, this, [&]() {
		QWebSocket * send = qobject_cast<QWebSocket *>(sender());
		if (!send) return;

		const QString macAddress = connectedClients_.value(send).macAddress();
		emit clientLoggedIn(macAddress, false);
		connectedClients_.remove(send);
	});

	return cd;
}

QList<ClientData> ClientService::getAllClients(QWebSocket * sendingSocket) const
{
	if (!userService_->userHasPermission(sendingSocket, User::Permission::Admin)) return QList<ClientData>();

	QList<ClientData> clients = DB::getAllClients();

	for (ClientData & availableClient: clients) {
		for (const ClientData & connectedClient: connectedClients_) {
			if (connectedClient.macAddress() == availableClient.macAddress()) {
				availableClient.setLoggedIn(true);
				break;
			}
		}
	}

	return clients;
}

bool ClientService::updateClient(QWebSocket * sendingSocket, const ClientData & clientData)
{
	if (!userService_->userHasPermission(sendingSocket, User::Permission::Admin)) return false;

	const ClientData cd = DB::getClient(clientData.macAddress());
	if (cd.macAddress().isEmpty()) return false;
	if (cd == clientData) return true;

	const bool updated = DB::updateClient(clientData);

	if (updated) {
		emit updatedClientData(clientData);
	}
	return updated;
}

QWebSocket * ClientService::getClientSocketForMac(const QString & macAddress)
{
	QHashIterator<QWebSocket *, ClientData> it(connectedClients_);
	while(it.hasNext()) {
		it.next();

		if (it.value().macAddress() == macAddress) {
			return it.key();
		}
	}
	return nullptr;
}
