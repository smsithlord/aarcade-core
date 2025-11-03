# Library KeyValues Structure and Schema Construction

## Overview

This document describes how the library database stores entries and how schema construction works. This information is essential for any engineer who needs to write to the database (e.g., updating an item's description) without corrupting the data structure.

## Database Storage Format

### Binary Format
All entry data in the library database is stored as **hexadecimal-encoded binary data** in the `data` column of each table (items, apps, instances, maps, models, platforms, types).

The binary format follows the **Valve KeyValues** structure, which is a hierarchical key-value data format similar to JSON but stored in a compact binary representation.

### Data Structure Hierarchy

The data follows a consistent nested structure:

```
Root (unnamed)
└── Table Name (e.g., "item", "app", "instance")
    ├── info (subsection - metadata)
    │   ├── id (string)
    │   ├── created (int - Unix timestamp)
    │   ├── modified (int - Unix timestamp)
    │   └── owner (string)
    └── local (subsection - actual content fields)
        ├── title (string)
        ├── description (string)
        ├── file (string)
        ├── screen (string)
        ├── keywords (string)
        ├── ... (other fields)
        └── ... (varies by entry)
```

**Important Notes:**
- The `local` subsection is where user-editable fields live (title, description, etc.)
- The `info` subsection contains metadata managed by the system
- Not all entries have all fields - the schema varies per entry

## How Schema Construction Works

### Location
**File:** `aarcade_core/Library.cpp`
**Function:** `Library::constructSchema(const std::string& entryType)`
**Called From:** JavaScript via `aapi.constructSchema(entryType)`

### Algorithm

1. **Fetch All Entries**: Retrieves up to 10,000 entries of the specified type from the database
   ```cpp
   std::vector<std::pair<std::string, std::string>> allEntries =
       dbManager_->getFirstEntries(entryType, 10000);
   ```

2. **Parse Each Entry**: For each entry's hex data:
   ```cpp
   auto kvData = ArcadeKeyValues::ParseFromHex(entry.second);
   ```

3. **Navigate the Hierarchy**:
   ```cpp
   // Get the table section (e.g., "item")
   ArcadeKeyValues* tableSection = kvData->GetFirstSubKey();

   // Try to find the "local" subsection
   ArcadeKeyValues* dataSection = tableSection->FindKey("local");
   if (!dataSection) {
       // If no "local" section, use table section directly
       dataSection = tableSection;
   }
   ```

4. **Collect Field Names**: Iterate through all children of the data section:
   ```cpp
   ArcadeKeyValues* field = dataSection->GetFirstSubKey();
   while (field != nullptr) {
       std::string fieldName = field->GetName();
       if (!fieldName.empty()) {
           fieldSet.insert(fieldName);  // Using set for uniqueness
       }
       field = field->GetNextKey();
   }
   ```

5. **Return Sorted List**: Converts the set to a sorted vector and returns it

### Why "local" Subsection?

The `item.local` path is used for **items table compatibility**. Looking at `JSBridge::entryDataToJSObject()`, we can see:

```cpp
// For items table, try to navigate to item.local path for backwards compatibility
ArcadeKeyValues* itemSection = keyValues->GetFirstSubKey();
if (itemSection) {
    ArcadeKeyValues* localSection = itemSection->FindKey("local");
    if (localSection) {
        return arcadeKeyValuesToJSObject(ctx, localSection);
    }
}
```

This means when JavaScript receives an item entry, it gets the contents of `item.local`, not the entire `item` structure. This is why schema construction looks at the `local` subsection.

## Writing to the Database Safely

### Critical Steps for Engineers

1. **Read the Existing Entry First**
   ```cpp
   // Get the current entry data
   auto entry = dbManager_->getEntry(entryType, entryId);

   // Parse it
   auto kvData = ArcadeKeyValues::ParseFromHex(entry.hexData);
   ```

2. **Navigate to the Correct Location**
   ```cpp
   ArcadeKeyValues* tableSection = kvData->GetFirstSubKey();
   ArcadeKeyValues* localSection = tableSection->FindKey("local");

   // For updating user fields like title, description, work within localSection
   ```

3. **Modify the Field**
   ```cpp
   // Update a field (example: description)
   ArcadeKeyValues* descField = localSection->FindKey("description");
   if (descField) {
       descField->SetString("New description text");
   } else {
       // Create new field if it doesn't exist
       localSection->SetString("description", "New description text");
   }
   ```

4. **Update Metadata** (if applicable)
   ```cpp
   ArcadeKeyValues* infoSection = tableSection->FindKey("info");
   if (infoSection) {
       // Update modified timestamp
       infoSection->SetInt("modified", time(nullptr));
   }
   ```

5. **Serialize Back to Hex**
   ```cpp
   std::string newHexData = kvData->ToHex();
   ```

6. **Write to Database**
   ```cpp
   dbManager_->updateEntry(entryType, entryId, newHexData);
   ```

### Important Warnings

⚠️ **DO NOT:**
- Change the top-level structure (root -> table name)
- Remove the `local` subsection from items
- Modify the `info.id` field (this is the primary key)
- Write malformed hex data (always parse and serialize using `ArcadeKeyValues`)

⚠️ **ALWAYS:**
- Parse existing data before modifying
- Maintain the hierarchy structure
- Update the `modified` timestamp in the `info` section
- Test with a backup of the database first

## ArcadeKeyValues Class Reference

### Key Methods for Reading
```cpp
// Navigation
ArcadeKeyValues* GetFirstSubKey() const;
ArcadeKeyValues* GetNextKey() const;
ArcadeKeyValues* FindKey(const char* keyName) const;

// Value retrieval
const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") const;
int GetInt(const char* keyName = nullptr, int defaultValue = 0) const;
float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) const;
bool GetBool(const char* keyName = nullptr, bool defaultValue = false) const;

// Metadata
const char* GetName() const;
```

### Key Methods for Writing
```cpp
// Value setting (implementation would need to be added)
void SetString(const char* keyName, const char* value);
void SetInt(const char* keyName, int value);
void SetFloat(const char* keyName, float value);

// Serialization
std::string ToHex() const;  // Convert back to hex string for database storage
```

**Note:** If the writing methods don't exist in `ArcadeKeyValues`, they will need to be implemented following the Valve KeyValues binary format specification.

## Example: Updating an Item's Description

```cpp
// 1. Fetch the item
std::string itemId = "some-item-id";
auto entry = dbManager_->getEntry("items", itemId);

// 2. Parse the data
auto kvData = ArcadeKeyValues::ParseFromHex(entry.hexData);

// 3. Navigate to local section
ArcadeKeyValues* itemSection = kvData->GetFirstSubKey();
ArcadeKeyValues* localSection = itemSection->FindKey("local");

// 4. Update description
localSection->SetString("description", "This is the new description");

// 5. Update modified timestamp
ArcadeKeyValues* infoSection = itemSection->FindKey("info");
if (infoSection) {
    infoSection->SetInt("modified", (int)time(nullptr));
}

// 6. Serialize and save
std::string newHexData = kvData->ToHex();
dbManager_->updateEntry("items", itemId, newHexData);
```

## Database Schema Tables

The library database contains these tables:
- `items` - Individual game/content items
- `apps` - Applications/launchers
- `instances` - Specific instances of items
- `maps` - Maps/levels
- `models` - 3D models or templates
- `platforms` - Gaming platforms
- `types` - Type definitions

Each table has:
- `id` (TEXT PRIMARY KEY) - Unique identifier
- `data` (TEXT) - Hexadecimal-encoded KeyValues data

## Related Files

- **Library.cpp** (lines 138-201) - Schema construction implementation
- **Library.h** (line 58) - Schema construction declaration
- **JSBridge.cpp** (lines 545-568) - Entry parsing logic
- **JSBridge.cpp** (lines 613-641) - JavaScript bridge for schema construction
- **ArcadeKeyValues.h** - KeyValues parser/serializer
- **database-analysis.html** - Frontend UI for schema construction

## Additional Resources

For more details on the Valve KeyValues binary format, refer to:
- Valve Developer Community: KeyValues Format
- The `ArcadeKeyValues` class implementation for specific binary structure details
