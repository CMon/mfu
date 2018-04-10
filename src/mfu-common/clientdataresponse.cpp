#include "clientdataresponse.h"

ClientDataResponse::ClientDataResponse()
    : streamPort_(0)
    , registered_(false)
{
}

ClientDataResponse::ClientDataResponse(const ClientData & cd)
    : macAddress_(cd.macAddress())
    , name_      (cd.name())
    , location_  (cd.location())
    , streamUrl_ (cd.streamUrl())
    , streamPort_(cd.streamPort())
    , registered_(cd.isRegistered())
    , loggedIn_  (cd.isLoggedIn())
{
}

ClientData ClientDataResponse::toClientData() const
{
	ClientData cd(macAddress_, name_, location_, streamUrl_, streamPort_, registered_);
	cd.setLoggedIn(loggedIn_);
	return cd;
}

QString ClientDataResponse::macAddress() const
{
	return macAddress_;
}

QString ClientDataResponse::name() const
{
	return name_;
}

QString ClientDataResponse::location() const
{
	return location_;
}

QString ClientDataResponse::streamUrl() const
{
	return streamUrl_;
}

int ClientDataResponse::streamPort() const
{
	return streamPort_;
}

bool ClientDataResponse::isRegistered() const
{
	return registered_;
}

bool ClientDataResponse::isLoggedIn() const
{
	return loggedIn_;
}

void ClientDataResponse::setMacAddress(QString macAddress)
{
	if (macAddress_ == macAddress)
		return;

	macAddress_ = macAddress;
	emit macAddressChanged(macAddress_);
}

void ClientDataResponse::setName(QString name)
{
	if (name_ == name)
		return;

	name_ = name;
	emit nameChanged(name_);
}

void ClientDataResponse::setLocation(QString location)
{
	if (location_ == location)
		return;

	location_ = location;
	emit locationChanged(location_);
}

void ClientDataResponse::setStreamUrl(QString streamUrl)
{
	if (streamUrl_ == streamUrl)
		return;

	streamUrl_ = streamUrl;
	emit streamUrlChanged(streamUrl_);
}

void ClientDataResponse::setStreamPort(int streamPort)
{
	if (streamPort_ == streamPort)
		return;

	streamPort_ = streamPort;
	emit streamPortChanged(streamPort_);
}

void ClientDataResponse::setRegistered(bool registered)
{
	if (registered_ == registered)
		return;

	registered_ = registered;
	emit registeredChanged(registered_);
}

void ClientDataResponse::setLoggedIn(bool isLoggedIn)
{
	if (loggedIn_ == isLoggedIn)
		return;

	loggedIn_ = isLoggedIn;
	emit loggedInChanged(loggedIn_);
}
