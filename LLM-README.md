# AARcade Core - Engineer Onboarding Guide

> **Purpose**: This document provides a comprehensive orientation for engineers (human or AI) working on AARcade Core. It consolidates architectural knowledge, data structures, and best practices for contributing to the codebase.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Core Concepts](#core-concepts)
4. [Data Storage Format](#data-storage-format)
5. [Key Components](#key-components)
6. [JavaScript Bridge API](#javascript-bridge-api)
7. [Database Tools](#database-tools)
8. [Development Guidelines](#development-guidelines)
9. [Common Tasks](#common-tasks)
10. [Troubleshooting](#troubleshooting)

---

## Project Overview

**AARcade Core** is a desktop application for arcade game library management built with C++ and Ultralight. It provides a modern web-based UI for browsing, searching, and managing game metadata stored in a SQLite database.

### Technology Stack

- **C++17**: Core application logic
- **Ultralight SDK**: Lightweight HTML/CSS/JavaScript UI framework (embedded Chromium-like renderer)
- **JavaScriptCore**: JavaScript engine for UI scripting
- **SQLite**: Embedded database for game metadata
- **Valve KeyValues**: Binary hierarchical data format for structured storage

### Key Features

- Library browser with infinite scroll and search
- Image caching system with hash-based deduplication
- Database maintenance tools (compression, anomaly detection, cleanup)
- Schema construction and analysis
- JavaScript-to-C++ bridge for seamless UI/backend communication

---

## Architecture

### High-Level Structure

```
┌─────────────────────────────────────────────┐
│         Frontend (HTML/CSS/JS)              │
│  - library.html (browser)                   │
│  - database-tools.html (utilities)          │
│  - LibraryBrowser class (library.js)        │
└──────────────┬──────────────────────────────┘
               │ JavaScript Calls (aapi.*)
               ▼
┌─────────────────────────────────────────────┐
│          JSBridge (C++)                     │
│  - Exposes C++ methods to JavaScript        │
│  - Type conversion (C++ ↔ JS)               │
│  - Callback management                      │
└──────────────┬──────────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────────┐
│          Library (C++)                      │
│  - Entry browsing & search                  │
│  - Database tools                           │
│  - Schema construction                      │
└──────────────┬──────────────────────────────┘
               │
      ┌────────┴────────┐
      ▼                 ▼
┌────────────┐   ┌─────────────┐
│ SQLite     │   │ ImageLoader │
│ Manager    │   │ (Caching)   │
└────────────┘   └─────────────┘
```

### Component Relationships

```
MainApp
├── Config (config.ini loader)
├── JSBridge (JavaScript interface)
├── Library (business logic)
│   ├── SQLiteManager (database access)
│   └── ImageLoader (image caching)
├── ImageLoader (offscreen rendering)
└── Ultralight App/Window/Overlay
```

### Initialization Sequence

1. `MainApp` constructor reads `config.ini`
2. Creates `SQLiteManager` with database path
3. Creates `Library` with database manager
4. Creates `JSBridge` with library reference
5. Creates Ultralight `App` instance
6. Creates `ImageLoader` with renderer
7. Sets up image loader references
8. Creates window (1280x720) and overlay
9. Registers event listeners (OnWindowObjectReady, etc.)
10. Loads `library.html` as the main UI

**File**: [aarcade_core/MainApp.cpp](aarcade_core/MainApp.cpp)

---

## Core Concepts

### 1. Entry Types

The library supports seven entry types, each stored in its own database table:

| Type | Table | Purpose |
|------|-------|---------|
| `items` | items | Individual game/content items |
| `apps` | apps | Applications/launchers |
| `instances` | instances | Specific configurations of items |
| `maps` | maps | Maps/levels |
| `models` | models | 3D models or templates |
| `platforms` | platforms | Gaming platforms |
| `types` | types | Type definitions/categories |

### 2. Valve KeyValues Format

All entry data is stored as **hexadecimal-encoded binary** following the Valve KeyValues specification.

**Why KeyValues?**
- Compact binary representation (smaller than JSON)
- Hierarchical structure (nested subsections)
- Type-safe values (string, int, float)
- Well-suited for game data serialization

**Type Bytes** (used in binary encoding):
- `0x00` - Nested subsection
- `0x01` - String value (null-terminated)
- `0x02` - Int32 value (little-endian)
- `0x03` - Float32 value (little-endian)
- `0x08` - End of object marker

### 3. Data Hierarchy

#### Standard Items/Apps Structure

```
Root (unnamed)
└── [table_name] (e.g., "item")
    ├── info (subsection - system metadata)
    │   ├── id (string) - Primary key
    │   ├── created (int) - Unix timestamp
    │   ├── modified (int) - Unix timestamp
    │   └── owner (string)
    └── local (subsection - user-editable content)
        ├── title (string)
        ├── description (string)
        ├── file (string) - Path to executable/rom
        ├── screen (string) - Screenshot URL
        ├── keywords (string)
        └── ... (varies by entry)
```

#### Instance Structure

Instances have a different structure optimized for object hierarchies:

```
Root (unnamed)
└── instance
    ├── generation (int) - Schema version
    ├── legacy (int) - Legacy format indicator
    ├── info (subsection)
    │   └── local (subsection - instance metadata)
    │       ├── id (string)
    │       ├── created (int)
    │       ├── modified (int)
    │       └── creator (string)
    ├── objects (subsection - scene objects)
    │   └── [object_id] (subsection for each object)
    │       ├── anim (string)
    │       ├── body (string)
    │       ├── child (string)
    │       └── ... (object properties)
    └── overrides (subsection - material/asset overrides)
        └── materials (subsection)
            └── [material_id] (subsection)
                └── ... (material properties)
```

**Key Differences**:
- Instance metadata stored in `info/local` subsection (not directly in `info` like items/apps)
- Has `generation` and `legacy` at root level (items/apps don't have these)
- Contains `objects` hierarchy for scene graph
- Contains `overrides` for customization
- No top-level `local` subsection (unlike items/apps which have one at root)

---

## Data Storage Format

### Reading Entry Data

**SQLite Storage**:
```sql
CREATE TABLE items (
    id TEXT PRIMARY KEY,
    data TEXT  -- Hexadecimal string (e.g., "00697465...08")
);
```

**Parsing Flow**:
1. Fetch hex string from database
2. Convert hex to byte array
3. Parse bytes using `ArcadeKeyValues::ParseFromHex()`
4. Navigate hierarchy using KeyValues API
5. Extract values using type-safe getters

**Example** (C++):
```cpp
// 1. Fetch from database
auto entry = dbManager->getEntry("items", "game-123");

// 2. Parse hex data
auto kvData = ArcadeKeyValues::ParseFromHex(entry.hexData);

// 3. Navigate hierarchy
ArcadeKeyValues* itemSection = kvData->GetFirstSubKey();  // Get "item" section
ArcadeKeyValues* localSection = itemSection->FindKey("local");

// 4. Extract values
const char* title = localSection->GetString("title", "Untitled");
const char* description = localSection->GetString("description", "");
```

### Writing Entry Data

**⚠️ CRITICAL: Always read existing data before writing!**

**Safe Update Flow**:
1. Read current entry from database
2. Parse to `ArcadeKeyValues`
3. Navigate to target section
4. Modify field(s)
5. Update `modified` timestamp in `info` section
6. Serialize back to hex using `SerializeToHex()`
7. Write to database

**Example** (C++):
```cpp
// 1. Read existing entry
auto entry = dbManager->getEntry("items", itemId);
auto kvData = ArcadeKeyValues::ParseFromHex(entry.hexData);

// 2. Navigate to local section
ArcadeKeyValues* itemSection = kvData->GetFirstSubKey();
ArcadeKeyValues* localSection = itemSection->FindKey("local");

// 3. Update field
localSection->SetString("description", "New description text");

// 4. Update metadata timestamp
ArcadeKeyValues* infoSection = itemSection->FindKey("info");
if (infoSection) {
    infoSection->SetInt("modified", (int)time(nullptr));
}

// 5. Serialize back to hex
std::string newHexData = kvData->SerializeToHex();

// 6. Write to database
dbManager->updateEntryById("items", itemId, newHexData);
```

### Critical Rules

**DO NOT**:
- ❌ Change top-level structure (root → table name)
- ❌ Remove `info` or `local` subsections
- ❌ Modify `info.id` (it's the primary key)
- ❌ Write raw hex strings without parsing/serializing
- ❌ Skip updating the `modified` timestamp

**ALWAYS**:
- ✅ Parse existing data before modifying
- ✅ Maintain hierarchy structure
- ✅ Update `modified` timestamp in `info` section
- ✅ Use `ArcadeKeyValues` methods for serialization
- ✅ Test with a database backup first

---

## Key Components

### 1. ArcadeKeyValues

**Location**: [aarcade_core/ArcadeKeyValues.h](aarcade_core/ArcadeKeyValues.h)

The core class for parsing, manipulating, and serializing Valve KeyValues data.

#### Key Methods

**Parsing**:
```cpp
// Static factory method
static std::unique_ptr<ArcadeKeyValues> ParseFromHex(const std::string& hexData);
```

**Navigation**:
```cpp
ArcadeKeyValues* GetFirstSubKey() const;        // Get first child
ArcadeKeyValues* GetNextKey() const;            // Get next sibling
ArcadeKeyValues* FindKey(const char* keyName);  // Find child by name
const char* GetName() const;                     // Get this node's name
int GetChildCount() const;                       // Count direct children
```

**Reading Values**:
```cpp
const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") const;
int GetInt(const char* keyName = nullptr, int defaultValue = 0) const;
float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) const;
bool GetBool(const char* keyName = nullptr, bool defaultValue = false) const;
```

**Writing Values**:
```cpp
void SetString(const char* keyName, const char* value);
void SetInt(const char* keyName, int value);
void SetFloat(const char* keyName, float value);
void SetBool(const char* keyName, bool value);
```

**Key Manipulation**:
```cpp
bool RemoveKey(const char* keyName);  // Remove a child key
void Clear();                          // Remove all children
```

**Serialization**:
```cpp
std::vector<uint8_t> SerializeToBinary() const;  // To byte array
std::string SerializeToHex() const;               // To hex string
void PrintToConsole(int depth = 0) const;         // Debug output
```

#### Implementation Details

- Uses `std::unordered_map<std::string, std::unique_ptr<ArcadeKeyValues>>` for children
- Supports lazy value type detection (string/int/float/subsection)
- Parent pointers enable sibling iteration
- Recursive parsing/serialization

### 2. Library Class

**Location**: [aarcade_core/Library.h](aarcade_core/Library.h), [aarcade_core/Library.cpp](aarcade_core/Library.cpp)

Manages all library-related business logic.

#### Responsibilities

- Entry browsing (paginated)
- Search functionality
- Image caching coordination
- Schema construction
- Database maintenance tools

#### Key Methods

**Entry Browsing**:
```cpp
std::vector<std::pair<std::string, std::string>> getFirstEntries(const std::string& entryType, int count);
std::vector<std::pair<std::string, std::string>> getNextEntries(int count);
std::pair<std::string, std::string> getFirstEntry(const std::string& entryType);
std::pair<std::string, std::string> getNextEntry();
```

**Search**:
```cpp
std::vector<std::pair<std::string, std::string>> getFirstSearchResults(
    const std::string& entryType,
    const std::string& searchTerm,
    int count
);
std::vector<std::pair<std::string, std::string>> getNextSearchResults(int count);
```

**Image Caching**:
```cpp
void cacheImage(const std::string& url, std::function<void(const ImageLoadResult&)> callback);
void processImageCompletions();  // Process completed cache requests
```

**Utilities**:
```cpp
std::vector<std::string> getSupportedEntryTypes() const;
std::vector<std::string> constructSchema(const std::string& entryType);
```

**Database Tools** (see [Database Tools](#database-tools) section)

### 3. SQLiteManager

**Location**: [aarcade_core/SQLiteManager.h](aarcade_core/SQLiteManager.h)

Low-level database access layer.

#### Key Methods

```cpp
bool openDatabase(const std::string& dbPath);
void closeDatabase();

// Entry retrieval
std::vector<std::pair<std::string, std::string>> getFirstEntries(const std::string& type, int count);
std::vector<std::pair<std::string, std::string>> getNextEntries(int count);

// Search
std::vector<std::pair<std::string, std::string>> getFirstSearchResults(
    const std::string& type,
    const std::string& searchTerm,
    int count
);
std::vector<std::pair<std::string, std::string>> getNextSearchResults(int count);

// Updates
bool updateEntryById(const std::string& tableName, const std::string& id, const std::string& hexData);

// Utilities
std::vector<std::string> getSupportedEntryTypes() const;
std::string extractTitleFromKeyValues(const std::string& hexData);
std::string getTableNameForType(const std::string& type);  // "items" -> "items", "item" -> "items"
```

#### Internal State

- Maintains pagination state (last retrieved ID)
- Keeps prepared statements for performance
- Manages SQLite connection lifecycle

### 4. ImageLoader

**Location**: [aarcade_core/ImageLoader.h](aarcade_core/ImageLoader.h)

Handles image loading with hash-based caching, similar to Kodi's approach.

#### Features

- **512x512 offscreen rendering**: Uses hidden Ultralight view
- **Kodi-style CRC32 hashing**: Deduplicates images by URL hash
- **Queue-based processing**: Sequential loading prevents race conditions
- **PNG caching**: Saves rendered images to disk
- **Multiple callbacks**: Multiple entries can share same image

#### Key Methods

```cpp
// Request image caching
void loadAndCacheImage(const std::string& url, std::function<void(const ImageLoadResult&)> callback);

// Process queue (called from JavaScript periodically)
void processCompletions();

// Callbacks from image-loader.html
void onImageLoaded(bool success, const std::string& url, int x, int y, int width, int height);
void onImageLoaderReady();

// Configuration
void setCacheDirectory(const std::string& path);
RefPtr<View> getView() const;
```

#### Cache Flow

1. **Request**: JavaScript calls `aapi.getCacheImage(url)`
2. **Hash Check**: Compute CRC32 hash of URL
3. **Cache Hit**: If PNG exists, return immediately via callback
4. **Cache Miss**: Add to queue
5. **Queue Processing**:
   - Load URL in offscreen view
   - Wait for `onImageLoaded` callback from JS
   - Capture rendered rectangle
   - Save as PNG
6. **Completion**: Call all pending callbacks with result

**Cache Directory Structure**:
```
cache/
├── a1b2c3d4.png  (hash of image URL)
├── e5f6a7b8.png
└── ...
```

### 5. JSBridge

**Location**: [aarcade_core/JSBridge.h](aarcade_core/JSBridge.h), [aarcade_core/JSBridge.cpp](aarcade_core/JSBridge.cpp)

Bridges JavaScript UI to C++ backend.

#### Architecture

**Callback Pattern**:
```cpp
// C++ method
JSValueRef JSBridge::getFirstEntries(JSContextRef ctx, ...);

// Global C-style callback wrapper
JSValueRef getFirstEntriesCallback(JSContextRef ctx, ...) {
    return JSBridge::getInstance()->getFirstEntries(ctx, ...);
}

// Registration in setupJavaScriptBridge()
JSStringRef methodName = JSStringCreateWithUTF8CString("getFirstEntries");
JSObjectRef methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, getFirstEntriesCallback);
JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
```

#### Type Conversion Helpers

```cpp
// C++ → JavaScript
JSObjectRef arcadeKeyValuesToJSObject(JSContextRef ctx, const ArcadeKeyValues* kv);
JSObjectRef entryDataToJSObject(JSContextRef ctx, const std::string& entryId, const std::string& hexData);
JSObjectRef createJSArray(JSContextRef ctx, const std::vector<std::pair<std::string, std::string>>& entries);
JSObjectRef createStringArray(JSContextRef ctx, const std::vector<std::string>& strings);
```

---

## JavaScript Bridge API

All C++ methods are exposed to JavaScript via the global `aapi` object.

### Entry Browsing

```javascript
// Get first batch of entries
const entries = aapi.getFirstEntries(entryType, count);
// Returns: Array of { id: string, ...fields }

// Get next batch (pagination)
const moreEntries = aapi.getNextEntries(count);

// Single entry access
const entry = aapi.getFirstEntry(entryType);
const nextEntry = aapi.getNextEntry();
```

### Search

```javascript
// Start new search
const results = aapi.getFirstSearchResults(entryType, searchTerm, count);
// Returns: Array of { id: string, ...fields }

// Get more results
const moreResults = aapi.getNextSearchResults(count);
```

### Image Caching

```javascript
// Request image caching (promise-like)
aapi.getCacheImage(imageUrl)
    .then((result) => {
        console.log('Cached:', result.filePath);
        imgElement.src = 'file:///' + result.filePath.replace(/\\/g, '/');
    })
    .catch((error) => {
        console.error('Failed:', error);
    });

// Must be called periodically to process queue
aapi.processImageCompletions();
```

### Utilities

```javascript
// Get supported entry types
const types = aapi.getSupportedEntryTypes();
// Returns: ["items", "apps", "instances", "maps", "models", "platforms", "types"]

// Construct schema for entry type
const fields = aapi.constructSchema(entryType);
// Returns: ["title", "description", "file", "screen", ...]
```

### Application Control

```javascript
// Quit the application
aapi.quitApplication();
```

### Database Tools

See [Database Tools](#database-tools) section for detailed API.

---

## Database Tools

AARcade Core includes built-in database maintenance utilities accessible via the UI.

### 1. Detect Large Entries

**Purpose**: Find oversized BLOB entries for optimization/cleanup

**JavaScript API**:
```javascript
const largeEntries = aapi.dbtFindLargeEntriesInTable(tableName, minSizeBytes);
// Returns: [{ id: string, title: string, sizeBytes: number }, ...]
```

**C++ Method**: [Library.cpp](aarcade_core/Library.cpp) - `dbtFindLargeEntriesInTable()`

**UI**: [detect-large-entries.html](src/assets/detect-large-entries.html)

### 2. Trim Text Fields

**Purpose**: Truncate long text fields to reduce database size

**JavaScript API**:
```javascript
const results = aapi.dbtTrimTextFields(tableName, entryIds, maxLength);
// Returns: [{ id: string, success: bool, error: string }, ...]
```

**C++ Method**: [Library.cpp](aarcade_core/Library.cpp) - `dbtTrimTextFields()`

**Implementation**:
- Parses each entry's KeyValues
- Truncates string fields in `local` section
- Preserves structure and metadata
- Updates `modified` timestamp

### 3. Database Maintenance

**Get Statistics**:
```javascript
const stats = aapi.dbtGetDatabaseStats();
// Returns: {
//   filePath: string,
//   fileSizeBytes: number,
//   pageCount: number,
//   pageSize: number,
//   freePages: number,
//   fragmentationPercent: number
// }
```

**Compact Database**:
```javascript
const result = aapi.dbtCompactDatabase();
// Returns: {
//   success: bool,
//   error: string,
//   beforeSizeBytes: number,
//   afterSizeBytes: number,
//   spaceSavedBytes: number
// }
```

**C++ Methods**: [Library.cpp](aarcade_core/Library.cpp) - `dbtGetDatabaseStats()`, `dbtCompactDatabase()`

**UI**: [compact-database.html](src/assets/compact-database.html)

**Implementation**: Executes SQLite `VACUUM` command

### 4. Detect Anomalous Instances at Root

**Purpose**: Find instances with unexpected root-level keys for data quality review

**Expected Keys**: `generation`, `info`, `objects`, `overrides`, `legacy`

**JavaScript API**:
```javascript
// Detect anomalous instances
const anomalous = aapi.dbtFindAnomalousInstances();
// Returns: [{
//   id: string,
//   unexpectedKeys: string[],
//   keyCount: number,
//   generation: number,
//   legacy: number
// }, ...]

// Get KeyValues as plain text
const keyValuesText = aapi.dbtGetInstanceKeyValues(instanceId);

// Remove anomalous keys (bulk operation)
const results = aapi.dbtRemoveAnomalousKeys(instanceIds);
// Returns: [{ id: string, success: bool, error: string }, ...]
```

**C++ Methods**: [Library.cpp](aarcade_core/Library.cpp) - `dbtFindAnomalousInstances()`, `dbtGetInstanceKeyValues()`, `dbtRemoveAnomalousKeys()`

**UI**: [detect-anomalous-instances.html](src/assets/detect-anomalous-instances.html)

**Features**:
- Checkbox selection for bulk operations
- Modal view for inspecting full KeyValues structure
- Safe removal of unexpected keys while preserving structure

### 5. Database Merge

**Purpose**: Merge entries from another database file into the current library

**JavaScript API**:
```javascript
const result = aapi.dbtMergeDatabase(sourcePath, tableName, skipExisting, overwriteIfLarger);
// Returns: {
//   success: bool,
//   error: string,
//   totalEntries: number,
//   mergedCount: number,
//   skippedCount: number,
//   overwrittenCount: number,
//   failedCount: number,
//   entries: [{ id: string, action: string, blobSizeBytes: number, error: string }, ...]
// }
```

**Merge Strategies**:
- **Skip existing** (`skipExisting=true, overwriteIfLarger=false`): Only add new entries, never overwrite
- **Overwrite all** (`skipExisting=false, overwriteIfLarger=false`): Replace all existing entries with source data
- **Overwrite if larger** (`skipExisting=false, overwriteIfLarger=true`): Only overwrite when source blob is larger

**Transaction Optimization**:

The merge operation has been optimized for reliability and performance using proper transaction management:

1. **Transaction Diagnostics**: Checks and displays current journal mode and synchronous settings
2. **DELETE Journal Mode**: Forces DELETE mode (not WAL) for direct writes to main database file
3. **PRAGMA synchronous=FULL**: Ensures SQLite waits for data to be written to disk, preventing data loss
4. **Single Transaction**: Wraps entire table merge in `BEGIN TRANSACTION` / `COMMIT` for atomicity
5. **Explicit Cache Flush**: Calls `sqlite3_db_cacheflush()` after commit to force cached pages to disk
6. **Rollback on Error**: Automatically rolls back transaction if any error occurs

**Performance Impact**:
- **Before optimization**: Each entry = separate transaction (thousands of disk commits)
- **After optimization**: Entire table = single transaction (one disk commit)
- **Merge All Tables**: Now efficiently processes all 7 tables with proper persistence guarantees

**Important Notes**:
- Size comparison parses both source and existing blobs to binary for accurate size measurement
- All changes persist reliably to disk instead of being buffered in memory
- Safe to use for large-scale merges (thousands of entries)
- Diagnostic output helps identify database configuration issues

**C++ Method**: [Library.cpp](aarcade_core/Library.cpp) - `dbtMergeDatabase()`

**UI**: [merge-database.html](src/assets/merge-database.html)

---

## Development Guidelines

### Building the Project

**Prerequisites**:
- Visual Studio 2022 or later
- Windows SDK 10.0.22621.0 or later
- vcpkg (for future dependencies)

**Build Commands**:

Via VS Code:
```
Ctrl+Shift+B  (Build task)
F5            (Debug)
```

Via PowerShell:
```powershell
.\.vscode\build.ps1
```

Via Command Line:
```cmd
"F:\Installed Programs\Visual Studio Community 2022\MSBuild\Current\Bin\amd64\MSBuild.exe" ^
    aarcade_core.sln /p:Configuration=Release /p:Platform=x64 /v:minimal
```

**Output**: `x64/Release/aarcade-core.exe`

### Code Style

**C++**:
- Use C++17 features
- Prefer smart pointers (`std::unique_ptr`, `RefPtr`)
- Use `const` for read-only parameters
- Member variables end with underscore (`dbManager_`)
- Use snake_case for functions/variables

**JavaScript**:
- Use modern ES6+ syntax
- Prefer `const`/`let` over `var`
- Use arrow functions for callbacks
- camelCase for functions/variables

### Adding New JavaScript Bridge Methods

**Step 1**: Add method to Library class
```cpp
// Library.h
std::string myNewMethod(const std::string& param);

// Library.cpp
std::string Library::myNewMethod(const std::string& param) {
    // Implementation
}
```

**Step 2**: Add JSBridge wrapper
```cpp
// JSBridge.h
JSValueRef myNewMethod(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

// JSBridge.cpp (callback wrapper)
JSValueRef myNewMethodCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    return JSBridge::getInstance()->myNewMethod(ctx, function, thisObject, argumentCount, arguments, exception);
}

// JSBridge.cpp (implementation)
JSValueRef JSBridge::myNewMethod(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
    size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    if (argumentCount < 1) {
        return JSValueMakeString(ctx, JSStringCreateWithUTF8CString("Error: Missing parameter"));
    }

    // Convert JS argument to C++ string
    JSStringRef jsStr = JSValueToStringCopy(ctx, arguments[0], exception);
    size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsStr);
    char* buffer = new char[maxSize];
    JSStringGetUTF8CString(jsStr, buffer, maxSize);
    std::string param(buffer);
    delete[] buffer;
    JSStringRelease(jsStr);

    // Call library method
    std::string result = library_->myNewMethod(param);

    // Convert result to JS
    return JSValueMakeString(ctx, JSStringCreateWithUTF8CString(result.c_str()));
}
```

**Step 3**: Register in setupJavaScriptBridge()
```cpp
// JSBridge.cpp
JSStringRef methodName = JSStringCreateWithUTF8CString("myNewMethod");
JSObjectRef methodFunc = JSObjectMakeFunctionWithCallback(ctx, methodName, myNewMethodCallback);
JSObjectSetProperty(ctx, aapiObj, methodName, methodFunc, 0, 0);
JSStringRelease(methodName);
```

**Step 4**: Use from JavaScript
```javascript
const result = aapi.myNewMethod("test parameter");
```

### Adding New Database Tools

**Pattern**: Follow existing tools (e.g., detect-large-entries, compact-database)

**Steps**:

1. **Add structs/methods to Library.h**
2. **Implement in Library.cpp**
3. **Add JSBridge wrappers**
4. **Create HTML UI in src/assets/**
5. **Add tool card to database-tools.html**
6. **Test thoroughly with backup database**

**UI Template**:
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Tool Name - Database Tools</title>
    <!-- Copy styles from existing tool pages -->
</head>
<body>
    <nav class="breadcrumbs">
        <a href="welcome.html">Home</a>
        <span class="separator">/</span>
        <a href="database-tools.html">Database Tools</a>
        <span class="separator">/</span>
        <span class="current">Tool Name</span>
    </nav>

    <div class="page-wrapper">
        <div class="container">
            <h1>🔧 Tool Name</h1>
            <button onclick="runTool()">Run Tool</button>
            <div id="results"></div>
        </div>
    </div>

    <script>
        function runTool() {
            const results = aapi.myToolMethod();
            // Display results
        }
    </script>
</body>
</html>
```

### Testing Guidelines

**Database Safety**:
- Always test with a backup copy of `library.db`
- Use SQLite Browser to inspect changes
- Verify KeyValues structure after modifications

**Image Caching**:
- Test with various image formats (PNG, JPG, WebP)
- Verify hash collisions don't occur
- Check cache directory cleanup

**UI Testing**:
- Test infinite scroll performance (10k+ entries)
- Verify search debouncing works
- Check modal overlays on different screen sizes

---

## Common Tasks

### Task 1: Update an Entry's Field

```cpp
// Example: Update item description
std::string itemId = "game-123";

// 1. Fetch entry
auto entries = dbManager_->getFirstEntries("items", 1000);
auto it = std::find_if(entries.begin(), entries.end(),
    [&](const auto& e) { return e.first == itemId; });

if (it == entries.end()) {
    std::cerr << "Entry not found" << std::endl;
    return;
}

// 2. Parse KeyValues
auto kvData = ArcadeKeyValues::ParseFromHex(it->second);

// 3. Navigate to local section
ArcadeKeyValues* itemSection = kvData->GetFirstSubKey();
ArcadeKeyValues* localSection = itemSection->FindKey("local");

if (!localSection) {
    std::cerr << "No local section found" << std::endl;
    return;
}

// 4. Update field
localSection->SetString("description", "New description text");

// 5. Update metadata
ArcadeKeyValues* infoSection = itemSection->FindKey("info");
if (infoSection) {
    infoSection->SetInt("modified", (int)time(nullptr));
}

// 6. Serialize and save
std::string newHexData = kvData->SerializeToHex();
dbManager_->updateEntryById("items", itemId, newHexData);

std::cout << "Entry updated successfully" << std::endl;
```

### Task 2: Add New Entry Type Support

**Currently not supported**, but here's the pattern:

1. **Create table in database**:
```sql
CREATE TABLE newtypes (
    id TEXT PRIMARY KEY,
    data TEXT
);
```

2. **Add to SQLiteManager::getSupportedEntryTypes()**:
```cpp
std::vector<std::string> SQLiteManager::getSupportedEntryTypes() const {
    return { "items", "apps", "instances", "maps", "models", "platforms", "types", "newtypes" };
}
```

3. **Update JSBridge if special handling needed**

4. **Add UI tab/filter in library.html**

### Task 3: Inspect KeyValues Structure

**Method 1: In C++ (Debug)**:
```cpp
auto kvData = ArcadeKeyValues::ParseFromHex(hexData);
kvData->PrintToConsole(0);  // Prints to stdout with indentation
```

**Method 2: In JavaScript (Browser UI)**:
```javascript
const entry = aapi.getFirstEntry("items");
console.log(JSON.stringify(entry, null, 2));
```

**Method 3: Database Tool (Schema Construction)**:
```javascript
const fields = aapi.constructSchema("items");
console.log("Available fields:", fields);
```

### Task 4: Clear Image Cache

**Manual**:
```cmd
del /Q x64\Release\cache\*.png
```

**Programmatic** (not currently exposed, but could be added):
```cpp
void ImageLoader::clearCache() {
    std::filesystem::remove_all(cacheDirectory_);
    std::filesystem::create_directories(cacheDirectory_);
}
```

### Task 5: Debug JavaScript Bridge

**Enable Console Logging**:
```javascript
// In your HTML file
console.log('Calling aapi method...');
const result = aapi.getFirstEntries("items", 10);
console.log('Result:', result);
```

**C++ Side Logging**:
```cpp
// In JSBridge.cpp
std::cout << "[JSBridge] getFirstEntries called with type=" << type << ", count=" << count << std::endl;
```

**Check for Exceptions**:
```cpp
JSValueRef exception = nullptr;
JSValueRef result = aapi.someMethod(ctx, ..., &exception);

if (exception) {
    JSStringRef exceptionString = JSValueToStringCopy(ctx, exception, nullptr);
    // Print exception
}
```

---

## Troubleshooting

### Build Errors

**Error**: `MSBuild not found`
- **Solution**: Ensure Visual Studio 2022 is installed with C++ desktop development workload

**Error**: `Ultralight SDK not found`
- **Solution**: Verify `include/Ultralight/` directory exists with SDK headers

**Error**: `sqlite3.h not found`
- **Solution**: Check `include/sqlite/` directory has SQLite headers

### Runtime Errors

**Error**: `Failed to open database`
- **Solution**: Check `config.ini` has correct `database_path`
- Verify `library.db` exists and is not corrupted
- Ensure write permissions on database file

**Error**: `JavaScript method undefined`
- **Solution**: Verify method is registered in `JSBridge::setupJavaScriptBridge()`
- Check spelling matches JavaScript call
- Rebuild project after adding new methods

**Error**: `Image not loading`
- **Solution**: Check URL is valid and accessible
- Verify `cache/` directory exists with write permissions
- Check `ImageLoader::onImageLoaderReady()` was called
- Look for errors in JavaScript console

### Database Corruption

**Symptoms**:
- Entries not loading
- Parse errors in KeyValues
- SQLite errors on query

**Recovery**:
1. **Backup immediately**: Copy `library.db` to safe location
2. **Check integrity**: Use SQLite Browser's integrity check
3. **Export data**: Export to SQL if possible
4. **Rebuild**: Create new database and re-import

**Prevention**:
- Always use transactions for bulk updates
- Validate KeyValues before writing
- Keep regular backups
- Use database tools to detect issues early

### Performance Issues

**Slow library browsing**:
- Check database size (use compact tool if >100MB)
- Verify indexes exist on `id` columns
- Reduce batch size in `getFirstEntries()`

**High memory usage**:
- Clear image cache periodically
- Limit number of concurrent image loads
- Check for memory leaks in KeyValues parsing

**UI freezing**:
- Ensure `processImageCompletions()` is called regularly
- Don't block main thread with heavy computations
- Use debouncing for search input

---

## File Reference

### Core Application Files

| File | Purpose | Lines |
|------|---------|-------|
| [aarcade-core.cpp](aarcade-core.cpp) | Application entry point | ~50 |
| [aarcade_core/MainApp.h](aarcade_core/MainApp.h) | Main app class header | ~50 |
| [aarcade_core/MainApp.cpp](aarcade_core/MainApp.cpp) | App initialization | ~150 |
| [aarcade_core/Config.h](aarcade_core/Config.h) | Configuration loader | ~100 |

### Data Layer

| File | Purpose | Lines |
|------|---------|-------|
| [aarcade_core/ArcadeKeyValues.h](aarcade_core/ArcadeKeyValues.h) | KeyValues parser/serializer | ~445 |
| [aarcade_core/SQLiteManager.h](aarcade_core/SQLiteManager.h) | Database interface | ~800 |

### Business Logic

| File | Purpose | Lines |
|------|---------|-------|
| [aarcade_core/Library.h](aarcade_core/Library.h) | Library manager header | ~130 |
| [aarcade_core/Library.cpp](aarcade_core/Library.cpp) | Library implementation | ~780 |

### Bridge Layer

| File | Purpose | Lines |
|------|---------|-------|
| [aarcade_core/JSBridge.h](aarcade_core/JSBridge.h) | JS-C++ bridge header | ~175 |
| [aarcade_core/JSBridge.cpp](aarcade_core/JSBridge.cpp) | Bridge implementation | ~1400 |

### Image System

| File | Purpose | Lines |
|------|---------|-------|
| [aarcade_core/ImageLoader.h](aarcade_core/ImageLoader.h) | Image caching system | ~630 |
| [src/assets/image-loader.html](src/assets/image-loader.html) | Offscreen image renderer | ~100 |

### UI Files

| File | Purpose | Lines |
|------|---------|-------|
| [src/assets/library.html](src/assets/library.html) | Main library browser UI | ~200 |
| [src/assets/library.js](src/assets/library.js) | LibraryBrowser class | ~630 |
| [src/assets/style.css](src/assets/style.css) | Global styles | ~500 |
| [src/assets/welcome.html](src/assets/welcome.html) | Welcome page | ~150 |
| [src/assets/database-tools.html](src/assets/database-tools.html) | Database tools menu | ~195 |
| [src/assets/database-analysis.html](src/assets/database-analysis.html) | Schema construction UI | ~200 |
| [src/assets/detect-large-entries.html](src/assets/detect-large-entries.html) | Large entry detection | ~400 |
| [src/assets/compact-database.html](src/assets/compact-database.html) | Database compaction | ~300 |
| [src/assets/detect-anomalous-instances.html](src/assets/detect-anomalous-instances.html) | Instance anomaly detection | ~460 |

### Configuration

| File | Purpose |
|------|---------|
| [config.ini](x64/Release/config.ini) | Runtime configuration (database path) |
| [.vscode/build.ps1](.vscode/build.ps1) | Build script |
| [.vscode/tasks.json](.vscode/tasks.json) | VS Code build tasks |
| [.vscode/launch.json](.vscode/launch.json) | Debug configuration |

---

## Additional Resources

### External Documentation

- **Ultralight SDK**: https://ultralig.ht/
- **JavaScriptCore**: https://developer.apple.com/documentation/javascriptcore
- **SQLite**: https://www.sqlite.org/docs.html
- **Valve KeyValues**: Valve Developer Community wiki

### Internal Documentation

- [README.md](README.md) - Project overview
- [LibraryKeyValuesNotes.md](LibraryKeyValuesNotes.md) - Original KeyValues notes (legacy)
- [LIBRARY_ANALYSIS.txt](LIBRARY_ANALYSIS.txt) - Original architecture analysis (legacy)

### Useful Commands

**SQLite CLI**:
```bash
# Open database
sqlite3 x64/Release/library.db

# List tables
.tables

# Inspect table schema
.schema items

# Count entries
SELECT COUNT(*) FROM items;

# Export to CSV
.mode csv
.output items.csv
SELECT id, length(data) as size FROM items;
```

**vcpkg** (for future dependencies):
```bash
# Install package
vcpkg install <package-name>:x64-windows

# List installed
vcpkg list
```

---

## Glossary

| Term | Definition |
|------|------------|
| **Entry** | A database record (item, app, instance, etc.) |
| **KeyValues** | Valve's binary hierarchical data format |
| **Hex Data** | Hexadecimal-encoded string representation of binary KeyValues |
| **Local Section** | User-editable fields within an entry (title, description, etc.) |
| **Info Section** | System metadata (id, created, modified, owner) |
| **aapi** | JavaScript global object exposing C++ bridge methods |
| **JSBridge** | C++ class managing JavaScript-to-C++ communication |
| **ImageLoader** | Offscreen rendering system for image caching |
| **SQLiteManager** | Low-level database access layer |
| **Library** | High-level business logic manager |

---

## Contributing

When contributing to this project:

1. **Follow code style guidelines** (see [Development Guidelines](#development-guidelines))
2. **Test thoroughly** with backup database
3. **Update documentation** for new features
4. **Add JSBridge methods** for UI-accessible functionality
5. **Keep KeyValues structure intact** when modifying data

---

## Version History

- **Current**: Library class architecture, database tools, anomaly detection
- **Previous**: JSBridge direct access to SQLiteManager (refactored)
- **Original**: Monolithic library.js with inline C++ calls

---

**Document Version**: 1.0
**Last Updated**: 2025
**Maintained By**: AARcade Core Team
