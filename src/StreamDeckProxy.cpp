#include "StreamDeckProxy.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <stdexcept>


StreamDeckProxy::StreamDeckProxy(uint port, const QString& event, const QString& uuid, QObject* parent) : 
	_registerEvent(event), _pluginUuid(uuid), QObject(parent)
{
	// connect websocket signals to class slot handlers
	connect(&_socket, &QWebSocket::textMessageReceived, this, &StreamDeckProxy::textMessageReceived);
	connect(&_socket, &QWebSocket::connected, this, &StreamDeckProxy::connected);
	connect(&_socket, &QWebSocket::disconnected, this, &StreamDeckProxy::disconnected);

	// open websocket connection
	_socket.open(QString::asprintf("ws://127.0.0.1:%d", port));
}

void StreamDeckProxy::use(StreamDeckPlugin* plugin) 
{
	// check if plugin is not null
	if (plugin == nullptr) 
	{
		throw std::runtime_error("plugin must not be null");
	}
	
	// streamdeck proxy --> plugin
	connect(this, &StreamDeckProxy::keyDown, plugin, &StreamDeckPlugin::keyDown);
	connect(this, &StreamDeckProxy::keyUp, plugin, &StreamDeckPlugin::keyUp);
	connect(this, &StreamDeckProxy::willAppear, plugin, &StreamDeckPlugin::willAppear);
	connect(this, &StreamDeckProxy::willDisappear, plugin, &StreamDeckPlugin::willDisappear);
	connect(this, &StreamDeckProxy::titleParameterDidChange, plugin, &StreamDeckPlugin::titleParameterDidChange);
	connect(this, &StreamDeckProxy::deviceDidConnect, plugin, &StreamDeckPlugin::deviceDidConnect);
	connect(this, &StreamDeckProxy::deviceDidDisconnect, plugin, &StreamDeckPlugin::deviceDidDisconnect);
	connect(this, &StreamDeckProxy::applicationDidLaunch, plugin, &StreamDeckPlugin::applicationDidLaunch);
	connect(this, &StreamDeckProxy::applicationDidTerminate, plugin, &StreamDeckPlugin::applicationDidTerminate);
	connect(this, &StreamDeckProxy::systemDidWakeUp, plugin, &StreamDeckPlugin::systemDidWakeUp);
	connect(this, &StreamDeckProxy::propertyInspectorDidAppear, plugin, &StreamDeckPlugin::propertyInspectorDidAppear);
	connect(this, &StreamDeckProxy::propertyInspectorDidDisappear, plugin, &StreamDeckPlugin::propertyInspectorDidDisappear);
	connect(this, &StreamDeckProxy::didReceiveSettings, plugin, &StreamDeckPlugin::didReceiveSettings);
	connect(this, &StreamDeckProxy::didReceiveGlobalSettings, plugin, &StreamDeckPlugin::didReceiveGlobalSettings);
	connect(this, &StreamDeckProxy::sendToPlugin, plugin, &StreamDeckPlugin::sendToPlugin);
	// plugin --> streamdeck proxy
	connect(plugin, &StreamDeckPlugin::setSettings, this, &StreamDeckProxy::setSettings);
	connect(plugin, &StreamDeckPlugin::getSettings, this, &StreamDeckProxy::getSettings);
	connect(plugin, &StreamDeckPlugin::setGlobalSettings, this, &StreamDeckProxy::setGlobalSettings);
	connect(plugin, &StreamDeckPlugin::getGlobalSettings, this, &StreamDeckProxy::getGlobalSettings);
	connect(plugin, &StreamDeckPlugin::openUrl, this, &StreamDeckProxy::openUrl);
	connect(plugin, &StreamDeckPlugin::setTitle, this, &StreamDeckProxy::setTitle);
	connect(plugin, &StreamDeckPlugin::setImage, this, &StreamDeckProxy::setImage);
	connect(plugin, &StreamDeckPlugin::showAlertForContext, this, &StreamDeckProxy::showAlertForContext);
	connect(plugin, &StreamDeckPlugin::showOkForContext, this, &StreamDeckProxy::showOkForContext);
	connect(plugin, &StreamDeckPlugin::setState, this, &StreamDeckProxy::setState);
	connect(plugin, &StreamDeckPlugin::sendToPropertyInspector, this, &StreamDeckProxy::sendToPropertyInspector);
	connect(plugin, &StreamDeckPlugin::switchToProfile, this, &StreamDeckProxy::switchToProfile);
	connect(plugin, &StreamDeckPlugin::logMessage, this, &StreamDeckProxy::logMessage);
}

