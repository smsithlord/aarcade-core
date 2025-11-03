#include "JSBridge.h"
#include "ImageLoader.h"
#include <windows.h>

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

JSValueRef constructSchemaCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->constructSchema(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef onImageLoadedCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->onImageLoaded(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef onImageLoaderReadyCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->onImageLoaderReady(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeUndefined(ctx);
}

JSValueRef quitApplicationCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->quitApplication(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeBoolean(ctx, false);
}

JSValueRef dbtFindLargeEntriesInTableCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtFindLargeEntriesInTable(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtTrimTextFieldsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtTrimTextFields(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtGetDatabaseStatsCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtGetDatabaseStats(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtCompactDatabaseCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtCompactDatabase(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtFindAnomalousInstancesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtFindAnomalousInstances(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtGetInstanceKeyValuesCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtGetInstanceKeyValues(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSValueRef dbtRemoveAnomalousKeysCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    JSBridge* bridge = JSBridge::getInstance();
    if (bridge) {
        return bridge->dbtRemoveAnomalousKeys(ctx, function, thisObject, argumentCount, arguments, exception);
    }
    return JSValueMakeNull(ctx);
}

JSBridge::JSBridge(SQLiteManager* dbManager, ArcadeConfig* config, Library* library)
    : dbManager_(dbManager), config_(config), library_(library), renderer_(nullptr), app_(nullptr), imageLoader_(nullptr) {
    // Set this as the global instance
    setInstance(this);

    // ImageLoader will be set by MainApp after initialization

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

    // ImageLoader is owned by MainApp, no cleanup needed here
}

void JSBridge::setApp(RefPtr<App> app) {
    if (app) {
        app_ = app;
        if (!renderer_) {
            renderer_ = app->renderer();
            OutputDebugStringA("[JSBridge] Renderer obtained from App\n");
        }
    }
}

void JSBridge::setImageLoader(ImageLoader* imageLoader) {
    imageLoader_ = imageLoader;
    OutputDebugStringA("[JSBridge] ImageLoader reference set\n");
}

void JSBridge::setupJavaScriptBridge(View* view, uint64_t frame_id, bool is_main_frame, const String& url) {
    if (!is_main_frame) return;

    // NOTE: ImageLoader is now initialized in MainApp after renderer is ready
    // It uses its own dedicated view for rendering images to cache

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

    // Register schema construction
    methodName = JSStringCreateWithUTF8CString("constructSchema");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, constructSchemaCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register application control methods
    methodName = JSStringCreateWithUTF8CString("quitApplication");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, quitApplicationCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register database tools methods
    methodName = JSStringCreateWithUTF8CString("dbtFindLargeEntriesInTable");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtFindLargeEntriesInTableCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtTrimTextFields");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtTrimTextFieldsCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtGetDatabaseStats");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtGetDatabaseStatsCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtCompactDatabase");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtCompactDatabaseCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtFindAnomalousInstances");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtFindAnomalousInstancesCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtGetInstanceKeyValues");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtGetInstanceKeyValuesCallback);
    JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    methodName = JSStringCreateWithUTF8CString("dbtRemoveAnomalousKeys");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, dbtRemoveAnomalousKeysCallback);
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
    OutputDebugStringA("[JSBridge]   - aapi.constructSchema\n");
    OutputDebugStringA("[JSBridge]   - aapi.dbtFindLargeEntriesInTable\n");
    OutputDebugStringA("[JSBridge]   - aapi.dbtTrimTextFields\n");
    OutputDebugStringA("[JSBridge]   - aapi.dbtGetDatabaseStats\n");
    OutputDebugStringA("[JSBridge]   - aapi.dbtCompactDatabase\n");
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

    // Start the image loading process via Library
    library_->cacheImage(url, [this, ctx, promiseObj](const ImageLoadResult& result) {
        if (result.success) {
            // Convert to file:// URL
            std::string fileUrl = convertToFileUrl(result.filePath);
            OutputDebugStringA(("[JSBridge] Image cached successfully: " + fileUrl + "\n").c_str());

            // Get the resolve callback
            JSStringRef resolveKey = JSStringCreateWithUTF8CString("_resolve");
            JSValueRef resolveFunc = JSObjectGetProperty(ctx, promiseObj, resolveKey, nullptr);
            JSStringRelease(resolveKey);

            if (JSValueIsObject(ctx, resolveFunc)) {
                // Create result object with filePath
                JSObjectRef resultObj = JSObjectMake(ctx, nullptr, nullptr);

                // Set filePath property
                JSStringRef filePathKey = JSStringCreateWithUTF8CString("filePath");
                JSStringRef filePathStr = JSStringCreateWithUTF8CString(fileUrl.c_str());
                JSValueRef filePathValue = JSValueMakeString(ctx, filePathStr);
                JSObjectSetProperty(ctx, resultObj, filePathKey, filePathValue, 0, 0);
                JSStringRelease(filePathKey);
                JSStringRelease(filePathStr);

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
    // Process any pending completions from the worker thread via Library
    library_->processImageCompletions();
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

    // Get supported types from Library
    std::vector<std::string> supportedTypes = library_->getSupportedEntryTypes();

    // Convert to JavaScript array
    return createStringArray(ctx, supportedTypes);
}

JSValueRef JSBridge::constructSchema(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] constructSchema called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] constructSchema: Missing entryType parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get entry type from first argument
    JSStringRef entryTypeStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!entryTypeStr) {
        OutputDebugStringA("[JSBridge] constructSchema: Invalid entry type parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t entryTypeLength = JSStringGetMaximumUTF8CStringSize(entryTypeStr);
    char* entryTypeBuffer = new char[entryTypeLength];
    JSStringGetUTF8CString(entryTypeStr, entryTypeBuffer, entryTypeLength);
    std::string entryType = entryTypeBuffer;
    delete[] entryTypeBuffer;
    JSStringRelease(entryTypeStr);

    // Get schema from Library
    std::vector<std::string> schema = library_->constructSchema(entryType);

    // Convert to JavaScript array
    return createStringArray(ctx, schema);
}

JSValueRef JSBridge::quitApplication(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] quitApplication called from JavaScript\n");

    if (app_) {
        app_->Quit();
        OutputDebugStringA("[JSBridge] Application quit requested\n");
        return JSValueMakeBoolean(ctx, true);
    }
    else {
        OutputDebugStringA("[JSBridge] ERROR: App instance not available\n");
        return JSValueMakeBoolean(ctx, false);
    }
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

    // Get the first entry via Library
    std::pair<std::string, std::string> entry = library_->getFirstEntry(entryType);

    return entryDataToJSObject(ctx, entry.first, entry.second);
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

    // Get the next search results via Library
    std::vector<std::pair<std::string, std::string>> results = library_->getNextSearchResults(count);

    return createJSArray(ctx, results);
}

JSValueRef JSBridge::debugGetFirstItemAsJSObject(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] debugGetFirstItemAsJSObject called from JavaScript (legacy method)\n");

    // Get the first item from the database via Library (legacy method)
    std::pair<std::string, std::string> itemResult = library_->getFirstItem();

    return entryDataToJSObject(ctx, itemResult.first, itemResult.second);
}

JSValueRef JSBridge::getNextEntry(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] getNextEntry called from JavaScript\n");

    // Get the next entry via Library (no parameters needed)
    std::pair<std::string, std::string> entry = library_->getNextEntry();

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

    // Get the first entries via Library
    std::vector<std::pair<std::string, std::string>> entries = library_->getFirstEntries(entryType, count);

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

    // Get the next entries via Library
    std::vector<std::pair<std::string, std::string>> entries = library_->getNextEntries(count);

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

    // Get the first search results via Library
    std::vector<std::pair<std::string, std::string>> results = library_->getFirstSearchResults(entryType, searchTerm, count);

    return createJSArray(ctx, results);
}

JSValueRef JSBridge::dbtFindLargeEntriesInTable(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtFindLargeEntriesInTable called from JavaScript\n");

    if (argumentCount < 2) {
        OutputDebugStringA("[JSBridge] dbtFindLargeEntriesInTable: Missing parameters (tableName, minSizeBytes)\n");
        return JSValueMakeNull(ctx);
    }

    // Get table name from first argument
    JSStringRef tableNameStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!tableNameStr) {
        OutputDebugStringA("[JSBridge] dbtFindLargeEntriesInTable: Invalid table name parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t tableNameLength = JSStringGetMaximumUTF8CStringSize(tableNameStr);
    char* tableNameBuffer = new char[tableNameLength];
    JSStringGetUTF8CString(tableNameStr, tableNameBuffer, tableNameLength);
    std::string tableName(tableNameBuffer);
    delete[] tableNameBuffer;
    JSStringRelease(tableNameStr);

    // Get minimum size from second argument
    double minSizeBytesDouble = JSValueToNumber(ctx, arguments[1], exception);
    int minSizeBytes = static_cast<int>(minSizeBytesDouble);

    if (minSizeBytes <= 0) {
        OutputDebugStringA("[JSBridge] dbtFindLargeEntriesInTable: Invalid minSizeBytes parameter\n");
        return JSValueMakeNull(ctx);
    }

    OutputDebugStringA(("[JSBridge] Searching " + tableName + " for entries over " + std::to_string(minSizeBytes) + " bytes\n").c_str());

    // Get large entries from Library
    std::vector<Library::LargeBlobEntry> entries = library_->dbtFindLargeEntriesInTable(tableName, minSizeBytes);

    OutputDebugStringA(("[JSBridge] Found " + std::to_string(entries.size()) + " large entries\n").c_str());

    // Convert to JavaScript array of objects
    JSObjectRef resultsArray = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (size_t i = 0; i < entries.size(); i++) {
        JSObjectRef entryObj = JSObjectMake(ctx, nullptr, nullptr);

        // Set id property
        JSStringRef idKey = JSStringCreateWithUTF8CString("id");
        JSStringRef idValue = JSStringCreateWithUTF8CString(entries[i].id.c_str());
        JSObjectSetProperty(ctx, entryObj, idKey, JSValueMakeString(ctx, idValue), 0, nullptr);
        JSStringRelease(idKey);
        JSStringRelease(idValue);

        // Set title property
        JSStringRef titleKey = JSStringCreateWithUTF8CString("title");
        JSStringRef titleValue = JSStringCreateWithUTF8CString(entries[i].title.c_str());
        JSObjectSetProperty(ctx, entryObj, titleKey, JSValueMakeString(ctx, titleValue), 0, nullptr);
        JSStringRelease(titleKey);
        JSStringRelease(titleValue);

        // Set sizeBytes property
        JSStringRef sizeKey = JSStringCreateWithUTF8CString("sizeBytes");
        JSObjectSetProperty(ctx, entryObj, sizeKey, JSValueMakeNumber(ctx, entries[i].sizeBytes), 0, nullptr);
        JSStringRelease(sizeKey);

        // Add to array
        JSObjectSetPropertyAtIndex(ctx, resultsArray, i, entryObj, nullptr);
    }

    return resultsArray;
}

JSValueRef JSBridge::dbtTrimTextFields(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtTrimTextFields called from JavaScript\n");

    if (argumentCount < 3) {
        OutputDebugStringA("[JSBridge] dbtTrimTextFields: Missing parameters (tableName, entryIds, maxLength)\n");
        return JSValueMakeNull(ctx);
    }

    // Get table name from first argument
    JSStringRef tableNameStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!tableNameStr) {
        OutputDebugStringA("[JSBridge] dbtTrimTextFields: Invalid table name parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t tableNameLength = JSStringGetMaximumUTF8CStringSize(tableNameStr);
    char* tableNameBuffer = new char[tableNameLength];
    JSStringGetUTF8CString(tableNameStr, tableNameBuffer, tableNameLength);
    std::string tableName(tableNameBuffer);
    delete[] tableNameBuffer;
    JSStringRelease(tableNameStr);

    // Get entryIds array from second argument
    JSObjectRef entryIdsArray = JSValueToObject(ctx, arguments[1], exception);
    if (!entryIdsArray) {
        OutputDebugStringA("[JSBridge] dbtTrimTextFields: Invalid entryIds parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get array length
    JSStringRef lengthProp = JSStringCreateWithUTF8CString("length");
    JSValueRef lengthValue = JSObjectGetProperty(ctx, entryIdsArray, lengthProp, exception);
    JSStringRelease(lengthProp);
    double lengthDouble = JSValueToNumber(ctx, lengthValue, exception);
    size_t arrayLength = static_cast<size_t>(lengthDouble);

    // Extract entry IDs from array
    std::vector<std::string> entryIds;
    for (size_t i = 0; i < arrayLength; i++) {
        JSValueRef idValue = JSObjectGetPropertyAtIndex(ctx, entryIdsArray, i, exception);
        JSStringRef idStr = JSValueToStringCopy(ctx, idValue, exception);
        if (idStr) {
            size_t idLength = JSStringGetMaximumUTF8CStringSize(idStr);
            char* idBuffer = new char[idLength];
            JSStringGetUTF8CString(idStr, idBuffer, idLength);
            entryIds.push_back(std::string(idBuffer));
            delete[] idBuffer;
            JSStringRelease(idStr);
        }
    }

    // Get max length from third argument
    double maxLengthDouble = JSValueToNumber(ctx, arguments[2], exception);
    int maxLength = static_cast<int>(maxLengthDouble);

    if (maxLength <= 0) {
        OutputDebugStringA("[JSBridge] dbtTrimTextFields: Invalid maxLength parameter\n");
        return JSValueMakeNull(ctx);
    }

    OutputDebugStringA(("[JSBridge] Trimming text fields for " + std::to_string(entryIds.size()) + " entries to max " + std::to_string(maxLength) + " chars\n").c_str());

    // Call Library method
    std::vector<Library::TrimResult> results = library_->dbtTrimTextFields(tableName, entryIds, maxLength);

    OutputDebugStringA(("[JSBridge] Trim operation completed for " + std::to_string(results.size()) + " entries\n").c_str());

    // Convert to JavaScript array of objects
    JSObjectRef resultsArray = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (size_t i = 0; i < results.size(); i++) {
        JSObjectRef resultObj = JSObjectMake(ctx, nullptr, nullptr);

        // Set id property
        JSStringRef idKey = JSStringCreateWithUTF8CString("id");
        JSStringRef idValue = JSStringCreateWithUTF8CString(results[i].id.c_str());
        JSObjectSetProperty(ctx, resultObj, idKey, JSValueMakeString(ctx, idValue), 0, nullptr);
        JSStringRelease(idKey);
        JSStringRelease(idValue);

        // Set success property
        JSStringRef successKey = JSStringCreateWithUTF8CString("success");
        JSObjectSetProperty(ctx, resultObj, successKey, JSValueMakeBoolean(ctx, results[i].success), 0, nullptr);
        JSStringRelease(successKey);

        // Set error property
        JSStringRef errorKey = JSStringCreateWithUTF8CString("error");
        JSStringRef errorValue = JSStringCreateWithUTF8CString(results[i].error.c_str());
        JSObjectSetProperty(ctx, resultObj, errorKey, JSValueMakeString(ctx, errorValue), 0, nullptr);
        JSStringRelease(errorKey);
        JSStringRelease(errorValue);

        // Add to array
        JSObjectSetPropertyAtIndex(ctx, resultsArray, i, resultObj, nullptr);
    }

    return resultsArray;
}

JSValueRef JSBridge::dbtGetDatabaseStats(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtGetDatabaseStats called from JavaScript\n");

    // Get database stats from Library
    Library::DatabaseStats stats = library_->dbtGetDatabaseStats();

    OutputDebugStringA(("[JSBridge] Stats received: fragmentationPercent=" + std::to_string(stats.fragmentationPercent) +
                       ", freePages=" + std::to_string(stats.freePages) +
                       ", pageCount=" + std::to_string(stats.pageCount) + "\n").c_str());

    // Convert to JavaScript object
    JSObjectRef statsObj = JSObjectMake(ctx, nullptr, nullptr);

    // Set filePath property
    JSStringRef filePathKey = JSStringCreateWithUTF8CString("filePath");
    JSStringRef filePathValue = JSStringCreateWithUTF8CString(stats.filePath.c_str());
    JSObjectSetProperty(ctx, statsObj, filePathKey, JSValueMakeString(ctx, filePathValue), 0, nullptr);
    JSStringRelease(filePathKey);
    JSStringRelease(filePathValue);

    // Set fileSizeBytes property
    JSStringRef fileSizeBytesKey = JSStringCreateWithUTF8CString("fileSizeBytes");
    JSObjectSetProperty(ctx, statsObj, fileSizeBytesKey, JSValueMakeNumber(ctx, stats.fileSizeBytes), 0, nullptr);
    JSStringRelease(fileSizeBytesKey);

    // Set pageCount property
    JSStringRef pageCountKey = JSStringCreateWithUTF8CString("pageCount");
    JSObjectSetProperty(ctx, statsObj, pageCountKey, JSValueMakeNumber(ctx, stats.pageCount), 0, nullptr);
    JSStringRelease(pageCountKey);

    // Set pageSize property
    JSStringRef pageSizeKey = JSStringCreateWithUTF8CString("pageSize");
    JSObjectSetProperty(ctx, statsObj, pageSizeKey, JSValueMakeNumber(ctx, stats.pageSize), 0, nullptr);
    JSStringRelease(pageSizeKey);

    // Set freePages property
    JSStringRef freePagesKey = JSStringCreateWithUTF8CString("freePages");
    JSObjectSetProperty(ctx, statsObj, freePagesKey, JSValueMakeNumber(ctx, stats.freePages), 0, nullptr);
    JSStringRelease(freePagesKey);

    // Set fragmentationPercent property
    JSStringRef fragmentationPercentKey = JSStringCreateWithUTF8CString("fragmentationPercent");
    JSObjectSetProperty(ctx, statsObj, fragmentationPercentKey, JSValueMakeNumber(ctx, stats.fragmentationPercent), 0, nullptr);
    JSStringRelease(fragmentationPercentKey);

    return statsObj;
}

JSValueRef JSBridge::dbtCompactDatabase(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtCompactDatabase called from JavaScript\n");

    // Call Library method
    Library::CompactResult result = library_->dbtCompactDatabase();

    OutputDebugStringA(("[JSBridge] Compact result: success=" + std::string(result.success ? "true" : "false") +
                       ", saved=" + std::to_string(result.spaceSavedBytes) + " bytes\n").c_str());

    // Convert to JavaScript object
    JSObjectRef resultObj = JSObjectMake(ctx, nullptr, nullptr);

    // Set success property
    JSStringRef successKey = JSStringCreateWithUTF8CString("success");
    JSObjectSetProperty(ctx, resultObj, successKey, JSValueMakeBoolean(ctx, result.success), 0, nullptr);
    JSStringRelease(successKey);

    // Set error property
    JSStringRef errorKey = JSStringCreateWithUTF8CString("error");
    JSStringRef errorValue = JSStringCreateWithUTF8CString(result.error.c_str());
    JSObjectSetProperty(ctx, resultObj, errorKey, JSValueMakeString(ctx, errorValue), 0, nullptr);
    JSStringRelease(errorKey);
    JSStringRelease(errorValue);

    // Set beforeSizeBytes property
    JSStringRef beforeSizeBytesKey = JSStringCreateWithUTF8CString("beforeSizeBytes");
    JSObjectSetProperty(ctx, resultObj, beforeSizeBytesKey, JSValueMakeNumber(ctx, result.beforeSizeBytes), 0, nullptr);
    JSStringRelease(beforeSizeBytesKey);

    // Set afterSizeBytes property
    JSStringRef afterSizeBytesKey = JSStringCreateWithUTF8CString("afterSizeBytes");
    JSObjectSetProperty(ctx, resultObj, afterSizeBytesKey, JSValueMakeNumber(ctx, result.afterSizeBytes), 0, nullptr);
    JSStringRelease(afterSizeBytesKey);

    // Set spaceSavedBytes property
    JSStringRef spaceSavedBytesKey = JSStringCreateWithUTF8CString("spaceSavedBytes");
    JSObjectSetProperty(ctx, resultObj, spaceSavedBytesKey, JSValueMakeNumber(ctx, result.spaceSavedBytes), 0, nullptr);
    JSStringRelease(spaceSavedBytesKey);

    return resultObj;
}

JSValueRef JSBridge::dbtFindAnomalousInstances(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtFindAnomalousInstances called from JavaScript\n");

    // Call Library method
    std::vector<Library::AnomalousInstanceEntry> entries = library_->dbtFindAnomalousInstances();

    OutputDebugStringA(("[JSBridge] Found " + std::to_string(entries.size()) + " anomalous instances\n").c_str());

    // Convert to JavaScript array of objects
    JSObjectRef resultsArray = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (size_t i = 0; i < entries.size(); i++) {
        const auto& entry = entries[i];

        // Create object for this entry
        JSObjectRef entryObj = JSObjectMake(ctx, nullptr, nullptr);

        // Set id property
        JSStringRef idKey = JSStringCreateWithUTF8CString("id");
        JSStringRef idValue = JSStringCreateWithUTF8CString(entry.id.c_str());
        JSObjectSetProperty(ctx, entryObj, idKey, JSValueMakeString(ctx, idValue), 0, nullptr);
        JSStringRelease(idKey);
        JSStringRelease(idValue);

        // Set unexpectedKeys property (array of strings)
        JSObjectRef unexpectedKeysArray = JSObjectMakeArray(ctx, 0, nullptr, nullptr);
        for (size_t j = 0; j < entry.unexpectedKeys.size(); j++) {
            JSStringRef keyStr = JSStringCreateWithUTF8CString(entry.unexpectedKeys[j].c_str());
            JSValueRef keyValue = JSValueMakeString(ctx, keyStr);
            JSObjectSetPropertyAtIndex(ctx, unexpectedKeysArray, j, keyValue, nullptr);
            JSStringRelease(keyStr);
        }

        JSStringRef unexpectedKeysKey = JSStringCreateWithUTF8CString("unexpectedKeys");
        JSObjectSetProperty(ctx, entryObj, unexpectedKeysKey, unexpectedKeysArray, 0, nullptr);
        JSStringRelease(unexpectedKeysKey);

        // Set keyCount property
        JSStringRef keyCountKey = JSStringCreateWithUTF8CString("keyCount");
        JSObjectSetProperty(ctx, entryObj, keyCountKey, JSValueMakeNumber(ctx, entry.keyCount), 0, nullptr);
        JSStringRelease(keyCountKey);

        // Set generation property (integer)
        JSStringRef generationKey = JSStringCreateWithUTF8CString("generation");
        JSObjectSetProperty(ctx, entryObj, generationKey, JSValueMakeNumber(ctx, entry.generation), 0, nullptr);
        JSStringRelease(generationKey);

        // Set legacy property (integer)
        JSStringRef legacyKey = JSStringCreateWithUTF8CString("legacy");
        JSObjectSetProperty(ctx, entryObj, legacyKey, JSValueMakeNumber(ctx, entry.legacy), 0, nullptr);
        JSStringRelease(legacyKey);

        // Add to results array
        JSObjectSetPropertyAtIndex(ctx, resultsArray, i, entryObj, nullptr);
    }

    return resultsArray;
}

JSValueRef JSBridge::dbtGetInstanceKeyValues(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtGetInstanceKeyValues called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] dbtGetInstanceKeyValues: Missing parameter (instanceId)\n");
        return JSValueMakeNull(ctx);
    }

    // Get instance ID from first argument
    JSStringRef instanceIdStr = JSValueToStringCopy(ctx, arguments[0], exception);
    if (!instanceIdStr) {
        OutputDebugStringA("[JSBridge] dbtGetInstanceKeyValues: Invalid instance ID parameter\n");
        return JSValueMakeNull(ctx);
    }

    size_t instanceIdLength = JSStringGetMaximumUTF8CStringSize(instanceIdStr);
    char* instanceIdBuffer = new char[instanceIdLength];
    JSStringGetUTF8CString(instanceIdStr, instanceIdBuffer, instanceIdLength);
    std::string instanceId(instanceIdBuffer);
    delete[] instanceIdBuffer;
    JSStringRelease(instanceIdStr);

    OutputDebugStringA(("[JSBridge] Fetching KeyValues for instance " + instanceId + "\n").c_str());

    // Call Library method
    std::string plainText = library_->dbtGetInstanceKeyValues(instanceId);

    OutputDebugStringA(("[JSBridge] Retrieved KeyValues plain text (" + std::to_string(plainText.length()) + " chars)\n").c_str());

    // Convert to JavaScript string
    JSStringRef resultStr = JSStringCreateWithUTF8CString(plainText.c_str());
    JSValueRef result = JSValueMakeString(ctx, resultStr);
    JSStringRelease(resultStr);

    return result;
}

JSValueRef JSBridge::dbtRemoveAnomalousKeys(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] dbtRemoveAnomalousKeys called from JavaScript\n");

    if (argumentCount < 1) {
        OutputDebugStringA("[JSBridge] dbtRemoveAnomalousKeys: Missing parameter (instanceIds array)\n");
        return JSValueMakeNull(ctx);
    }

    // Get instance IDs array from first argument
    JSObjectRef entryIdsArray = JSValueToObject(ctx, arguments[0], exception);
    if (!entryIdsArray) {
        OutputDebugStringA("[JSBridge] dbtRemoveAnomalousKeys: Invalid instanceIds array parameter\n");
        return JSValueMakeNull(ctx);
    }

    // Get array length
    JSStringRef lengthProp = JSStringCreateWithUTF8CString("length");
    JSValueRef lengthValue = JSObjectGetProperty(ctx, entryIdsArray, lengthProp, exception);
    JSStringRelease(lengthProp);
    double arrayLength = JSValueToNumber(ctx, lengthValue, exception);

    // Extract instance IDs from array
    std::vector<std::string> instanceIds;
    for (size_t i = 0; i < arrayLength; i++) {
        JSValueRef idValue = JSObjectGetPropertyAtIndex(ctx, entryIdsArray, i, exception);
        JSStringRef idStr = JSValueToStringCopy(ctx, idValue, exception);
        if (idStr) {
            size_t idLength = JSStringGetMaximumUTF8CStringSize(idStr);
            char* idBuffer = new char[idLength];
            JSStringGetUTF8CString(idStr, idBuffer, idLength);
            instanceIds.push_back(std::string(idBuffer));
            delete[] idBuffer;
            JSStringRelease(idStr);
        }
    }

    OutputDebugStringA(("[JSBridge] Removing anomalous keys from " + std::to_string(instanceIds.size()) + " instances\n").c_str());

    // Call Library method
    std::vector<Library::RemoveKeysResult> results = library_->dbtRemoveAnomalousKeys(instanceIds);

    OutputDebugStringA(("[JSBridge] Remove operation completed for " + std::to_string(results.size()) + " instances\n").c_str());

    // Convert to JavaScript array of objects
    JSObjectRef resultsArray = JSObjectMakeArray(ctx, 0, nullptr, nullptr);

    for (size_t i = 0; i < results.size(); i++) {
        const auto& result = results[i];

        // Create object for this result
        JSObjectRef resultObj = JSObjectMake(ctx, nullptr, nullptr);

        // Set id property
        JSStringRef idKey = JSStringCreateWithUTF8CString("id");
        JSStringRef idValue = JSStringCreateWithUTF8CString(result.id.c_str());
        JSObjectSetProperty(ctx, resultObj, idKey, JSValueMakeString(ctx, idValue), 0, nullptr);
        JSStringRelease(idKey);
        JSStringRelease(idValue);

        // Set success property
        JSStringRef successKey = JSStringCreateWithUTF8CString("success");
        JSObjectSetProperty(ctx, resultObj, successKey, JSValueMakeBoolean(ctx, result.success), 0, nullptr);
        JSStringRelease(successKey);

        // Set error property
        JSStringRef errorKey = JSStringCreateWithUTF8CString("error");
        JSStringRef errorValue = JSStringCreateWithUTF8CString(result.error.c_str());
        JSObjectSetProperty(ctx, resultObj, errorKey, JSValueMakeString(ctx, errorValue), 0, nullptr);
        JSStringRelease(errorKey);
        JSStringRelease(errorValue);

        // Add to results array
        JSObjectSetPropertyAtIndex(ctx, resultsArray, i, resultObj, nullptr);
    }

    return resultsArray;
}

// Setup JS bridge for image loader view
void JSBridge::setupImageLoaderBridge(View* view) {
    OutputDebugStringA("[JSBridge] Setting up image loader JS bridge\n");

    // Acquire the JS execution context
    auto scoped_context = view->LockJSContext();
    JSContextRef ctx = (*scoped_context);

    // Get the global JavaScript object (aka 'window')
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Create the 'cppBridge' object for image loader
    JSObjectRef bridgeObj = JSObjectMake(ctx, nullptr, nullptr);

    // Register onImageLoaded method
    JSStringRef methodName = JSStringCreateWithUTF8CString("onImageLoaded");
    JSObjectRef methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, onImageLoadedCallback);
    JSObjectSetProperty(ctx, bridgeObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Register onImageLoaderReady method
    methodName = JSStringCreateWithUTF8CString("onImageLoaderReady");
    methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, onImageLoaderReadyCallback);
    JSObjectSetProperty(ctx, bridgeObj, methodName, methodFunc, 0, 0);
    JSStringRelease(methodName);

    // Add the cppBridge object to the global object
    JSStringRef bridgeName = JSStringCreateWithUTF8CString("cppBridge");
    JSObjectSetProperty(ctx, globalObj, bridgeName, bridgeObj, 0, 0);
    JSStringRelease(bridgeName);

    OutputDebugStringA("[JSBridge] Image loader JS bridge registered:\n");
    OutputDebugStringA("[JSBridge]   - window.cppBridge.onImageLoaded\n");
    OutputDebugStringA("[JSBridge]   - window.cppBridge.onImageLoaderReady\n");
}

// Image loader bridge method implementations
JSValueRef JSBridge::onImageLoaded(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] onImageLoaded called from image-loader.html\n");
    OutputDebugStringA(("[JSBridge] Received " + std::to_string(argumentCount) + " arguments\n").c_str());

    if (argumentCount < 6) {
        OutputDebugStringA("[JSBridge] onImageLoaded: Missing parameters (success, url, x, y, width, height)\n");
        return JSValueMakeUndefined(ctx);
    }

    // Get success from first argument
    bool success = JSValueToBoolean(ctx, arguments[0]);

    // Get URL from second argument
    JSStringRef urlStr = JSValueToStringCopy(ctx, arguments[1], exception);
    if (!urlStr) {
        OutputDebugStringA("[JSBridge] onImageLoaded: Invalid URL parameter\n");
        return JSValueMakeUndefined(ctx);
    }

    size_t urlLength = JSStringGetMaximumUTF8CStringSize(urlStr);
    char* urlBuffer = new char[urlLength];
    JSStringGetUTF8CString(urlStr, urlBuffer, urlLength);
    std::string url(urlBuffer);
    delete[] urlBuffer;
    JSStringRelease(urlStr);

    // Get rect parameters (x, y, width, height)
    int rectX = (int)JSValueToNumber(ctx, arguments[2], exception);
    int rectY = (int)JSValueToNumber(ctx, arguments[3], exception);
    int rectWidth = (int)JSValueToNumber(ctx, arguments[4], exception);
    int rectHeight = (int)JSValueToNumber(ctx, arguments[5], exception);

    OutputDebugStringA(("[JSBridge] Image loaded: " + url + " (success: " + (success ? "true" : "false") + ")\n").c_str());
    OutputDebugStringA(("[JSBridge] Rect: (" + std::to_string(rectX) + ", " + std::to_string(rectY) + ", " +
                       std::to_string(rectWidth) + "x" + std::to_string(rectHeight) + ")\n").c_str());

    // Call the image loader
    if (imageLoader_) {
        imageLoader_->onImageLoaded(success, url, rectX, rectY, rectWidth, rectHeight);
    } else {
        OutputDebugStringA("[JSBridge] ERROR: ImageLoader not initialized!\n");
    }

    return JSValueMakeUndefined(ctx);
}

JSValueRef JSBridge::onImageLoaderReady(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    OutputDebugStringA("[JSBridge] onImageLoaderReady called from image-loader.html\n");

    // Call the image loader
    if (imageLoader_) {
        imageLoader_->onImageLoaderReady();
    } else {
        OutputDebugStringA("[JSBridge] ERROR: ImageLoader not initialized!\n");
    }

    return JSValueMakeUndefined(ctx);
}

// Static instance management
JSBridge* JSBridge::getInstance() {
    return g_jsBridgeInstance;
}

void JSBridge::setInstance(JSBridge* instance) {
    g_jsBridgeInstance = instance;
}