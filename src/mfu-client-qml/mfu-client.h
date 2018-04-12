#pragma once

#include <QObject>

class ClientData;
class RPCClient;

class MFUClient : public QObject
{
	Q_OBJECT
public:
	explicit MFUClient(RPCClient * client);

signals:
	void clientDataUpdate(const ClientData & cdr);
	void error(const QString & errorMessage);

private slots:
	void handleMessageFromServer(const QVariantHash & map);

private:
	void handleCallbackResponse(const QString & signature, const QJsonValue & rawData);
	void handleNotificationResponse(const QString & notification, const QJsonValue & rawData);

private:
	QString localMacAddress_;
	RPCClient * rpcClient_;
};
