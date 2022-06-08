# Elgato StreamDeck Qt C++ SDK
This SDK/Template can be used to develop StreamDeck plugins using the Qt Framework. I developed the SDK because I dont´t like the sources and examples provided by Elgato and to have a clean template for programming StreamDeck Plugins.

## How to start?
To develop your own plugin, just clone this repository and create a new class. Your plugin class has to inherit from `StreamDeckPlugin`. In the `main.cpp` look for:

    // create plugin instance here
	StreamDeckPlugin* plugin = nullptr;

Create your plugin instance there and you are done. Your plugin is automatically connected to the StreamDeck through the `StreamDeckProxy`.

## How it works?
Have a look at the StreamDeck Developer documentation from Elgato for further explanation of the plugin architecture: https://developer.elgato.com/documentation/stream-deck/sdk/overview/.

The `StreamDeckProxy` is used to establish a WebSocket connection to the StreamDeck Application. The `StreamDeckProxy` provides `Qt Signals` for the following StreamDeck Events: 
 - keyDown
 - keyUp
 - willAppear
 - willDisappear
 - titleParameterDidChange
 - deviceDidConnect
 - deviceDidDisconnect
 - applicationDidLaunch
 - applicationDidTerminate
 - systemDidWakeUp
 - propertyInspectorDidAppear
 - propertyInspectorDidDisappear
 - didReceiveSettings
 - didReceiveGlobalSettings
 - sendToPlugin

Further it provides the following `Qt Slots` to send data back to the StreamDeck:
 - setSettings
 - getSettings
 - setGlobalSettings
 - getGlobalSettings
 - openUrl
 - setTitle
 - setImage
 - showAlertForContext
 - showOkForContext
 - setState
 - sendToPropertyInspector
 - switchToProfile
 - logMessage

The `StreamDeckPlugin` also provides `Qt Signals` and `Qt Slots` which are connected to the `StreamDeckProxy`. To act on a specific StreamDeck event just implement the corresponding slot in your plugin. If you want to send data back to the StreamDeck just emit the corresponding signal.

# License

MIT licensed 2022 Cedrik Kaufmann. See the LICENSE file for further details.
