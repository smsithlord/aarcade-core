#ifndef JSBRIDGE_H
#define JSBRIDGE_H

#include <JavaScriptCore/JavaScript.h>
#include <AppCore/App.h>
#include <AppCore/Overlay.h>
#include <Ultralight/Renderer.h>
#include "SQLiteManager.h"
#include "Config.h"
#include "ArcadeKeyValues.h"
#include "ImageLoader.h"
#include <memory>

using namespace ultralight;

class JSBridge {
private:
    SQLiteManager* dbManager_;
    ArcadeConfig* config_;
    std::unique_ptr<ImageLoader> imageLoader_;
    RefPtr<Renderer> renderer_; // Store renderer for image downloader

public:
    // Constructor takes references to the managers it needs
    JSBridge(SQLiteManager* dbManager, ArcadeConfig* config);
    virtual ~JSBridge();

    // Main entry point - called from MainApp::OnWindowObjectReady
    void setupJavaScriptBridge(View* view, uint64_t frame_id, bool is_main_frame, const String& url);

    // Set the app (to access renderer)
    void setApp(RefPtr<App> app);

    std::string convertToFileUrl(const std::string& filePath);

    // JavaScript bridge methods (called by C callback wrappers)
    JSValueRef debugTesterJoint(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Legacy debug method (keep for backwards compatibility)
    JSValueRef debugGetFirstItemAsJSObject(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Entry browsing methods
    JSValueRef getFirstEntry(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef getNextEntry(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef getFirstEntries(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef getNextEntries(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Search methods
    JSValueRef getFirstSearchResults(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef getNextSearchResults(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Image caching methods
    JSValueRef getCacheImage(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef processImageCompletions(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Utility method
    JSValueRef getSupportedEntryTypes(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Helper functions
    JSObjectRef arcadeKeyValuesToJSObject(JSContextRef ctx, const ArcadeKeyValues* kv);
    JSObjectRef entryDataToJSObject(JSContextRef ctx, const std::string& entryId, const std::string& hexData);
    JSObjectRef createJSArray(JSContextRef ctx, const std::vector<std::pair<std::string, std::string>>& entries);
    JSObjectRef createStringArray(JSContextRef ctx, const std::vector<std::string>& strings);

    // Static instance getter for callbacks
    static JSBridge* getInstance();
    static void setInstance(JSBridge* instance);
};

// C-style callback wrappers (need to be global functions)
JSValueRef debugTesterJointCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef debugGetFirstItemAsJSObjectCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getFirstEntryCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getNextEntryCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getFirstEntriesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getNextEntriesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getFirstSearchResultsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getNextSearchResultsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getCacheImageCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef getSupportedEntryTypesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

#endif