#include <client/rpcclient.h>
#include <mfu-client-qml/mfu-client.h>
#include <mfu-common/clientdataresponse.h>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QCommandLineOption hostOpt(QStringList() << "d" << "hostname",
	                               QCoreApplication::translate("main", "the host of the mfu-daemon (default: localhost)"),
	                               "hostname",
	                               "localhost");
	QCommandLineOption portOpt(QStringList() << "p" << "port",
	                           QCoreApplication::translate("main", "the port of the mfu-daemon (default: 4711)"),
	                           "port", QString::number(4711));
	QCommandLineParser argParser;
	argParser.addHelpOption();
	argParser.addOption(hostOpt);
	argParser.addOption(portOpt);
	argParser.process(app);

	// init network
	const QString host = argParser.value(hostOpt);
	const quint16 port = quint16(argParser.value(portOpt).toInt());

	RPCClient rpcClient(QString("ws://%1:%2").arg(host).arg(port));
	MFUClient mfuClient(&rpcClient);
	rpcClient.connectToServer();

	// dummy code until there is no gui/other way of output
	QObject::connect(&mfuClient, &MFUClient::clientDataUpdate, [] (const ClientDataResponse & cdr) {
		QTextStream out(stdout);
		out << "got new client data:" << endl;
		out << "MacAddress: " << cdr.macAddress()   << endl;
		out << "Name:       " << cdr.name()         << endl;
		out << "Location:   " << cdr.location()     << endl;
		out << "StreamUrl:  " << cdr.streamUrl()    << endl;
		out << "StreamPort: " << cdr.streamPort()   << endl;
		out << "Registered: " << cdr.isRegistered() << endl;
	});

	return app.exec();
}
