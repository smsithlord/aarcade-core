#include "ConsoleLogger.h"

ConsoleLogger::ConsoleLogger() {
    debugOutput("ConsoleLogger initialized");
}

ConsoleLogger::~ConsoleLogger() {
    debugOutput("ConsoleLogger destroyed");
}

void ConsoleLogger::OnAddConsoleMessage(ultralight::View* caller,
                                       const ultralight::ConsoleMessage& message) {
    std::string msg(message.message().utf8().data());
    std::string prefix;

    switch (message.level()) {
        case kMessageLevel_Error:
            prefix = "[JS ERROR] ";
            break;
        case kMessageLevel_Warning:
            prefix = "[JS WARN] ";
            break;
        case kMessageLevel_Info:
            prefix = "[JS INFO] ";
            break;
        case kMessageLevel_Debug:
            prefix = "[JS DEBUG] ";
            break;
        case kMessageLevel_Log:
        default:
            prefix = "[JS] ";
            break;
    }

    debugOutput(prefix + msg);
}

void ConsoleLogger::debugOutput(const std::string& message) {
    std::string debugMsg = "[ConsoleLogger] " + message;
    OutputDebugStringA((debugMsg + "\n").c_str());
}
