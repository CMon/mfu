#pragma once

#include <QObject>

class QSqlDatabase;

namespace DB {

namespace Internal {

class ThreadData : public QObject
{
	Q_OBJECT
public:
	QSqlDatabase * db;
	bool transactionActive;
	bool doRollback;

	ThreadData();
	~ThreadData();

private slots:
	void checkConnection();
};

void setParameter(const QString & type, const QString & host, const int port, const QString & database, const QString & user, const QString & password);
void setParameter(const QString & type, const QString & path);
}}
