#pragma once

#include <QAbstractListModel>

#include <mfu-common/clientdata.h>

class MFUAdmin;
class RPCClient;

class ClientsModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Roles {
		MacAddressRole = Qt::UserRole + 1,
		NameRole,
		LocationRole,
		StreamUrl,
		StreamPort,
		RegisteredRole,
		LoggedInRole,
		ActionRole
	};
	Q_ENUMS(Roles)

	ClientsModel(RPCClient * client);

	void clear();
	int rowCount(const QModelIndex & parent = QModelIndex()) const override;
	QVariant data(const QModelIndex & index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;
	virtual int columnCount(const QModelIndex & = QModelIndex()) const override;

	Q_INVOKABLE void updateClient(const QString & macAddress, const QString & name, const QString & location, const QString & streamUrl, int streamPort, bool registered);

private slots:
	void handleMessageFromServer(const QVariantHash & map);

private:
	void updateClient(const ClientData & clientData);
	void updateClient(const QString & macAddress);
	void resetAllClients(const QList<ClientData> & data);
	void handleCallbackResponse(const QString & signature, const QJsonValue & rawData);
	void handleNotificationResponse(const QString & notification, const QJsonValue & rawData);

private:
	RPCClient * rpcClient_;
	QMap<QString, ClientData> clients_;
};
