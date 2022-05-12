#include "StreamDeckProxy.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>


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

void StreamDeckProxy::textMessageReceived(const QString& message) 
{
	// parse message as json
	QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());

	// parse as json object
	QJsonObject json = jsonDoc.object();
		
	// read all common values
	QString event = json[kESDSDKCommonEvent].toString();
	QString context = json[kESDSDKCommonContext].toString();
	QString action = json[kESDSDKCommonAction].toString();
	QString deviceId = json[kESDSDKCommonDevice].toString();
	QJsonObject payload = json[kESDSDKCommonPayload].toObject();

	// emit the corresponding signal
	if (event == kESDSDKEventKeyDown) {
		emit keyDown(action, context, payload, deviceId);
		return;
	}

	if (event == kESDSDKEventKeyUp) {
		emit keyUp(action, context, payload, deviceId);
		return;
	}

	if (event == kESDSDKEventWillAppear) {
		emit willAppear(action, context, payload, deviceId);
		return;
	}

	if (event == kESDSDKEventWillDisappear) {
		emit willDisappear(action, context, payload, deviceId);
		return;
	}

	if (event == kESDSDKEventDeviceDidConnect) {
		QJsonObject deviceInfo = json[kESDSDKCommonDeviceInfo].toObject();
		emit deviceConnected(deviceId, deviceInfo);
		return;
	}

	if (event == kESDSDKEventDeviceDidDisconnect) {
		emit deviceDisconnected(deviceId);
		return;
	}

	if (event == kESDSDKEventSendToPlugin) {
		emit sendToPlugin(action, context, payload, deviceId);
		return;
	}
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
	// 
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
