#include <client/rpcclient.h>
#include <mfu-admin/mfuadmin.h>
#include <mfu-admin/clientsmodel.h>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScopedPointer>

QScopedPointer<RPCClient> Client(new RPCClient()); // has to be global otherwise the singleton is unable to get it

int main(int argc, char *argv[])
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);
	app.setOrganizationName("cmon");
	app.setApplicationName("MFU Admin gui");

	QQmlApplicationEngine engine;

	qmlRegisterSingletonType<MFUAdmin>("CMon.MFUAdmin", 1, 0, "MFUAdmin", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		MFUAdmin * mfuAdmin = new MFUAdmin(Client.data());
		return mfuAdmin;
	});

	ClientsModel clientsModel(Client.data());
	engine.rootContext()->setContextProperty("clientsModel", &clientsModel);

	engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));

	return app.exec();
}