void StreamDeckProxy::textMessageReceived(const QString& message) 
{
	// parse message as json
	QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());

	// parse as json object
	QJsonObject json = jsonDoc.object();
		
	// read all common values
	QString ev = json[kESDSDKCommonEvent].toString();
	QString context = json[kESDSDKCommonContext].toString();
	QString action = json[kESDSDKCommonAction].toString();
	QString deviceId = json[kESDSDKCommonDevice].toString();
	QJsonObject payload = json[kESDSDKCommonPayload].toObject();

	// emit the corresponding signal
	if (ev == kESDSDKEventKeyDown) {
		emit keyDown(action, context, payload, deviceId);
		return;
	}

	if (ev == kESDSDKEventKeyUp) {
		emit keyUp(action, context, payload, deviceId);
		return;
	}

	if (ev == kESDSDKEventWillAppear) {
		emit willAppear(action, context, payload, deviceId);
		return;
	}

	if (ev == kESDSDKEventWillDisappear) {
		emit willDisappear(action, context, payload, deviceId);
		return;
	}

	if (ev == kESDSDKEventDeviceDidConnect) {
		QJsonObject deviceInfo = json[kESDSDKCommonDeviceInfo].toObject();
		emit deviceDidConnect(deviceId, deviceInfo);
		return;
	}

	if (ev == kESDSDKEventDeviceDidDisconnect) {
		emit deviceDidDisconnect(deviceId);
		return;
	}

	if (ev == kESDSDKEventSendToPlugin) {
		emit sendToPlugin(action, context, payload, deviceId);
		return;
	}

	if (ev == kESDSDKEventDidReceiveSettings) {
		emit didReceiveSettings(payload, context, deviceId);
	}

	logMessage(ev);
}

void StreamDeckProxy::connected() 
{
	// send plugin registration
	QJsonObject json;
	json[kESDSDKCommonEvent] = _registerEvent;
	json[kESDSDKRegisterUUID] = _pluginUuid;

	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::disconnected()
{
	// not implemented
}

void StreamDeckProxy::getSettings(const QString& context)
{
}

void StreamDeckProxy::setGlobalSettings(const QJsonObject& settings, const QString& context)
{
}

void StreamDeckProxy::getGlobalSettings(const QString& context)
{
}

void StreamDeckProxy::openUrl(const QUrl& url)
{
}

void StreamDeckProxy::setTitle(const QString& title, const QString& context, ESDSDKTarget target)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSetTitle;
	json[kESDSDKCommonContext] = context;

	QJsonObject payload;
	payload[kESDSDKPayloadTarget] = target;
	payload[kESDSDKPayloadTitle] = title;
	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::setImage(const QString& base64Image, const QString& context, ESDSDKTarget target)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSetImage;
	json[kESDSDKCommonContext] = context;

	QJsonObject payload;
	payload[kESDSDKPayloadTarget] = target;

	QString prefix("data:image/png;base64,");

	if (base64Image.startsWith(prefix)) {
		payload[kESDSDKPayloadImage] = base64Image;
	}
	else {
		payload[kESDSDKPayloadImage] = prefix + base64Image;
	}

	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::showAlertForContext(const QString& context)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventShowAlert;
	json[kESDSDKCommonContext] = context;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::showOkForContext(const QString& context)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventShowOK;
	json[kESDSDKCommonContext] = context;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::setSettings(const QJsonObject& settings, const QString& context)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSetSettings;
	json[kESDSDKCommonContext] = context;
	json[kESDSDKCommonPayload] = settings;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::setState(int state, const QString& context) 
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSetState;
	json[kESDSDKCommonContext] = context;

	QJsonObject payload;
	payload[kESDSDKPayloadState] = state;
	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::sendToPropertyInspector(const QString& action, const QString& context, const QJsonObject& payload)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSendToPropertyInspector;
	json[kESDSDKCommonContext] = context;
	json[kESDSDKCommonAction] = action;
	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::switchToProfile(const QString& deviceId, const QString& profileName)
{	
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventSwitchToProfile;
	json[kESDSDKCommonContext] = _pluginUuid;
	json[kESDSDKCommonDevice] = deviceId;

	QJsonObject payload;
	payload[kESDSDKPayloadProfile] = profileName;
	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}

void StreamDeckProxy::logMessage(const QString& message)
{
	// prepare json object
	QJsonObject json;
	json[kESDSDKCommonEvent] = kESDSDKEventLogMessage;

	QJsonObject payload;
	payload[kESDSDKPayloadMessage] = message;
	json[kESDSDKCommonPayload] = payload;

	// send json over websocket
	_socket.sendTextMessage(QJsonDocument(json).toJson());
}
