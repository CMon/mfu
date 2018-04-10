#pragma once

#include <QString>

class ClientData
{
public:
	ClientData();
	ClientData(const QString & macAddress, const QString & name, const QString & location, const QString & streamUrl,
	           const int streamPort, const bool registered = false);

	void setLoggedIn(bool loggedIn);

	QString macAddress() const;
	QString name() const;
	QString location() const;
	QString streamUrl() const;
	int streamPort() const;
	bool isRegistered() const;
	bool isLoggedIn() const;

	bool operator==(const ClientData & rhs) const;
	bool operator!=(const ClientData & rhs) const;

private:
	QString macAddress_;
	QString name_;
	QString location_;
	QString streamUrl_;
	int streamPort_;
	bool registered_;
	bool loggedIn_;
};
