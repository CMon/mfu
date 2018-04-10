#include <database/database.h>
#include <database/dbusers.h>
#include <database/schemeupdater.h>
#include <mfu-common/util.h>
#include <server/rpcserver.h>
#include <services/clientservice.h>
#include <services/userservice.h>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QHostAddress>

bool addUser()
{
	QTextStream out(stdout);
	QTextStream in(stdin);
	out << QCoreApplication::translate("main", "Please enter the username: "); out.flush();
	const QString username = in.readLine();
	const QString password = Util::getPasswordFromConsole(QCoreApplication::translate("main", "Please enter your password: "));
	const QString checkPassword = Util::getPasswordFromConsole(QCoreApplication::translate("main", "Please retype your password: "));
	if (password != checkPassword) {
		qCritical() << QCoreApplication::translate("main", "Passwords did not match, aborting");
		return false;
	}

	out << QCoreApplication::translate("main", "Please enter a comma separated list of permission ids: "); out.flush();
	QStringList permissionsStr = in.readLine().split(",", QString::SkipEmptyParts);
	QSet<User::Permission> permissions;
	std::for_each(permissionsStr.begin(), permissionsStr.end(), [&permissions](const QString & str) {
		bool ok = false;
		User::Permission perm = User::Permission(str.toInt(&ok));
		if (ok) permissions << perm;
	});

	if (!DB::addUser(username, QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toBase64()), permissions)) {
		out << QCoreApplication::translate("main", "Add of user failed, because of db errors, see log for more information.");
		return false;
	}

	out << QCoreApplication::translate("main", "Added user:") << " " << username << endl;
	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QCommandLineOption listenOnOpt(QStringList() << "l" << "listenOn",
	                               QCoreApplication::translate("main", "listen on address (default Any)"),
	                               "listenOn",
	                               QHostAddress(QHostAddress::Any).toString());
	QCommandLineOption portOpt(QStringList() << "p" << "port",
	                           QCoreApplication::translate("main", "the port to listen on (default: 4711)"),
	                           "port", QString::number(4711));
	QCommandLineOption createUserOpt(QStringList() << "c" << "createUser",
	                                 QCoreApplication::translate("main", "used to create a user in the database"));
	QCommandLineParser argParser;
	argParser.addHelpOption();
	argParser.addOption(listenOnOpt);
	argParser.addOption(portOpt);
	argParser.addOption(createUserOpt);
	argParser.process(app);


	// init database!!
	PATH_DB_PARAMETER("QSQLITE", "mfu-daemon.db");
	if (!DB::Scheme::update(":/database/scheme.sql")) {
		qCritical() << QCoreApplication::translate("main", "Could not update database");
		return -1;
	}

	if (argParser.isSet(createUserOpt)) {
		if (!addUser()) {
			return -2;
		} else {
			return 0;
		}
	}

	// init network
	const QHostAddress listenOn = QHostAddress(argParser.value(listenOnOpt));
	const quint16 port = quint16(argParser.value(portOpt).toInt());

	RPCServer rpcServer(listenOn, port);
	UserService userService;
	userService.registerMethods(&rpcServer);

	ClientService regService(&userService);
	regService.registerMethods(&rpcServer);

	if (!rpcServer.start()) {
		qCritical() << QCoreApplication::translate("main", "Could not start server");
		return -3;
	}

	return app.exec();
}
