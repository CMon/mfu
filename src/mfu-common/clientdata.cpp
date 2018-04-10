#include "clientdata.h"

ClientData::ClientData()
    : streamPort_(0)
    , registered_(false)
{
}

ClientData::ClientData(const QString & macAddress, const QString & name, const QString & location, const QString & streamUrl,
                       const int streamPort, const bool registered)
    : macAddress_(macAddress)
    , name_(name)
    , location_(location)
    , streamUrl_(streamUrl)
    , streamPort_(streamPort)
    , registered_(registered)
{
}

void ClientData::setLoggedIn(bool loggedIn)
{
	loggedIn_ = loggedIn;
}

QString ClientData::macAddress() const
{
	return macAddress_;
}

QString ClientData::name() const
{
	return name_;
}

QString ClientData::location() const
{
	return location_;
}

QString ClientData::streamUrl() const
{
	return streamUrl_;
}

int ClientData::streamPort() const
{
	return streamPort_;
}

bool ClientData::isRegistered() const
{
	return registered_;
}

bool ClientData::isLoggedIn() const
{
	return loggedIn_;
}

bool ClientData::operator==(const ClientData & rhs) const
{
	return
	        macAddress_ == rhs.macAddress_ &&
	        name_       == rhs.name_       &&
	        location_   == rhs.location_   &&
	        streamUrl_  == rhs.streamUrl_  &&
	        streamPort_ == rhs.streamPort_ &&
	        registered_ == rhs.registered_ &&
	        loggedIn_   == rhs.loggedIn_
	        ;

}

bool ClientData::operator!=(const ClientData & rhs) const
{
	return !operator==(rhs);
}
