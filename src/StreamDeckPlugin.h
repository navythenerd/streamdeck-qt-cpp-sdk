#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "ESDSDKDefines.h"


class StreamDeckPlugin : 
	public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StreamDeckPlugin)

public:
	StreamDeckPlugin(QObject* parent = nullptr) : QObject(parent) {};
	virtual ~StreamDeckPlugin() = default;

public slots:
	virtual void keyDown(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId) = 0;
	virtual void keyUp(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId) = 0;
	virtual void willAppear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId) = 0;
	virtual void willDisappear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId) = 0;
	virtual void titleParameterDidChange(const QString& action, const QString& context, const QJsonObject& payload, QString& deviceId) = 0;
	virtual void deviceDidConnect(const QString& deviceId, const QJsonObject& deviceInfo) = 0;
	virtual void deviceDidDisconnect(const QString& deviceId) = 0;
	virtual void applicationDidLaunch(const QJsonObject& payload) = 0;
	virtual void applicationDidTerminate(const QJsonObject& payload) = 0;
	virtual void systemDidWakeUp() = 0;
	virtual void propertyInspectorDidAppear(const QString& context, const QString& deviceId) = 0;
	virtual void propertyInspectorDidDisappear(const QString& context, const QString& deviceId) = 0;
	virtual void didReceiveSettings(const QJsonObject& settings, const QString& context, const QString& deviceId) = 0;
	virtual void didReceiveGlobalSettings(const QJsonObject& settings) = 0;
	virtual void sendToPlugin(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId) = 0;

signals:
	void setSettings(const QJsonObject& settings, const QString& context);
	void getSettings(const QString& context);
	void setGlobalSettings(const QJsonObject& settings, const QString& context);
	void getGlobalSettings(const QString& context);
	void openUrl(const QUrl& url);
	void setTitle(const QString& title, const QString& context, ESDSDKTarget target);
	void setImage(const QString& base64Image, const QString& context, ESDSDKTarget target);
	void showAlertForContext(const QString& context);
	void showOkForContext(const QString& context);
	void setState(int state, const QString& context);
	void sendToPropertyInspector(const QString& action, const QString& context, const QJsonObject& payload);
	void switchToProfile(const QString& deviceId, const QString& profileName);
	void logMessage(const QString& message);
};