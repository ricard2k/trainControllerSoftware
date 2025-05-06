#include "DccExCommandManager.h"

void DccExCommandManager::connect(const String& connectionUrl) {
    // Add DccEx connection code here using the connectionUrl
}

void DccExCommandManager::disconnect() {
    // Add DccEx disconnection code here
}

void DccExCommandManager::sendCommand(const String& command) {
    // Add DccEx command handling code here
}

void DccExCommandManager::sendSpeedCommand(int speed) {
    // Add DccEx speed handling code here
}

void DccExCommandManager::sendBrakeCommand(int brake) {
    // Add DccEx brake handling code here
}

void DccExCommandManager::sendFrontLightsCommand(LightStatus status) {
    // Add DccEx front lights handling code here
}

void DccExCommandManager::sendBackLightsCommand(LightStatus status) {
    // Add DccEx back lights handling code here
}

void DccExCommandManager::sendBellCommand(bool active) {
    // Add DccEx bell handling code here
}

void DccExCommandManager::sendHornCommand(bool active) {
    // Add DccEx horn handling code here
}

String DccExCommandManager::lightStatusToString(LightStatus status) {
    switch (status) {
        case LightStatus::OFF:
            return "OFF";
        case LightStatus::DIMM:
            return "DIMM";
        case LightStatus::BRIGHT:
            return "BRIGHT";
        case LightStatus::DITCHES:
            return "DITCHES";
        default:
            return "UNKNOWN";
    }
}