#include "mfu-client.h"

#include <client/rpcclient.h>
#include <mfu-common/clientdataresponse.h>

#include <QJsonObject>
#include <QNetworkInterface>

MFUClient::MFUClient(RPCClient * client)
    : rpcClient_(client)
{
	QList<QNetworkInterface> foo = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface & ni: foo) {
		const QString mac = ni.hardwareAddress();
		if (mac.isEmpty()) continue;
		if (mac == "00:00:00:00:00:00") continue;

		localMacAddress_ = mac;
		break;
	}

	connect(rpcClient_, &RPCClient::connected, this, [this]() {
		qInfo() << "Connected to server";
		QJsonObject request;
		request.insert("macAddress", localMacAddress_);
		rpcClient_->messageToServer("ClientData ClientService::loginClient(QString)", QJsonValue(request));
	});

	connect(rpcClient_, &RPCClient::newMessageArrived, this, &MFUClient::handleMessageFromServer);
	connect(rpcClient_, &RPCClient::error, this, [this](const QString & err) {
		qWarning() << "RPCClient:" << err;
		emit error(err);
	});
}

void MFUClient::handleMessageFromServer(const QVariantHash & map)
{
	const QJsonObject message = QJsonObject::fromVariantHash(map);

	if (message.contains("signature")) {
		handleCallbackResponse(message.value("signature").toString(), message.value("data"));
	}

	if (message.contains("notification")) {
		handleNotificationResponse(message.value("notification").toString(), message.value("data"));
	}
}

void MFUClient::handleCallbackResponse(const QString & signature, const QJsonValue & rawData)
{
	if (signature == "ClientData ClientService::loginClient(QString)") {
		ClientDataResponse cdr;
		cdr.fromJson(rawData.toObject());
		const ClientData cd = cdr.toClientData();
		const bool registered = cd.isRegistered();
		if (!registered) {
			emit error(tr("Could not register at server"));
		} else {
			emit clientDataUpdate(cd);
		}
	}
}

void MFUClient::handleNotificationResponse(const QString & notification, const QJsonValue & rawData)
{
	if (notification == "ClientService::updatedClientData(ClientData)") {
		ClientDataResponse cdr;
		cdr.fromJson(rawData.toObject());
		emit clientDataUpdate(cdr.toClientData());
	}
}
