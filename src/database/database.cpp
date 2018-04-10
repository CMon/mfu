#include "database.h"

#include <QDebug>
#include <QSqlError>
#include <QVariant>

QThreadStorage<DB::Internal::ThreadData *> DB::Internal::Transaction::threadData_;

DB::Internal::Transaction::Transaction()
    : td_(threadData_.hasLocalData() ? *(threadData_.localData())
                                     : (static_cast<void>(threadData_.setLocalData(new ThreadData())), *(threadData_.localData())))
    , db(*td_.db)
    , committed_(false)
{
	nested_ = td_.transactionActive;
	qDebug () << QString("Database %1transaction start").arg(nested_ ? "sub-" : "");
	if (!nested_) {
		td_.transactionActive = true;
		if (!db.transaction()) qCritical() << "starting DB transaction failed:" << db.lastError().text();
		watch_.start();
	}
}

DB::Internal::Transaction::~Transaction()
{
	if (!committed_) {
		qWarning() << "Rolling back because of uncommitted state";
		td_.doRollback = true;
	}
	if (td_.doRollback) {
		qDebug () << QString("Database %1transaction rollback").arg(nested_ ? "sub-" : "");
		if (!nested_) {
			if (!db.rollback()) qWarning() << "Database transaction rollback failed:" << db.lastError().text();
			td_.doRollback = false;
			td_.transactionActive = false;
		}
	}
}

bool DB::Internal::Transaction::commit()
{
	if (committed_) {
		qCritical() << "Database transaction committed twice";
		return false;
	}
	committed_ = true;

	if (td_.doRollback) return false;

	if (nested_) {
		qDebug() << "Database sub-transaction commit";
		return true;
	}

	QElapsedTimer watch;
	watch.start();
	bool ok = db.commit();
	if (!ok) {
		qWarning() << "Database transaction commit failed: " << db.lastError().text();
	} else {
		qDebug() << QString("Database transaction commit (%1/%2 msec)").arg(watch.elapsed()).arg(watch_.elapsed());
	}

	td_.transactionActive = false;
	return ok;
}

void DB::Internal::Transaction::closeDBConnection()
{
	threadData_.setLocalData(nullptr);
}

void DB::Internal::closeDBConnection()
{
	DB::Internal::Transaction::closeDBConnection();
}

bool DB::Internal::exec(QSqlQuery & query)
{
	if (!query.lastError().isValid()) {
		if (query.exec()) return true;
	}
	qDebug () << "Query:" << query.lastQuery();
	qWarning () << "SQL error:" << query.lastError().text();

	return false;
}

quint64 DB::Internal::lastInsertId()
{
	TRANSACTION;

	QSqlQuery query(t.db);
	query.prepare("SELECT LAST_INSERT_ID()");
	if (!execQuery(query) || !query.next()) {
		return 0;
	}
	const quint64 lastId = query.value(0).toULongLong();
	t.commit();

	return lastId;
}
