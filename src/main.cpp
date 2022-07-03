#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QString>
#include <QList>
#include <QUrl>

#include "StreamDeckProxy.h"
#include "StreamDeckPlugin.h"


int main(int argc, char *argv[])
{
	// create qt application
	QCoreApplication a(argc, argv);

	// register some types, to be used with queued signals and slots
	qRegisterMetaType<ESDSDKTarget>("ESDSDKTarget");
	qRegisterMetaType<QUrl>("QUrl");

	// setup commandline parser
	QCommandLineParser parser;
	parser.addHelpOption();
	QCommandLineOption portOption("port", "", "port", "0");
	QCommandLineOption pluginUuidOption("pluginUUID", "", "pluginUUID", "");
	QCommandLineOption registerEventOption("registerEvent", "", "registerEvent", "");
	QCommandLineOption infoOption("info", "", "info", "");
	parser.addOptions({portOption, pluginUuidOption, registerEventOption, infoOption});
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	// parse arguments
	parser.process(a);

	// streamdeck connection parameters
	uint port = parser.value(portOption).toUInt();
	QString pluginUUID = parser.value(pluginUuidOption);
	QString registerEvent = parser.value(registerEventOption);
	QString info = parser.value(infoOption);

	// create streamdeck proxy
	StreamDeckProxy proxy(port, registerEvent, pluginUUID, &a);

	// create plugin instance here
	StreamDeckProxy* plugin = nullptr;

	// register the plugin with the streamdeck proxy
	// streamdeck proxy handles connection with streamdeck through websocket connection
	// the plugin implements the logic and acts upon streamdeck events and sends data back
	// you could register any amount of plugins with the proxy if code should be split across multiple plugin classes
	// communication between proxy and plugin uses qt signal/slot mechanism
	proxy.use(plugin);

	// execute qt event loop
	return a.exec();
}
