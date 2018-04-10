#include "database_p.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

namespace {

QString dbType;
QString dbPath;
QString dbDatabase;
QString dbUser;
QString dbPassword;
QString dbHost;
int     dbPort;

QAtomicInt dbNameId;

QSqlDatabase createNewConnection(const QString & datbaseType)
{
	return QSqlDatabase::addDatabase(datbaseType, "DB-Connection-" + QString::number(dbNameId.fetchAndAddRelaxed(1)));
}
}

DB::Internal::ThreadData::ThreadData()
    : db(new QSqlDatabase(createNewConnection(dbType)))
    , transactionActive(false)
    , doRollback(false)
{
	qDebug() << "new DB connection:" << db->connectionName();
	if (dbPath.isEmpty()) {
		qDebug () << "connection based database is used";
		db->setHostName(dbHost);
		db->setPort(dbPort);
		db->setDatabaseName(dbDatabase);
		db->setUserName(dbUser);
		db->setPassword(dbPassword);
	} else {
		qDebug () << "path based database is used";
		db->setDatabaseName(dbPath);
	}
	if (!db->open()) {
		qWarning() << "Could not open database, error:" << db->lastError().text();
	}

	QTimer * timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(checkConnection()));
	timer->start(15 * 1000);
}

DB::Internal::ThreadData::~ThreadData()
{
	const QString name = db->connectionName();
	db->close();
	delete db;
	QSqlDatabase::removeDatabase(name);
	qDebug () << "DB connection" << name << "closed";
}

void DB::Internal::ThreadData::checkConnection()
{
	QSqlQuery query(*db);
	if (!query.exec("SELECT 1") || !query.next()) {
		qWarning() << "DB error on connection check:" << db->lastError().text();
		if (!db->open()) qWarning() << "DB error:" << db->lastError().text();
	}
}

void DB::Internal::setParameter(const QString & type, const QString & host, const int port, const QString & database, const QString & user, const QString & password)
{
	dbType     = type;
	dbHost     = host;
	dbPort     = port;
	dbDatabase = database;
	dbUser     = user;
	dbPassword = password;
}

void DB::Internal::setParameter(const QString & type, const QString & path)
{
	dbType = type;
	dbPath = path;
}
