#include "mfuadmin.h"

#include <client/rpcclient.h>

#include <QJsonObject>
#include <QCryptographicHash>

MFUAdmin::MFUAdmin(RPCClient * rpcClient, QObject *parent)
    : QObject(parent)
    , rpcClient_(rpcClient)
    , loggedIn_(false)
    , statusResetTimer_(this)
{
	connect(rpcClient_, &RPCClient::connected, this, [&] () {
		setStatus(tr("Connected"));
	});

	connect(rpcClient_, &RPCClient::newMessageArrived, this, &MFUAdmin::handleMessageFromServer);
	connect(rpcClient_, &RPCClient::error, this, &MFUAdmin::setStatus);

	statusResetTimer_.setInterval(30 * 1000);
	statusResetTimer_.setSingleShot(true);
	connect(&statusResetTimer_, &QTimer::timeout, this, [&]() {
		setStatus(QString());
	});
}

void MFUAdmin::connectToServer(const QString & host, const int port, const QString & login, const QString & password)
{
	rpcClient_->setUrl(QString("ws://%1:%2").arg(host).arg(port));
	rpcClient_->connectToServer();
	if (!rpcClient_->waitForConnected()) {
		qCritical() << "Could not connect to host";
		setStatus(tr("Could not connect to host"));
		return;
	}
	QJsonObject loginRequest;
	loginRequest.insert("login", login);
	const QByteArray pass = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toBase64();
	loginRequest.insert("password", QString(pass));
	rpcClient_->messageToServer("bool UserService::login(QString,QString)", loginRequest);
}

QString MFUAdmin::status() const
{
	return status_;
}

bool MFUAdmin::loggedIn() const
{
	return loggedIn_;
}

void MFUAdmin::setStatus(const QString & status)
{
	if (status_ == status)
		return;

	statusResetTimer_.start();
	status_ = status;
	emit statusChanged(status_);
}

void MFUAdmin::setLoggedIn(bool loggedIn)
{
	if (loggedIn_ == loggedIn)
		return;

	loggedIn_ = loggedIn;
	emit loggedInChanged(loggedIn_);
}

void MFUAdmin::handleMessageFromServer(const QVariantHash & map)
{
	const QJsonObject message = QJsonObject::fromVariantHash(map);

	if (message.contains("signature")) {
		handleCallbackResponse(message.value("signature").toString(), message.value("data"));
	}

	if (message.contains("notification")) {
		handleNotificationResponse(message.value("notification").toString(), message.value("data"));
	}
}

void MFUAdmin::handleCallbackResponse(const QString & signature, const QJsonValue & rawData)
{
	if (signature == "bool UserService::login(QString,QString)") {
		const bool loggedIn = rawData.toBool();
		if (!loggedIn) {
			qWarning() << "Could not login";
			setStatus(tr("Login failed"));
		}
		setLoggedIn(loggedIn);
	}
}

void MFUAdmin::handleNotificationResponse(const QString & notification, const QJsonValue & rawData)
{
Q_UNUSED(notification)
Q_UNUSED(rawData)
#warning handle updates
}
