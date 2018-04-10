#pragma once

#include <common/jsonserialize.h>
#include <mfu-common/clientdata.h>

class ClientDataResponse : public JsonSerialize
{
	Q_OBJECT
	Q_PROPERTY(QString macAddress READ macAddress WRITE setMacAddress NOTIFY macAddressChanged)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
	Q_PROPERTY(QString streamUrl READ streamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)
	Q_PROPERTY(int streamPort READ streamPort WRITE setStreamPort NOTIFY streamPortChanged)
	Q_PROPERTY(bool isRegistered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)
	Q_PROPERTY(bool isLoggedIn READ isLoggedIn WRITE setLoggedIn NOTIFY loggedInChanged)

public:
	ClientDataResponse();
	ClientDataResponse(const ClientData & cd);
	ClientData toClientData() const;

	QString macAddress() const;
	QString name() const;
	QString location() const;
	QString streamUrl() const;
	int streamPort() const;
	bool isRegistered() const;
	bool isLoggedIn() const;

public slots:
	void setMacAddress(QString macAddress);
	void setName(QString name);
	void setLocation(QString location);
	void setStreamUrl(QString streamUrl);
	void setStreamPort(int streamPort);
	void setRegistered(bool isRegistered);
	void setLoggedIn(bool isLoggedIn);

signals:
	void macAddressChanged(QString macAddress);
	void nameChanged(QString name);
	void locationChanged(QString location);
	void streamUrlChanged(QString streamUrl);
	void streamPortChanged(int streamPort);
	void registeredChanged(bool isRegistered);
	void loggedInChanged(bool isLoggedIn);

private:
	QString macAddress_;
	QString name_;
	QString location_;
	QString streamUrl_;
	int streamPort_;
	bool registered_;
	bool loggedIn_;
};
