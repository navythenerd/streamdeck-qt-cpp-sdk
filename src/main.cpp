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
	// streamdeck --> plugin
	QObject::connect(&proxy, &StreamDeckProxy::keyDown, plugin, &StreamDeckPlugin::keyDown);
	QObject::connect(&proxy, &StreamDeckProxy::keyUp, plugin, &StreamDeckPlugin::keyUp);
	QObject::connect(&proxy, &StreamDeckProxy::willAppear, plugin, &StreamDeckPlugin::willAppear);
	QObject::connect(&proxy, &StreamDeckProxy::willDisappear, plugin, &StreamDeckPlugin::willDisappear);
	QObject::connect(&proxy, &StreamDeckProxy::willDisappear, plugin, &StreamDeckPlugin::willDisappear);
	QObject::connect(&proxy, &StreamDeckProxy::deviceConnected, plugin, &StreamDeckPlugin::deviceConnected);
	QObject::connect(&proxy, &StreamDeckProxy::deviceDisconnected, plugin, &StreamDeckPlugin::deviceDisconnected);
	QObject::connect(&proxy, &StreamDeckProxy::sendToPlugin, plugin, &StreamDeckPlugin::sendToPlugin);
	// plugin --> streamdeck
	QObject::connect(plugin, &StreamDeckPlugin::setTitle, &proxy, &StreamDeckProxy::setTitle);
	QObject::connect(plugin, &StreamDeckPlugin::setImage, &proxy, &StreamDeckProxy::setImage);
	QObject::connect(plugin, &StreamDeckPlugin::showAlertForContext, &proxy, &StreamDeckProxy::showAlertForContext);
	QObject::connect(plugin, &StreamDeckPlugin::showOkForContext, &proxy, &StreamDeckProxy::showOkForContext);
	QObject::connect(plugin, &StreamDeckPlugin::setSettings, &proxy, &StreamDeckProxy::setSettings);
	QObject::connect(plugin, &StreamDeckPlugin::setState, &proxy, &StreamDeckProxy::setState);
	QObject::connect(plugin, &StreamDeckPlugin::sendToPropertyInspector, &proxy, &StreamDeckProxy::sendToPropertyInspector);
	QObject::connect(plugin, &StreamDeckPlugin::switchToProfile, &proxy, &StreamDeckProxy::switchToProfile);
	QObject::connect(plugin, &StreamDeckPlugin::logMessage, &proxy, &StreamDeckProxy::logMessage);

	// execute qt event loop
	return a.exec();
}