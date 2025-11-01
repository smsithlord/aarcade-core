#include "JSBridge.h"
#include "ImageLoader.h"
#include "CurlImageDownloader.h"
#include "UltralightImageDownloader.h"
#include <windows.h>
#include <curl/curl.h>

// Static instance for callback access
static JSBridge* g_jsBridgeInstance = nullptr;

// C-style callback wrappers
JSValueRef debugTesterJointCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->debugTesterJoint(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeBoolean(ctx, false);
}

JSValueRef debugGetFirstItemAsJSObjectCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->debugGetFirstItemAsJSObject(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getFirstEntryCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getFirstEntry(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getNextEntryCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getNextEntry(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getFirstEntriesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getFirstEntries(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getNextEntriesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getNextEntries(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getFirstSearchResultsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getFirstSearchResults(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getNextSearchResultsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getNextSearchResults(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getCacheImageCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getCacheImage(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef processImageCompletionsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->processImageCompletions(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef getSupportedEntryTypesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->getSupportedEntryTypes(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSBridge::JSBridge(SQLiteManager* dbManager, ArcadeConfig* config)
    : dbManager_(dbManager), config_(config), renderer_(nullptr) {
    // Set this as the global instance
    setInstance(this);

    // Initialize curl globally (required for CurlImageDownloader)
    // curl_global_init(CURL_GLOBAL_DEFAULT);

    // ImageLoader will be initialized in setupJavaScriptBridge once we have access to the renderer

    // Get current working directory for debug output
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::string debugMsg = "[JSBridge] Current working directory: " + std::string(currentDir) + "\n";
    OutputDebugStringA(debugMsg.c_str());
}

JSBridge::~JSBridge() {
    // Clear the global instance
    if (g_jsBridgeInstance == this) {
        g_jsBridgeInstance = nullptr;
    }

    // ImageLoader cleanup is automatic with unique_ptr
    // This will also trigger downloader cleanup

    // Cleanup curl globally (only needed for CurlImageDownloader)
    // curl_global_cleanup();
}

void JSBridge::setApp(RefPtr<App> app) {
    if (app && !renderer_) {
        renderer_ = app->renderer();
        OutputDebugStringA("[JSBridge] Renderer obtained from App\n");
    }
}

void JSBridge::setupJavaScriptBridge(View* view, uint64_t frame_id, bool is_main_frame, const String& url) {
    if (!is_main_frame) return;

    // Initialize ImageLoader on first call when we have the renderer
    if (!imageLoader_ && renderer_) {
        // ============================================================================
        // IMAGE DOWNLOADER SWITCH - Change this to toggle between implementations
        // ============================================================================
        const bool USE_ULTRALIGHT_DOWNLOADER = true;  // Set to true for Ultralight, false for libcurl

        if (USE_ULTRALIGHT_DOWNLOADER) {
            // Ultralight: Renders images to 512x512 PNG thumbnails
            imageLoader_ = std::make_unique<ImageLoader>(std::make_unique<UltralightImageDownloader>(renderer_, 512, 512));
            OutputDebugStringA("[JSBridge] ImageLoader initialized with Ultralight downloader\n");
        } else {
            // libcurl: Direct downloads with automatic format detection
            imageLoader_ = std::make_unique<ImageLoader>(std::make_unique<CurlImageDownloader>());
            OutputDebugStringA("[JSBridge] ImageLoader initialized with libcurl downloader\n");
        }

        // Set up ImageLoader cache directory
        std::string cacheDir = ".\\cache\\urls";
        imageLoader_->setCacheDirectory(cacheDir);
    }

    // Acquire the JS execution context for the current page
    auto scoped_context = view->LockJSContext();
    JSContextRef ctx = (*scoped_context);

    // Get the global JavaScript object (aka 'window')
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Create the 'aapi' object
    JSObjectRef aapiObj = JSObjectMake(ctx, nullptr, nullptr);

    // Register debug methods
    JSStringRef methodName = JSStringCreateWithUTF8CString("debugTesterJoint");
    JSObjectRef methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, debugTesterJointCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("debugGetFirstItemAsJSObject");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, debugGetFirstItemAsJSObjectCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register entry browsing methods
    methodName = JSStringCreateWithUTF8CString("getFirstEntry");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getFirstEntryCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("getNextEntry");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getNextEntryCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("getFirstEntries");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getFirstEntriesCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("getNextEntries");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getNextEntriesCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register search methods
    methodName = JSStringCreateWithUTF8CString("getFirstSearchResults");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getFirstSearchResultsCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("getNextSearchResults");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getNextSearchResultsCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register image caching methods
    methodName = JSStringCreateWithUTF8CString("getCacheImage");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getCacheImageCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("processImageCompletions");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, processImageCompletionsCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register utility method
    methodName = JSStringCreateWithUTF8CString("getSupportedEntryTypes");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getSupportedEntryTypesCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Add the aapi object to the global object
    JSStringRef aapiName = JSStringCreateWithUTF8CString("aapi");
    JSObjectSetProperty(ctx, globalObj, aapiName, aapiObj, 0, 0);
    JSStringRelease(aapiName);

    // Debug output to confirm registration
    OutputDebugStringA("[JSBridge] JavaScript bridge registered with image caching support:\n");
    OutputDebugStringA("[JSBridge]   - aapi.debugTesterJoint\n");
    OutputDebugStringA("[JSBridge]   - aapi.debugGetFirstItemAsJSObject\n");
    OutputDebugStringA("[JSBridge]   - aapi.getFirstEntry\n");
    OutputDebugStringA("[JSBridge]   - aapi.getNextEntry\n");
    OutputDebugStringA("[JSBridge]   - aapi.getFirstEntries\n");
    OutputDebugStringA("[JSBridge]   - aapi.getNextEntries\n");
    OutputDebugStringA("[JSBridge]   - aapi.getFirstSearchResults\n");
    OutputDebugStringA("[JSBridge]   - aapi.getNextSearchResults\n");
    OutputDebugStringA("[JSBridge]   - aapi.getCacheImage\n");
    OutputDebugStringA("[JSBridge]   - aapi.processImageCompletions\n");
    OutputDebugStringA("[JSBridge]   - aapi.getSupportedEntryTypes\n");
}

// Helper function to convert Windows path to file:// URL
std::string JSBridge::convertToFileUrl(const std::string& filePath) {
    std::string fileUrl = "file:///";

    for (char c : filePath) {
        if (c == '\\') {
            fileUrl += '/';
        }
        else {
            fileUrl += c;
        }
    }

    return fileUrl;
}

JSValueRef JSBridge::getCacheImage(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getCacheImage called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] getCacheImage: Missing URL parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get URL from first argument
    JSStringRef urlStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!urlStr) {
        OutputDebugStringA("[JSBridge] getCacheImage: Invalid URL parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t urlLength = JSStringGetMaximumUTF8CStringSize(urlStr);
    char* urlBuffer = new char[urlLength];
    JSStringGetUTF8CString(urlStr, urlBuffer, urlLength);
    std::string url(urlBuffer);
    delete[] urlBuffer;
    JSStringRelease(urlStr);

    OutputDebugStringA(("[JSBridge] getCacheImage: Processing URL '" + url + "'\n").c_str());

    // Create a simple Promise-like object
    JSObjectRef promiseObj = JSObjectMake(ctx, nullptr, nullptr);

    // Create then method that stores callbacks
    JSStringRef thenStr = JSStringCreateWithUTF8CString("then");
    JSObjectRef thenFunc = JSObjectMakeFunctionWithCallback(ctx, thenStr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
            size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {

                if (argumentCount < 1) return JSValueMakeUndefined(ctx);

                // Get the promise object (stored as 'this')
                JSObjectRef promiseObj = thisObject;

                // Store the resolve callback
                JSStringRef resolveKey = JSStringCreateWithUTF8CString("_resolve");
                JSObjectSetProperty(ctx, promiseObj, resolveKey, arguments[0], 0, 0);
                JSStringRelease(resolveKey);

                // Store reject callback if provided
                if (argumentCount >= 2) {
                    JSStringRef rejectKey = JSStringCreateWithUTF8CString("_reject");
                    JSObjectSetProperty(ctx, promiseObj, rejectKey, arguments[1], 0, 0);
                    JSStringRelease(rejectKey);
                }

                return promiseObj;
        });

    JSObjectSetProperty(ctx, promiseObj, thenStr, thenFunc, 0, 0);
    JSStringRelease(thenStr);

    // Create catch method for error handling
    JSStringRef catchStr = JSStringCreateWithUTF8CString("catch");
    JSObjectRef catchFunc = JSObjectMakeFunctionWithCallback(ctx, catchStr,
        [](JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
            size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) -> JSValueRef {

                if (argumentCount < 1) return JSValueMakeUndefined(ctx);

                // Store the reject callback
                JSStringRef rejectKey = JSStringCreateWithUTF8CString("_reject");
                JSObjectSetProperty(ctx, thisObject, rejectKey, arguments[0], 0, 0);
                JSStringRelease(rejectKey);

                return thisObject;
        });

    JSObjectSetProperty(ctx, promiseObj, catchStr, catchFunc, 0, 0);
    JSStringRelease(catchStr);

    // Protect the promise object from garbage collection
    JSValueProtect(ctx, promiseObj);

    // Start the image loading process
    imageLoader_->loadImage(url, [this, ctx, promiseObj](const ImageLoadResult& result) {
        if (result.success) {
            // Convert to file:// URL
            std::string fileUrl = convertToFileUrl(result.filePath);
            OutputDebugStringA(("[JSBridge] Image cached successfully: " + fileUrl +
                              " (downloader: " + result.downloaderName + ")\n").c_str());

            // Get the resolve callback
            JSStringRef resolveKey = JSStringCreateWithUTF8CString("_resolve");
            JSValueRef resolveFunc = JSObjectGetProperty(ctx, promiseObj, resolveKey, nullptr);
            JSStringRelease(resolveKey);

            if (JSValueIsObject(ctx, resolveFunc)) {
                // Create result object with filePath and downloaderName
                JSObjectRef resultObj = JSObjectMake(ctx, nullptr, nullptr);

                // Set filePath property
                JSStringRef filePathKey = JSStringCreateWithUTF8CString("filePath");
                JSStringRef filePathStr = JSStringCreateWithUTF8CString(fileUrl.c_str());
                JSValueRef filePathValue = JSValueMakeString(ctx, filePathStr);
                JSObjectSetProperty(ctx, resultObj, filePathKey, filePathValue, 0, 0);
                JSStringRelease(filePathKey);
                JSStringRelease(filePathStr);

                // Set downloaderName property
                JSStringRef downloaderKey = JSStringCreateWithUTF8CString("downloaderName");
                JSStringRef downloaderStr = JSStringCreateWithUTF8CString(result.downloaderName.c_str());
                JSValueRef downloaderValue = JSValueMakeString(ctx, downloaderStr);
                JSObjectSetProperty(ctx, resultObj, downloaderKey, downloaderValue, 0, 0);
                JSStringRelease(downloaderKey);
                JSStringRelease(downloaderStr);

                // Call the resolve function with the result object
                JSValueRef args[] = { resultObj };
                JSObjectCallAsFunction(ctx, (JSObjectRef)resolveFunc, nullptr, 1, args, nullptr);
            }
        }
        else {
            OutputDebugStringA("[JSBridge] Image loading failed\n");

            // Get the reject callback
            JSStringRef rejectKey = JSStringCreateWithUTF8CString("_reject");
            JSValueRef rejectFunc = JSObjectGetProperty(ctx, promiseObj, rejectKey, nullptr);
            JSStringRelease(rejectKey);

            if (JSValueIsObject(ctx, rejectFunc)) {
                // Call the reject function with error message
                JSStringRef errorStr = JSStringCreateWithUTF8CString("Failed to load image");
                JSValueRef errorValue = JSValueMakeString(ctx, errorStr);
                JSStringRelease(errorStr);

                JSValueRef args[] = { errorValue };
                JSObjectCallAsFunction(ctx, (JSObjectRef)rejectFunc, nullptr, 1, args, nullptr);
            }
        }

        // Unprotect the promise object
        JSValueUnprotect(ctx, promiseObj);
        });

    return promiseObj;
}

JSValueRef JSBridge::processImageCompletions(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    // Process any pending completions from the worker thread
    imageLoader_->processCompletions();
    return JSValueMakeUndefined(ctx);
}

JSValueRef JSBridge::debugTesterJoint(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] debugTesterJoint called from JavaScript\n");
    OutputDebugStringA("=== SQLite Database Analysis Started ===\n");

    std::string versionMsg = "SQLite Version: " + dbManager_->getVersion();
    OutputDebugStringA((versionMsg + "\n").c_str());

    std::string dbPathMsg = "Database Path: " + config_->getDatabasePath();
    OutputDebugStringA((dbPathMsg + "\n").c_str());

    bool success = true;

    // Open or create database using config path
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[JSBridge] Failed to open database!\n");
        success = false;
    }

    if (success) {
        // Get general database information
        if (!dbManager_->getDatabaseInfo()) {
            OutputDebugStringA("[JSBridge] Failed to get database info!\n");
            success = false;
        }

        // List all tables in the database
        if (!dbManager_->listTables()) {
            OutputDebugStringA("[JSBridge] Failed to list tables!\n");
            success = false;
        }

        OutputDebugStringA("[JSBridge] Database analysis complete. Check output above for details.\n");
    }

    if (success) {
        OutputDebugStringA("=== SQLite Database Analysis Completed Successfully ===\n");
    }
    else {
        OutputDebugStringA("=== SQLite Database Analysis Failed ===\n");
    }

    // Return success status to JavaScript
    return JSValueMakeBoolean(ctx, success);
}

JSObjectRef JSBridge::arcadeKeyValuesToJSObject(JSContextRef ctx, const ArcadeKeyValues* kv) {
    if (!kv) {
        return JSObjectMake(ctx, nullptr, nullptr);
    }

    JSObjectRef jsObj = JSObjectMake(ctx, nullptr, nullptr);

    // Add all children as properties (no metadata)
    const auto& children = kv->GetChildren();
    for (const auto& pair : children) {
        const std::string& childName = pair.first;
        const std::unique_ptr<ArcadeKeyValues>& child = pair.second;

        JSStringRef childKey = JSStringCreateWithUTF8CString(childName.c_str());

        // If the child has its own children, recursively convert to JSObject
        if (child->GetChildCount() > 0) {
            JSObjectRef childObj = arcadeKeyValuesToJSObject(ctx, child.get());
            JSObjectSetProperty(ctx, jsObj, childKey, childObj, 0, 0);
        }
        else {
            // If it's a leaf node, add its value directly
            const char* childStringVal = child->GetString();
            if (childStringVal && strlen(childStringVal) > 0) {
                JSStringRef childValueStr = JSStringCreateWithUTF8CString(childStringVal);
                JSValueRef childValueJS = JSValueMakeString(ctx, childValueStr);
                JSObjectSetProperty(ctx, jsObj, childKey, childValueJS, 0, 0);
                JSStringRelease(childValueStr);
            }
            else {
                int childIntVal = child->GetInt();
                if (childIntVal != 0) {
                    JSValueRef childValueJS = JSValueMakeNumber(ctx, childIntVal);
                    JSObjectSetProperty(ctx, jsObj, childKey, childValueJS, 0, 0);
                }
                else {
                    float childFloatVal = child->GetFloat();
                    if (childFloatVal != 0.0f) {
                        JSValueRef childValueJS = JSValueMakeNumber(ctx, childFloatVal);
                        JSObjectSetProperty(ctx, jsObj, childKey, childValueJS, 0, 0);
                    }
                }
            }
        }

        JSStringRelease(childKey);
    }

    return jsObj;
}

JSObjectRef JSBridge::entryDataToJSObject(JSContextRef ctx, const std::string& entryId, const std::string& hexData) {
    if (entryId.empty() || hexData.empty()) {
        return JSValueToObject(ctx, JSValueMakeNull(ctx), nullptr);
    }

    // Parse the hex data using ArcadeKeyValues
    try {
        auto keyValues = ArcadeKeyValues::ParseFromHex(hexData);
        if (!keyValues) {
            return JSValueToObject(ctx, JSValueMakeNull(ctx), nullptr);
        }

        // For items table, try to navigate to item.local path for backwards compatibility
        ArcadeKeyValues* itemSection = keyValues->GetFirstSubKey();
        if (itemSection) {
            ArcadeKeyValues* localSection = itemSection->FindKey("local");
            if (localSection) {
                return arcadeKeyValuesToJSObject(ctx, localSection);
            }
        }

        // For other tables or if item.local doesn't exist, return the entire parsed structure
        return arcadeKeyValuesToJSObject(ctx, keyValues.get());

    }
    catch (const std::exception& e) {
        std::string errorMsg = "[JSBridge] Exception parsing KeyValues: " + std::string(e.what());
        OutputDebugStringA((errorMsg + "\n").c_str());
        return JSValueToObject(ctx, JSValueMakeNull(ctx), nullptr);
    }
}

JSObjectRef JSBridge::createJSArray(JSContextRef ctx, const std::vector<std::pair<std::string, std::string>>& entries) {
    JSObjectRef arrayObj = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (unsigned int i = 0; i < entries.size(); i++) {
        JSObjectRef entryObj = entryDataToJSObject(ctx, entries[i].first, entries[i].second);
        JSValueRef entryValue = static_cast<JSValueRef>(entryObj);
        JSObjectSetPropertyAtIndex(ctx, arrayObj, i, entryValue, nullptr);
    }

    return arrayObj;
}

JSObjectRef JSBridge::createStringArray(JSContextRef ctx, const std::vector<std::string>& strings) {
    JSObjectRef arrayObj = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (unsigned int i = 0; i < strings.size(); i++) {
        JSStringRef stringValue = JSStringCreateWithUTF8CString(strings[i].c_str());
        JSValueRef jsStringValue = JSValueMakeString(ctx, stringValue);
        JSObjectSetPropertyAtIndex(ctx, arrayObj, i, jsStringValue, nullptr);
        JSStringRelease(stringValue);
    }

    return arrayObj;
}

JSValueRef JSBridge::getSupportedEntryTypes(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getSupportedEntryTypes called from JavaScript\n");

    // Get supported types from SQLiteManager
    std::vector<std::string> supportedTypes = dbManager_->getSupportedEntryTypes();

    // Convert to JavaScript array
    return createStringArray(ctx, supportedTypes);
}

JSValueRef JSBridge::getFirstEntry(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getFirstEntry called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] getFirstEntry: Missing entry type parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get entry type from first argument
    JSStringRef entryTypeStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!entryTypeStr) {
        OutputDebugStringA("[JSBridge] getFirstEntry: Invalid entry type parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t entryTypeLength = JSStringGetMaximumUTF8CStringSize(entryTypeStr);
    char* entryTypeBuffer = new char[entryTypeLength];
    JSStringGetUTF8CString(entryTypeStr, entryTypeBuffer, entryTypeLength);
    std::string entryType(entryTypeBuffer);
    delete[] entryTypeBuffer;
    JSStringRelease(entryTypeStr);

    // Get search term from second argument
    JSStringRef searchTermStr = JSValueToStringCopy(ctx, arguments[1], exception);
    if (!searchTermStr) {
        OutputDebugStringA("[JSBridge] getFirstSearchResults: Invalid search term parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t searchTermLength = JSStringGetMaximumUTF8CStringSize(searchTermStr);
    char* searchTermBuffer = new char[searchTermLength];
    JSStringGetUTF8CString(searchTermStr, searchTermBuffer, searchTermLength);
    std::string searchTerm(searchTermBuffer);
    delete[] searchTermBuffer;
    JSStringRelease(searchTermStr);

    // Get count from third argument (optional, default to 50)
    int count = 50;
    if (argumentCount >= 3) {
        double countDouble = JSValueToNumber(ctx, arguments[2], exception);
        count = static_cast<int>(countDouble);
        if (count <= 0 || count > 1000) {
            count = 50; // Reset to default if invalid
        }
    }

    OutputDebugStringA(("[JSBridge] getFirstSearchResults: Searching '" + entryType + "' for '" + searchTerm + "' (count: " + std::to_string(count) + ")\n").c_str());

    // Open database if not already open
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[JSBridge] Failed to open database!\n");
        return JSValueMakeNull(ctx);
    }

    // Get the first search results
    std::vector<std::pair<std::string, std::string>> results = dbManager_->getFirstSearchResults(entryType, searchTerm, count);

    return createJSArray(ctx, results);
}

JSValueRef JSBridge::getNextSearchResults(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getNextSearchResults called from JavaScript\n");

    // Get count from first argument (optional, default to 50)
    int count = 50;
    if (argumentCount >= 1) {
        double countDouble = JSValueToNumber(ctx, arguments[0], exception);
        count = static_cast<int>(countDouble);
        if (count <= 0 || count > 1000) {
            count = 50; // Reset to default if invalid
        }
    }

    OutputDebugStringA(("[JSBridge] getNextSearchResults: Requesting " + std::to_string(count) + " more search results\n").c_str());

    // Get the next search results
    std::vector<std::pair<std::string, std::string>> results = dbManager_->getNextSearchResults(count);

    return createJSArray(ctx, results);
}

JSValueRef JSBridge::debugGetFirstItemAsJSObject(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] debugGetFirstItemAsJSObject called from JavaScript (legacy method)\n");

    // Open database if not already open
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[JSBridge] Failed to open database!\n");
        return JSValueMakeNull(ctx);
    }

    // Get the first item from the database (legacy method)
    std::pair<std::string, std::string> itemResult = dbManager_->getFirstItem();

    return entryDataToJSObject(ctx, itemResult.first, itemResult.second);
}

JSValueRef JSBridge::getNextEntry(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getNextEntry called from JavaScript\n");

    // Get the next entry (no parameters needed)
    std::pair<std::string, std::string> entry = dbManager_->getNextEntry();

    return entryDataToJSObject(ctx, entry.first, entry.second);
}

JSValueRef JSBridge::getFirstEntries(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getFirstEntries called from JavaScript\n");

    if (argumentCount < 2) {
        OutputDebugStringA("[JSBridge] getFirstEntries: Missing parameters (entryType, count)\n");
        return JSValueMakeNull(ctx);
    }

    // Get entry type from first argument
    JSStringRef entryTypeStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!entryTypeStr) {
        OutputDebugStringA("[JSBridge] getFirstEntries: Invalid entry type parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t entryTypeLength = JSStringGetMaximumUTF8CStringSize(entryTypeStr);
    char* entryTypeBuffer = new char[entryTypeLength];
    JSStringGetUTF8CString(entryTypeStr, entryTypeBuffer, entryTypeLength);
    std::string entryType(entryTypeBuffer);
    delete[] entryTypeBuffer;
    JSStringRelease(entryTypeStr);

    // Get count from second argument
    double countDouble = JSValueToNumber(ctx, arguments[1], exception);
    int count = static_cast<int>(countDouble);

    if (count <= 0 || count > 1000) { // Reasonable limit
        OutputDebugStringA("[JSBridge] getFirstEntries: Invalid count parameter\n");
        return JSValueMakeNull(ctx);
    }

    OutputDebugStringA(("[JSBridge] getFirstEntries: Requesting " + std::to_string(count) + " entries of type '" + entryType + "'\n").c_str());

    // Open database if not already open
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[JSBridge] Failed to open database!\n");
        return JSValueMakeNull(ctx);
    }

    // Get the first entries
    std::vector<std::pair<std::string, std::string>> entries = dbManager_->getFirstEntries(entryType, count);

    return createJSArray(ctx, entries);
}

JSValueRef JSBridge::getNextEntries(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getNextEntries called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] getNextEntries: Missing count parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get count from first argument
    double countDouble = JSValueToNumber(ctx, arguments[0], exception);
    int count = static_cast<int>(countDouble);

    if (count <= 0 || count > 1000) { // Reasonable limit
        OutputDebugStringA("[JSBridge] getNextEntries: Invalid count parameter\n");
        return JSValueMakeNull(ctx);
    }

    OutputDebugStringA(("[JSBridge] getNextEntries: Requesting " + std::to_string(count) + " more entries\n").c_str());

    // Get the next entries
    std::vector<std::pair<std::string, std::string>> entries = dbManager_->getNextEntries(count);

    return createJSArray(ctx, entries);
}

JSValueRef JSBridge::getFirstSearchResults(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getFirstSearchResults called from JavaScript\n");

    if (argumentCount < 2) {
        OutputDebugStringA("[JSBridge] getFirstSearchResults: Missing parameters (entryType, searchTerm)\n");
        return JSValueMakeNull(ctx);
    }

    // Get entry type from first argument
    JSStringRef entryTypeStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!entryTypeStr) {
        OutputDebugStringA("[JSBridge] getFirstSearchResults: Invalid entry type parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t entryTypeLength = JSStringGetMaximumUTF8CStringSize(entryTypeStr);
    char* entryTypeBuffer = new char[entryTypeLength];
    JSStringGetUTF8CString(entryTypeStr, entryTypeBuffer, entryTypeLength);
    std::string entryType(entryTypeBuffer);
    delete[] entryTypeBuffer;
    JSStringRelease(entryTypeStr);

    // Get search term from second argument
    JSStringRef searchTermStr = JSValueToStringCopy(ctx, arguments[1], exception);
    if (!searchTermStr) {
        OutputDebugStringA("[JSBridge] getFirstSearchResults: Invalid search term parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t searchTermLength = JSStringGetMaximumUTF8CStringSize(searchTermStr);
    char* searchTermBuffer = new char[searchTermLength];
    JSStringGetUTF8CString(searchTermStr, searchTermBuffer, searchTermLength);
    std::string searchTerm(searchTermBuffer);
    delete[] searchTermBuffer;
    JSStringRelease(searchTermStr);

    // Get count from third argument (optional, default to 50)
    int count = 50;
    if (argumentCount >= 3) {
        double countDouble = JSValueToNumber(ctx, arguments[2], exception);
        count = static_cast<int>(countDouble);
        if (count <= 0 || count > 1000) {
            count = 50; // Reset to default if invalid
        }
    }

    OutputDebugStringA(("[JSBridge] getFirstSearchResults: Searching '" + entryType + "' for '" + searchTerm + "' (count: " + std::to_string(count) + ")\n").c_str());

    // Open database if not already open
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[JSBridge] Failed to open database!\n");
        return JSValueMakeNull(ctx);
    }

    // Get the first search results
    std::vector<std::pair<std::string, std::string>> results = dbManager_->getFirstSearchResults(entryType, searchTerm, count);

    return createJSArray(ctx, results);
}

// Static instance management
JSBridge* JSBridge::getInstance() {
    return g_jsBridgeInstance;
}

void JSBridge::setInstance(JSBridge* instance) {
    g_jsBridgeInstance = instance;
}