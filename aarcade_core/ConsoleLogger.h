#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include <Ultralight/Ultralight.h>
#include <Ultralight/ConsoleMessage.h>
#include <windows.h>
#include <string>

using namespace ultralight;

/**
 * ConsoleLogger - Captures JavaScript console output from Ultralight views
 *
 * This class implements Ultralight's ViewListener interface to capture
 * console.log(), console.error(), console.warn(), etc. from JavaScript
 * and output them via OutputDebugString for debugging.
 */
class ConsoleLogger : public ViewListener {
public:
    ConsoleLogger();
    virtual ~ConsoleLogger();

    // ViewListener implementation - OnAddConsoleMessage
    virtual void OnAddConsoleMessage(ultralight::View* caller,
                                    const ultralight::ConsoleMessage& message) override;

private:
    void debugOutput(const std::string& message);
};

#endif // CONSOLELOGGER_H
