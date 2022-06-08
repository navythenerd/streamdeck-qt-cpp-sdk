#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>
#include <QUrl>

#include "ESDSDKDefines.h"
#include "StreamDeckPlugin.h"


class StreamDeckProxy : 
	public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StreamDeckProxy)

public:
	explicit StreamDeckProxy(uint port, const QString& event, const QString& uuid, QObject* parent = nullptr);

	void use(StreamDeckPlugin* plugin) const;
	bool isConnected() const;

public slots:
	void textMessageReceived(const QString& message);
	void connected();
	void disconnected();

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

signals:
	void keyDown(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void keyUp(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void willAppear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void willDisappear(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);
	void titleParameterDidChange(const QString& action, const QString& context, const QJsonObject& payload, QString& deviceId);
	void deviceDidConnect(const QString& deviceId, const QJsonObject& deviceInfo);
	void deviceDidDisconnect(const QString& deviceId);
	void applicationDidLaunch(const QJsonObject& payload);
	void applicationDidTerminate(const QJsonObject& payload);
	void systemDidWakeUp();
	void propertyInspectorDidAppear(const QString& context, const QString& deviceId);
	void propertyInspectorDidDisappear(const QString& context, const QString& deviceId);
	void didReceiveSettings(const QJsonObject& settings, const QString& context,  const QString& deviceId);
	void didReceiveGlobalSettings(const QJsonObject& settings);
	void sendToPlugin(const QString& action, const QString& context, const QJsonObject& payload, const QString& deviceId);

	void connectionStatusChanged(bool connected);

private:
	QWebSocket _socket;
	QString _registerEvent;
	QString _pluginUuid;
	bool _isConnected;
};

