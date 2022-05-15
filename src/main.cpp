#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QString>

#include "StreamDeckProxy.h"
#include "StreamDeckPlugin.h"


int main(int argc, char *argv[])
{
	// create qt application
	QCoreApplication a(argc, argv);

	// register ESDSDKTarget, to be used with queued signals and slots
	qRegisterMetaType<ESDSDKTarget>("ESDSDKTarget");

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
	StreamDeckPlugin* plugin = nullptr;

	// connect plugin to streamdeck proxy
	proxy.use(plugin);

	// execute qt event loop
	return a.exec();
}