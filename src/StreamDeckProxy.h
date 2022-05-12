#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>

#include "ESDSDKDefines.h"


class StreamDeckProxy : 
	public QObject
{
	Q_OBJECT

public:
	explicit StreamDeckProxy(uint port, const QString& event, const QString& uuid, QObject* parent = nullptr);

public slots:
	void textMessageReceived(const QString& message);
	void connected();
	void disconnected();

	void setTitle(const QString& title, const QString& context, ESDSDKTarget target);
	void setImage(const QString& base64Image, const QString& context, ESDSDKTarget target);
	void showAlertForContext(const QString& context);
	void showOkForContext(const QString& context);
	void setSettings(const QJsonObject& settings, const QString& context);
	void setState(int state, const QString& context);
	void sendToPropertyInspector(const QString& action, const QString& context, const QJsonObject& payload);
	void switchToProfile(const QString& deviceId, const QString& profileName);
	void logMessage(const QString& message);

signals:
	void keyDown(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void keyUp(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);

	void willAppear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void willDisappear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);

	void deviceConnected(const QString& deviceId, const QJsonObject& deviceInfo);
	void deviceDisconnected(const QString& deviceId);
	
	void sendToPlugin(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);

private:
	QWebSocket _socket;
	QString _registerEvent;
	QString _pluginUuid;
};

