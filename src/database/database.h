#pragma once

#define TRANSACTION DB::Internal::Transaction t
#define execQuery(query) DB::Internal::exec(query)
#define NET_DB_PARAMETER(type, host, port, database, user, password) DB::Internal::setParameter(type, host, port, database, user, password)
#define PATH_DB_PARAMETER(type, path) DB::Internal::setParameter(type, path)

#include "database_p.h"

#include <QElapsedTimer>
#include <QSqlQuery>
#include <QThreadStorage>

namespace DB {

namespace Internal {

class Transaction
{
	Q_DISABLE_COPY(Transaction)
private:
	static QThreadStorage<ThreadData *> threadData_;
	ThreadData & td_;

public:
	Transaction();
	~Transaction();

	QSqlDatabase & db;

	bool commit();

	static void closeDBConnection();

private:
	bool nested_;
	bool committed_;
	QElapsedTimer watch_;
};

void closeDBConnection();
bool exec(QSqlQuery & query);
quint64 lastInsertId();

}

}
