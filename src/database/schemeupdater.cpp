#include "schemeupdater.h"

#include <database/database.h>

#include <QSqlError>
#include <QSqlQuery>
#include <QString>

namespace  {

bool insertVersion(const QString & version)
{
	TRANSACTION;

	QSqlQuery query(t.db);
	query.prepare(
	            "INSERT INTO __scheme_versions__ "
	            "(version, applied) "
	            "VALUES "
	            "(:version, :applied) "
	            );
	query.bindValue(":version", version);
	query.bindValue(":applied", QDateTime::currentDateTimeUtc());
	return execQuery(query) && t.commit();
}

bool applyVersion(const QString & queryStr)
{
	static const QRegularExpression commentRe("(?:^|\n)--.+");

	QString cleanQuery = queryStr;
	cleanQuery.replace(commentRe, "");
	cleanQuery = cleanQuery.trimmed();
	if (cleanQuery.isEmpty()) return true;

	TRANSACTION;
	bool retval = false;
	QStringList errors;
	QSqlQuery query(t.db);
	if (t.db.driverName().contains("sqlite", Qt::CaseInsensitive)) {
		// sqlite does not support multiple queries per one query
		cleanQuery.replace("\n", "");
		for (QString singleQuery: cleanQuery.split(";", QString::SkipEmptyParts)) {
			retval = query.exec(singleQuery);
			if (!retval) errors << query.lastError().text();
		}
	} else {
		retval = query.exec(cleanQuery);
		if (!retval) errors << query.lastError().text();
	}

	if (!retval) {
		qWarning() << "errors occured during apply:" << errors;
	}

	return retval && t.commit();
}

}

bool DB::Scheme::update(const QString & filename)
{
	TRANSACTION;

	// get current versions OR create versions table
	QSet<QString> existingVersions;
	QSqlQuery versionQuery(t.db);
	if (!versionQuery.exec("SELECT version FROM __scheme_versions__")) {
		qInfo() << "creating table __scheme_versions__";
		if (!versionQuery.exec(
		        "CREATE TABLE __scheme_versions__ ("
		        "version text NOT NULL, "
		        "applied timestamp with time zone NOT NULL, "
		        "PRIMARY KEY (version)"
		       ")"))
		{
			qCritical() << "Failed to create scheme versions table";
			return false;
		}
	} else {
		while(versionQuery.next()) {
			existingVersions << versionQuery.value(0).toString();
		}
	}

	// read current version data
	const QRegularExpression revisionRegExp("^-- VERSION (.+)$", QRegularExpression::MultilineOption);

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qCritical() << "could not open scheme file" << filename << file.errorString();
		return false;
	}

	const QByteArray fileContent = file.readAll();
	if (fileContent.isEmpty()) {
		qWarning() << "Scheme file is empty";
		return true;
	}
	const QString utf8Schema = QString::fromUtf8(fileContent);

	int start = 0;
	QRegularExpressionMatch match = revisionRegExp.match(utf8Schema, start);
	QString lastRev = "__initial__";
	while (match.hasMatch()) {
		if (!existingVersions.contains(lastRev)) {
			qInfo() << "applying version" << lastRev;
			if (!applyVersion(utf8Schema.mid(start, match.capturedStart() - start))) return false;
			if (!insertVersion(lastRev)) return false;
		}

		lastRev = match.captured(1);
		start = match.capturedEnd();
		match = revisionRegExp.match(utf8Schema, start);
	}

	if (!existingVersions.contains(lastRev)) {
		qInfo() << "applying revision" << lastRev;
		if (!applyVersion(utf8Schema.mid(start, match.capturedStart() - start))) return false;
		if (!insertVersion(lastRev)) return false;
	}

	return t.commit();
}
