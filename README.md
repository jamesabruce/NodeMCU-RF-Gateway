# NodeMCU-RF-Gateway
Control RF sockets and custom devices from OpenHAB and HomeAssistant

#Requirements
You'll need a working OpenHAB or Home Asssistance installation, and an MQTT server running. You can find my getting started guide to both here:
- [Getting started with OpenHAB](http://www.makeuseof.com/tag/getting-started-openhab-home-automation-raspberry-pi/)
- [OpenHAB guide part 2: MQTT and ZWave](http://www.makeuseof.com/tag/openhab-beginners-guide-part-2-zwave-mqtt-rules-charting/)

#Usage

- Place the mqtt_rcswitch folder into your Arduino user folder. 
- Put the included libraries into your arduino/libraries folder. Move existing MQTT/PubSubClient and RCSwitch libraries to a backup folder, as it took me ages to find a decent MQTT library which actually received commands. 

For basic RF socket control, the kind with dials to select channel and id, just modify the sketch to include your own WiFi details and MQTT server. Change the channel name if required. 

#MQTT Format
Basic socket control is performed by sending a payload of on or off to:
    livingroom/control/switch/X/Y
where X and Y are the channel and ID of the socket, eg.
    livingroom/control/switch/4/1 <- "on"

#OpenHAB Item Definition
    Switch Switch41 "Living room friendly socket name" <socket> (Cinema) {mqtt=">[broker:livingroom/control/switch/4/1:command:ON:on],>[broker:livingroom/control/switch/4/1:command:OFF:off]"}


#Additional Devices
Heavy modification will be needed for custom devices, but I've included a single one for a cheap project screen for you to work off as an example. 
