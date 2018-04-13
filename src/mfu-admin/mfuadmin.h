#pragma once

#include <QObject>
#include <QTimer>

class RPCClient;

class MFUAdmin : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(bool loggedIn READ loggedIn WRITE setLoggedIn NOTIFY loggedInChanged)

public:
	explicit MFUAdmin(RPCClient * rpcClient, QObject *parent = nullptr);
	~MFUAdmin() override = default;

	Q_INVOKABLE void connectToServer(const QString & host, const int port, const QString & username, const QString & password);

	QString status() const;
	bool loggedIn() const;

signals:
	void statusChanged(QString status);
	void loggedInChanged(bool loggedIn);

public slots:
	void setStatus(const QString& status);
	void setLoggedIn(bool loggedIn);

private slots:
	void handleMessageFromServer(const QVariantHash & map);

private:
	void handleCallbackResponse(const QString & signature, const QJsonValue & rawData);
	void handleNotificationResponse(const QString & notification, const QJsonValue & rawData);

private:
	RPCClient * rpcClient_;
	QString status_;
	bool loggedIn_;
	QTimer statusResetTimer_;
};
