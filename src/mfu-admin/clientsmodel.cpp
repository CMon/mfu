#include "clientsmodel.h"

#include <mfu-admin/mfuadmin.h>
#include <client/rpcclient.h>
#include <mfu-common/clientdataresponse.h>

#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>

ClientsModel::ClientsModel(RPCClient * client)
    : rpcClient_(client)
{
	connect(rpcClient_, &RPCClient::newMessageArrived, this, &ClientsModel::handleMessageFromServer);
}

int ClientsModel::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return clients_.size();
}

QVariant ClientsModel::data(const QModelIndex & index, int role) const
{
	if (index.row() < 0 || index.row() >= rowCount()) return QVariant();

	const QString key = clients_.keys().at(index.row());

	switch (Roles(role)) {
		case MacAddressRole: return clients_.value(key).macAddress();
		case NameRole:       return clients_.value(key).name();
		case LocationRole:   return clients_.value(key).location();
		case StreamUrl:      return clients_.value(key).streamUrl();
		case StreamPort:     return clients_.value(key).streamPort();
		case RegisteredRole: return clients_.value(key).isRegistered();
		case LoggedInRole:   return clients_.value(key).isLoggedIn();
		case ActionRole:     return index;
	}

	return QVariant();
}

QHash<int, QByteArray> ClientsModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("Roles"));
	for (int i = 0; i < e.keyCount(); ++i) {
		const int value = e.value(i);
		QByteArray name = e.key(i);
		name = name.replace("Role", "");
		name = name.left(1).toLower() + name.mid(1);
		roles[value] = name;
	}
	return roles;
}

int ClientsModel::columnCount(const QModelIndex &) const
{
	QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("Roles"));
	return e.keyCount();
}

void ClientsModel::updateClient(const QString & macAddress, const QString & name, const QString & location, const QString & streamUrl, int streamPort, bool registered)
{
	const ClientData cd(macAddress, name, location, streamUrl, streamPort, registered);
	rpcClient_->messageToServer("bool ClientService::updateClient(ClientData)", QJsonValue(ClientDataResponse(cd).toJson()));
}

void ClientsModel::handleMessageFromServer(const QVariantHash & map)
{
	const QJsonObject message = QJsonObject::fromVariantHash(map);

	if (message.contains("signature")) {
		handleCallbackResponse(message.value("signature").toString(), message.value("data"));
	}

	if (message.contains("notification")) {
		handleNotificationResponse(message.value("notification").toString(), message.value("data"));
	}
}

void ClientsModel::updateClient(const ClientData & clientData)
{
	clients_[clientData.macAddress()] = clientData;
	updateClient(clientData.macAddress());
}

void ClientsModel::updateClient(const QString & macAddress)
{
	const int row = clients_.keys().indexOf(macAddress);
	emit dataChanged(createIndex(row, 0), createIndex(row, columnCount()));
}

void ClientsModel::resetAllClients(const QList<ClientData> & data)
{
	beginResetModel();
	clients_.clear();
	for (const ClientData & cd: data) {
		clients_[cd.macAddress()] = cd;
	}
	endResetModel();
}

void ClientsModel::handleCallbackResponse(const QString & signature, const QJsonValue & rawData)
{
	if (signature == "bool UserService::login(QString,QString)") {
		const bool loggedIn = rawData.toBool();
		if (loggedIn) {
			rpcClient_->messageToServer("QList<ClientData> ClientService::getAllClients()", QJsonValue());
		} else {
			resetAllClients(QList<ClientData>());
		}
	} else if (signature == "QList<ClientData> ClientService::getAllClients()") {
		QJsonArray arr = rawData.toArray();
		QList<ClientData> list;
		for (const QJsonValue & val: arr) {
			ClientDataResponse cdr;
			cdr.fromJson(val.toObject());
			list << cdr.toClientData();
		}
		resetAllClients(list);
	}
}

void ClientsModel::handleNotificationResponse(const QString & notification, const QJsonValue & rawData)
{
	if (notification == "ClientService::updatedClientData(ClientData)") {
		ClientDataResponse cdr;
		cdr.fromJson(rawData.toObject());
		updateClient(cdr.toClientData());
	} else if (notification == "ClientService::clientLoggedIn(QString,bool)") {
		const QJsonObject args = rawData.toObject();
		const QString macAddress = args.value("macAddress").toString();
		const bool loggedIn = args.value("loggedIn").toBool();

		clients_[macAddress].setLoggedIn(loggedIn);
		updateClient(macAddress);
	}
}
