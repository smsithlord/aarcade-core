#ifndef MAIN_APP
#define MAIN_APP
#include <AppCore/App.h>
#include <AppCore/Overlay.h>
#include <AppCore/Window.h>
#include "SQLiteManager.h"
#include "Config.h"
#include "JSBridge.h"

using namespace ultralight;

class MainApp : public WindowListener, public ViewListener, public LoadListener {
    RefPtr<App> app_;
    RefPtr<Window> window_;
    RefPtr<Overlay> overlay_;
    SQLiteManager dbManager_;
    ArcadeConfig config_;
    JSBridge jsBridge_;

public:
    MainApp();
    virtual ~MainApp();

    // WindowListener
    virtual void OnClose(ultralight::Window* window);
    virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height);

    // ViewListener
    virtual void OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor);

    // LoadListener
    virtual void OnBeginLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url);
    virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url);
    virtual void OnFailLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url, const String& description, const String& error_domain, int error_code);
    virtual void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url);
    virtual void OnWindowObjectReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url);

    void Run();
};
#endif