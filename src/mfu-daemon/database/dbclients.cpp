#include "dbclients.h"

#include <database/database.h>

#include <QSqlQuery>
#include <QVariant>

ClientData DB::getClient(const QString & macAddress)
{
	TRANSACTION;

	QSqlQuery query(t.db);
	query.prepare("SELECT macAddress, name, location, streamUrl, streamPort, registered FROM clients WHERE macAddress = :macAddress");
	query.bindValue(":macAddress", macAddress);
	execQuery(query);
	ClientData cd;
	if (query.next()) {
		const QString dbMacAddress = query.value(0).toString();
		const QString name         = query.value(1).toString();
		const QString location     = query.value(2).toString();
		const QString streamUrl    = query.value(3).toString();
		const int streamPort       = query.value(4).toInt();
		const bool registered      = query.value(5).toBool();
		cd = ClientData(dbMacAddress, name, location, streamUrl, streamPort, registered);
	}
	t.commit();
	return cd;
}

bool DB::createUnregisteredClient(const QString & macAddress)
{
	TRANSACTION;
	QSqlQuery query(t.db);
	query.prepare("INSERT INTO clients "
	              "(macAddress, registered) "
	              "VALUES "
	              "(:macAddress, :registered)");
	query.bindValue(":macAddress", macAddress);
	query.bindValue(":registered", false);
	execQuery(query);

	return t.commit();
}
