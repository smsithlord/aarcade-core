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
#include "Library.h"
#include <memory>

using namespace ultralight;

class JSBridge {
private:
    SQLiteManager* dbManager_;
    ArcadeConfig* config_;
    ImageLoader* imageLoader_; // Weak pointer, owned by MainApp
    RefPtr<Renderer> renderer_; // Store renderer for future use
    RefPtr<App> app_; // Store app for quit functionality
    Library* library_; // Library manager for arcade functionality

public:
    // Constructor takes references to the managers it needs
    JSBridge(SQLiteManager* dbManager, ArcadeConfig* config, Library* library);
    virtual ~JSBridge();

    // Main entry point - called from MainApp::OnWindowObjectReady
    void setupJavaScriptBridge(View* view, uint64_t frame_id, bool is_main_frame, const String& url);

    // Setup JS bridge for image loader view
    void setupImageLoaderBridge(View* view);

    // Set the app (to access renderer)
    void setApp(RefPtr<App> app);

    // Set the image loader (owned by MainApp)
    void setImageLoader(ImageLoader* imageLoader);

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

    // Image loader bridge methods (called from image-loader.html)
    JSValueRef onImageLoaded(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef onImageLoaderReady(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Utility method
    JSValueRef getSupportedEntryTypes(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Schema construction
    JSValueRef constructSchema(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Application control methods
    JSValueRef quitApplication(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    // Database tools methods
    JSValueRef dbtFindLargeEntriesInTable(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtTrimTextFields(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtGetDatabaseStats(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtCompactDatabase(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtFindAnomalousInstances(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtGetInstanceKeyValues(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
        size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

    JSValueRef dbtRemoveAnomalousKeys(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
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

JSValueRef constructSchemaCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef onImageLoadedCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef onImageLoaderReadyCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

JSValueRef quitApplicationCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

#endif