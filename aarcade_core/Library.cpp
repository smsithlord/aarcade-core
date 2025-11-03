#include "Library.h"
#include <windows.h>
#include <set>
#include <algorithm>

Library::Library(SQLiteManager* dbManager, ArcadeConfig* config)
    : dbManager_(dbManager), config_(config), imageLoader_(nullptr) {
    OutputDebugStringA("[Library] Library initialized\n");
}

Library::~Library() {
    OutputDebugStringA("[Library] Library destroyed\n");
}

void Library::setImageLoader(ImageLoader* imageLoader) {
    imageLoader_ = imageLoader;
    OutputDebugStringA("[Library] ImageLoader reference set\n");
}

bool Library::openDatabase() {
    if (!dbManager_->openDatabase(config_->getDatabasePath())) {
        OutputDebugStringA("[Library] Failed to open database!\n");
        return false;
    }
    return true;
}

std::vector<std::string> Library::getSupportedEntryTypes() const {
    return dbManager_->getSupportedEntryTypes();
}

std::vector<std::pair<std::string, std::string>> Library::getFirstEntries(const std::string& entryType, int count) {
    OutputDebugStringA(("[Library] getFirstEntries: Requesting " + std::to_string(count) + " entries of type '" + entryType + "'\n").c_str());

    // Validate count
    if (count <= 0 || count > 1000) {
        OutputDebugStringA("[Library] getFirstEntries: Invalid count parameter\n");
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Open database if not already open
    if (!openDatabase()) {
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Get the first entries
    return dbManager_->getFirstEntries(entryType, count);
}

std::vector<std::pair<std::string, std::string>> Library::getNextEntries(int count) {
    OutputDebugStringA(("[Library] getNextEntries: Requesting " + std::to_string(count) + " more entries\n").c_str());

    // Validate count
    if (count <= 0 || count > 1000) {
        OutputDebugStringA("[Library] getNextEntries: Invalid count parameter\n");
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Get the next entries
    return dbManager_->getNextEntries(count);
}

std::pair<std::string, std::string> Library::getFirstEntry(const std::string& entryType) {
    OutputDebugStringA(("[Library] getFirstEntry: Getting first entry of type '" + entryType + "'\n").c_str());

    // Open database if not already open
    if (!openDatabase()) {
        return std::make_pair("", "");
    }

    // Get first entries with count of 1
    std::vector<std::pair<std::string, std::string>> entries = dbManager_->getFirstEntries(entryType, 1);

    if (!entries.empty()) {
        return entries[0];
    }

    return std::make_pair("", "");
}

std::pair<std::string, std::string> Library::getNextEntry() {
    OutputDebugStringA("[Library] getNextEntry: Getting next entry\n");
    return dbManager_->getNextEntry();
}

std::vector<std::pair<std::string, std::string>> Library::getFirstSearchResults(const std::string& entryType, const std::string& searchTerm, int count) {
    OutputDebugStringA(("[Library] getFirstSearchResults: Searching '" + entryType + "' for '" + searchTerm + "' (count: " + std::to_string(count) + ")\n").c_str());

    // Validate count
    if (count <= 0 || count > 1000) {
        OutputDebugStringA("[Library] getFirstSearchResults: Invalid count parameter\n");
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Open database if not already open
    if (!openDatabase()) {
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Get the first search results
    return dbManager_->getFirstSearchResults(entryType, searchTerm, count);
}

std::vector<std::pair<std::string, std::string>> Library::getNextSearchResults(int count) {
    OutputDebugStringA(("[Library] getNextSearchResults: Requesting " + std::to_string(count) + " more search results\n").c_str());

    // Validate count
    if (count <= 0 || count > 1000) {
        OutputDebugStringA("[Library] getNextSearchResults: Invalid count parameter\n");
        return std::vector<std::pair<std::string, std::string>>();
    }

    // Get the next search results
    return dbManager_->getNextSearchResults(count);
}

void Library::cacheImage(const std::string& url, std::function<void(const ImageLoadResult&)> callback) {
    OutputDebugStringA(("[Library] cacheImage: Processing URL '" + url + "'\n").c_str());

    if (!imageLoader_) {
        OutputDebugStringA("[Library] ERROR: ImageLoader not initialized!\n");
        // Call callback with failure result
        ImageLoadResult result;
        result.success = false;
        result.filePath = "";
        result.url = url;
        callback(result);
        return;
    }

    imageLoader_->loadAndCacheImage(url, callback);
}

void Library::processImageCompletions() {
    if (imageLoader_) {
        imageLoader_->processCompletions();
    }
}

// Helper function to recursively collect field paths
void Library::collectFieldPathsRecursive(ArcadeKeyValues* node, const std::string& currentPath, std::set<std::string>& fieldSet, bool isInstanceData, int depth) {
    if (!node) return;

    ArcadeKeyValues* child = node->GetFirstSubKey();
    while (child != nullptr) {
        std::string fieldName = child->GetName();
        if (!fieldName.empty()) {
            // Build the full path for this field
            std::string fullPath = currentPath.empty() ? fieldName : currentPath + "." + fieldName;

            // Special handling for instances table
            if (isInstanceData) {
                // Check if we're entering the "objects" section (root level)
                if (fieldName == "objects" && currentPath.empty()) {
                    // This is the "objects" field at the root level
                    fieldSet.insert(fullPath);

                    // Inside objects, we have object IDs - handle them specially
                    ArcadeKeyValues* objectChild = child->GetFirstSubKey();
                    while (objectChild != nullptr) {
                        // Don't add the object ID itself, but process its children with placeholder
                        if (objectChild->GetFirstSubKey() != nullptr) {
                            collectFieldPathsRecursive(objectChild, fullPath + ".[object_id]", fieldSet, isInstanceData, depth + 1);
                        }
                        objectChild = objectChild->GetNextKey();
                    }
                }
                // Check if we're entering the "materials" section (under overrides)
                else if (fieldName == "materials" && currentPath == "overrides") {
                    // This is the "materials" field under "overrides"
                    fieldSet.insert(fullPath);

                    // Inside materials, we have material IDs - handle them specially
                    ArcadeKeyValues* materialChild = child->GetFirstSubKey();
                    while (materialChild != nullptr) {
                        // Don't add the material ID itself, but process its children with placeholder
                        if (materialChild->GetFirstSubKey() != nullptr) {
                            collectFieldPathsRecursive(materialChild, fullPath + ".[material_id]", fieldSet, isInstanceData, depth + 1);
                        }
                        materialChild = materialChild->GetNextKey();
                    }
                }
                else {
                    // Normal field - add it and recurse
                    fieldSet.insert(fullPath);

                    // Recursively process nested objects
                    if (child->GetFirstSubKey() != nullptr) {
                        collectFieldPathsRecursive(child, fullPath, fieldSet, isInstanceData, depth + 1);
                    }
                }
            } else {
                // Normal field - add it and recurse
                fieldSet.insert(fullPath);

                // Recursively process nested objects
                if (child->GetFirstSubKey() != nullptr) {
                    collectFieldPathsRecursive(child, fullPath, fieldSet, isInstanceData, depth + 1);
                }
            }
        }
        child = child->GetNextKey();
    }
}

std::vector<std::string> Library::constructSchema(const std::string& entryType) {
    OutputDebugStringA(("[Library] constructSchema: Constructing schema for '" + entryType + "'\n").c_str());

    std::vector<std::string> schema;
    std::set<std::string> fieldSet; // Use set to track unique fields with paths

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] constructSchema: Failed to open database\n");
        return schema;
    }

    // Get all entries for this type
    std::vector<std::pair<std::string, std::string>> allEntries = dbManager_->getFirstEntries(entryType, 10000);

    OutputDebugStringA(("[Library] constructSchema: Analyzing " + std::to_string(allEntries.size()) + " entries\n").c_str());

    // Iterate through all entries and collect field names with paths
    for (const auto& entry : allEntries) {
        if (entry.second.empty()) {
            continue;
        }

        // Parse the hex data
        auto kvData = ArcadeKeyValues::ParseFromHex(entry.second);
        if (!kvData) {
            continue;
        }

        // Navigate to the actual data section
        // Structure is: root -> "item"/"app"/etc -> fields
        ArcadeKeyValues* tableSection = kvData->GetFirstSubKey();
        if (!tableSection) {
            continue;
        }

        // Check if there's a "local" subsection (for items table compatibility)
        ArcadeKeyValues* dataSection = tableSection->FindKey("local");
        if (!dataSection) {
            // If no local section, use the table section itself
            dataSection = tableSection;
        }

        // Determine if this is an instances table (for special "objects" handling)
        bool isInstanceData = (entryType == "instances");

        // Recursively collect all field paths
        collectFieldPathsRecursive(dataSection, "", fieldSet, isInstanceData, 0);
    }

    // Convert set to vector for return
    schema.assign(fieldSet.begin(), fieldSet.end());

    // Sort alphabetically for consistency
    std::sort(schema.begin(), schema.end());

    OutputDebugStringA(("[Library] constructSchema: Found " + std::to_string(schema.size()) + " unique fields\n").c_str());

    return schema;
}

std::vector<Library::LargeBlobEntry> Library::dbtFindLargeEntriesInTable(const std::string& tableName, int minSizeBytes) {
    OutputDebugStringA(("[Library] dbtFindLargeEntriesInTable: Searching '" + tableName + "' for BLOBs over " + std::to_string(minSizeBytes) + " bytes\n").c_str());

    std::vector<LargeBlobEntry> results;

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtFindLargeEntriesInTable: Failed to open database\n");
        return results;
    }

    // Get large BLOBs from database
    std::vector<std::pair<std::string, int>> largeBlobs = dbManager_->dbtFindLargeBlobsInTable(tableName, minSizeBytes);

    OutputDebugStringA(("[Library] dbtFindLargeEntriesInTable: Found " + std::to_string(largeBlobs.size()) + " large BLOBs\n").c_str());

    // For each large BLOB, fetch the full entry and extract title
    for (const auto& blob : largeBlobs) {
        const std::string& id = blob.first;
        int sizeBytes = blob.second;

        LargeBlobEntry entry;
        entry.id = id;
        entry.sizeBytes = sizeBytes;
        entry.title = id; // Default to ID if title extraction fails

        // Fetch the full entry data by ID
        std::pair<std::string, std::string> entryData = dbManager_->getEntryById(tableName, id);

        if (!entryData.second.empty()) {
            // Parse the hex data to extract title
            auto kvData = ArcadeKeyValues::ParseFromHex(entryData.second);
            if (kvData) {
                // Navigate to the actual data section
                // Structure is: root -> "item"/"app"/etc -> fields
                ArcadeKeyValues* tableSection = kvData->GetFirstSubKey();
                if (tableSection) {
                    // Check if there's a "local" subsection (for items table compatibility)
                    ArcadeKeyValues* dataSection = tableSection->FindKey("local");
                    if (!dataSection) {
                        // If no local section, use the table section itself
                        dataSection = tableSection;
                    }

                    // Extract the title field
                    std::string extractedTitle = dataSection->GetString("title", "");
                    if (!extractedTitle.empty()) {
                        entry.title = extractedTitle;
                    }
                }
            }
        }

        results.push_back(entry);
    }

    OutputDebugStringA(("[Library] dbtFindLargeEntriesInTable: Processed " + std::to_string(results.size()) + " entries\n").c_str());

    return results;
}

std::vector<Library::TrimResult> Library::dbtTrimTextFields(const std::string& tableName, const std::vector<std::string>& entryIds, int maxLength) {
    OutputDebugStringA(("[Library] dbtTrimTextFields: Trimming text fields for " + std::to_string(entryIds.size()) + " entries in " + tableName + "\n").c_str());

    std::vector<TrimResult> results;

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtTrimTextFields: Failed to open database\n");
        // Return failure for all entries
        for (const auto& id : entryIds) {
            results.push_back({ id, false, "Database not available" });
        }
        return results;
    }

    // Process each entry
    for (const auto& id : entryIds) {
        TrimResult result;
        result.id = id;
        result.success = false;

        // Fetch the entry data
        std::pair<std::string, std::string> entryData = dbManager_->getEntryById(tableName, id);

        if (entryData.second.empty()) {
            result.error = "Entry not found";
            results.push_back(result);
            continue;
        }

        // Parse the KeyValues data
        auto kvData = ArcadeKeyValues::ParseFromHex(entryData.second);
        if (!kvData) {
            result.error = "Failed to parse KeyValues data";
            results.push_back(result);
            continue;
        }

        // Navigate to the data section
        ArcadeKeyValues* tableSection = kvData->GetFirstSubKey();
        if (!tableSection) {
            result.error = "Invalid data structure";
            results.push_back(result);
            continue;
        }

        // Check for "local" subsection (items table compatibility)
        ArcadeKeyValues* dataSection = tableSection->FindKey("local");
        if (!dataSection) {
            dataSection = tableSection;
        }

        // Track if we made any changes
        bool modified = false;

        // Trim "title" field
        std::string title = dataSection->GetString("title", "");
        if (title.length() > static_cast<size_t>(maxLength)) {
            std::string trimmedTitle = title.substr(0, maxLength);
            dataSection->SetString("title", trimmedTitle.c_str());
            modified = true;
            OutputDebugStringA(("[Library] Trimmed title for " + id + " from " + std::to_string(title.length()) + " to " + std::to_string(maxLength) + " chars\n").c_str());
        }

        // Trim "description" field
        std::string description = dataSection->GetString("description", "");
        if (description.length() > static_cast<size_t>(maxLength)) {
            std::string trimmedDescription = description.substr(0, maxLength);
            dataSection->SetString("description", trimmedDescription.c_str());
            modified = true;
            OutputDebugStringA(("[Library] Trimmed description for " + id + " from " + std::to_string(description.length()) + " to " + std::to_string(maxLength) + " chars\n").c_str());
        }

        // Only update if we made changes
        if (modified) {
            // Serialize back to hex
            std::string updatedHex = kvData->SerializeToHex();

            // Update in database
            if (dbManager_->updateEntryById(tableName, id, updatedHex)) {
                result.success = true;
                result.error = "";
                OutputDebugStringA(("[Library] Successfully trimmed text fields for " + id + "\n").c_str());
            }
            else {
                result.error = "Failed to update database";
                OutputDebugStringA(("[Library] Failed to update database for " + id + "\n").c_str());
            }
        }
        else {
            // No changes needed, consider it a success
            result.success = true;
            result.error = "No trimming needed";
            OutputDebugStringA(("[Library] No trimming needed for " + id + "\n").c_str());
        }

        results.push_back(result);
    }

    OutputDebugStringA(("[Library] dbtTrimTextFields: Completed processing " + std::to_string(results.size()) + " entries\n").c_str());

    return results;
}

Library::DatabaseStats Library::dbtGetDatabaseStats() {
    OutputDebugStringA("[Library] dbtGetDatabaseStats: Getting database statistics\n");

    DatabaseStats stats;
    stats.fileSizeBytes = 0;
    stats.pageCount = 0;
    stats.pageSize = 0;
    stats.freePages = 0;
    stats.fragmentationPercent = 0.0;

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtGetDatabaseStats: Failed to open database\n");
        return stats;
    }

    // Get stats from SQLiteManager
    SQLiteManager::DatabaseStats dbStats = dbManager_->dbtGetDatabaseStats();

    // Copy to Library stats
    stats.filePath = dbStats.filePath;
    stats.fileSizeBytes = dbStats.fileSizeBytes;
    stats.pageCount = dbStats.pageCount;
    stats.pageSize = dbStats.pageSize;
    stats.freePages = dbStats.freePages;
    stats.fragmentationPercent = dbStats.fragmentationPercent;

    OutputDebugStringA(("[Library] Database size: " + std::to_string(stats.fileSizeBytes) + " bytes, " +
                       std::to_string(stats.fragmentationPercent) + "% fragmentation\n").c_str());

    return stats;
}

Library::CompactResult Library::dbtCompactDatabase() {
    OutputDebugStringA("[Library] dbtCompactDatabase: Starting database compaction\n");

    CompactResult result;
    result.success = false;
    result.beforeSizeBytes = 0;
    result.afterSizeBytes = 0;
    result.spaceSavedBytes = 0;

    // Open database if not already open
    if (!openDatabase()) {
        result.error = "Failed to open database";
        OutputDebugStringA("[Library] dbtCompactDatabase: Failed to open database\n");
        return result;
    }

    // Get size before compaction
    DatabaseStats beforeStats = dbtGetDatabaseStats();
    result.beforeSizeBytes = beforeStats.fileSizeBytes;

    // Run VACUUM
    bool success = dbManager_->dbtCompactDatabase();

    if (!success) {
        result.error = "VACUUM operation failed";
        OutputDebugStringA("[Library] dbtCompactDatabase: VACUUM operation failed\n");
        return result;
    }

    // Get size after compaction
    DatabaseStats afterStats = dbtGetDatabaseStats();
    result.afterSizeBytes = afterStats.fileSizeBytes;
    result.spaceSavedBytes = result.beforeSizeBytes - result.afterSizeBytes;
    result.success = true;

    OutputDebugStringA(("[Library] dbtCompactDatabase: Completed! Saved " + std::to_string(result.spaceSavedBytes) + " bytes\n").c_str());

    return result;
}

std::pair<std::string, std::string> Library::getFirstItem() {
    OutputDebugStringA("[Library] getFirstItem: Getting first item (legacy method)\n");

    // Open database if not already open
    if (!openDatabase()) {
        return std::make_pair("", "");
    }

    return dbManager_->getFirstItem();
}

// Helper function to convert KeyValues to plain text format
std::string Library::keyValuesToPlainText(ArcadeKeyValues* kv, int indent) {
    if (!kv) {
        return "";
    }

    std::string result;
    std::string indentStr(indent * 2, ' '); // 2 spaces per indent level

    // Get the name of this key
    const char* name = kv->GetName();
    if (name && name[0] != '\0') {
        result += indentStr + name;
    }

    // Check if this node has a value (leaf node)
    const char* value = kv->GetString(nullptr, nullptr);
    if (value && value[0] != '\0') {
        result += ": \"" + std::string(value) + "\"\n";
    } else {
        // This is a parent node with children
        if (name && name[0] != '\0') {
            result += "\n";
        }

        // Iterate through all children
        ArcadeKeyValues* child = kv->GetFirstSubKey();
        while (child) {
            result += keyValuesToPlainText(child, indent + 1);
            child = child->GetNextKey();
        }
    }

    return result;
}

std::vector<Library::AnomalousInstanceEntry> Library::dbtFindAnomalousInstances() {
    OutputDebugStringA("[Library] dbtFindAnomalousInstances: Searching for instances with unexpected root keys\n");

    std::vector<AnomalousInstanceEntry> results;

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtFindAnomalousInstances: Failed to open database\n");
        return results;
    }

    // Get all instances (up to 10000)
    std::vector<std::pair<std::string, std::string>> allInstances = dbManager_->getFirstEntries("instances", 10000);

    OutputDebugStringA(("[Library] dbtFindAnomalousInstances: Analyzing " + std::to_string(allInstances.size()) + " instances\n").c_str());

    // Expected keys at the root of an instance
    std::set<std::string> expectedKeys = { "generation", "info", "objects", "overrides", "legacy" };

    // Iterate through all instances
    for (const auto& instance : allInstances) {
        if (instance.second.empty()) {
            continue;
        }

        const std::string& id = instance.first;

        // Parse the hex data
        auto kvData = ArcadeKeyValues::ParseFromHex(instance.second);
        if (!kvData) {
            continue;
        }

        // Navigate to the instance section (root -> "instance")
        ArcadeKeyValues* instanceSection = kvData->GetFirstSubKey();
        if (!instanceSection) {
            continue;
        }

        // Collect all keys at the root level
        std::vector<std::string> unexpectedKeys;
        ArcadeKeyValues* child = instanceSection->GetFirstSubKey();
        while (child) {
            const char* keyName = child->GetName();
            if (keyName && keyName[0] != '\0') {
                std::string key(keyName);
                // Check if this key is not in the expected set
                if (expectedKeys.find(key) == expectedKeys.end()) {
                    unexpectedKeys.push_back(key);
                }
            }
            child = child->GetNextKey();
        }

        // If we found any unexpected keys, add to results
        if (!unexpectedKeys.empty()) {
            AnomalousInstanceEntry entry;
            entry.id = id;
            entry.unexpectedKeys = unexpectedKeys;
            entry.keyCount = static_cast<int>(unexpectedKeys.size());

            // Extract generation value (integer)
            ArcadeKeyValues* generationKey = instanceSection->FindKey("generation");
            if (generationKey) {
                entry.generation = generationKey->GetInt(nullptr, 0);
            } else {
                entry.generation = 0;
            }

            // Extract legacy value (integer)
            ArcadeKeyValues* legacyKey = instanceSection->FindKey("legacy");
            if (legacyKey) {
                entry.legacy = legacyKey->GetInt(nullptr, 0);
            } else {
                entry.legacy = 0;
            }

            results.push_back(entry);
        }
    }

    OutputDebugStringA(("[Library] dbtFindAnomalousInstances: Found " + std::to_string(results.size()) + " anomalous instances\n").c_str());

    return results;
}

std::string Library::dbtGetInstanceKeyValues(const std::string& instanceId) {
    OutputDebugStringA(("[Library] dbtGetInstanceKeyValues: Fetching KeyValues for instance " + instanceId + "\n").c_str());

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtGetInstanceKeyValues: Failed to open database\n");
        return "Error: Failed to open database";
    }

    // Fetch the instance data by ID
    std::pair<std::string, std::string> instanceData = dbManager_->getEntryById("instances", instanceId);

    if (instanceData.second.empty()) {
        OutputDebugStringA("[Library] dbtGetInstanceKeyValues: Instance not found\n");
        return "Error: Instance not found";
    }

    // Parse the hex data
    auto kvData = ArcadeKeyValues::ParseFromHex(instanceData.second);
    if (!kvData) {
        OutputDebugStringA("[Library] dbtGetInstanceKeyValues: Failed to parse KeyValues\n");
        return "Error: Failed to parse KeyValues data";
    }

    // Convert to plain text format
    std::string plainText = keyValuesToPlainText(kvData.get(), 0);

    OutputDebugStringA(("[Library] dbtGetInstanceKeyValues: Successfully converted to plain text (" + std::to_string(plainText.length()) + " chars)\n").c_str());

    return plainText;
}

std::vector<Library::RemoveKeysResult> Library::dbtRemoveAnomalousKeys(const std::vector<std::string>& instanceIds) {
    OutputDebugStringA(("[Library] dbtRemoveAnomalousKeys: Removing anomalous keys from " + std::to_string(instanceIds.size()) + " instances\n").c_str());

    std::vector<RemoveKeysResult> results;

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] dbtRemoveAnomalousKeys: Failed to open database\n");
        // Return failure for all instances
        for (const auto& id : instanceIds) {
            results.push_back({ id, false, "Database not available" });
        }
        return results;
    }

    // Expected keys at the root of an instance
    std::set<std::string> expectedKeys = { "generation", "info", "objects", "overrides", "legacy" };

    // Process each instance
    for (const auto& id : instanceIds) {
        RemoveKeysResult result;
        result.id = id;
        result.success = false;

        // Fetch the instance data
        std::pair<std::string, std::string> instanceData = dbManager_->getEntryById("instances", id);

        if (instanceData.second.empty()) {
            result.error = "Instance not found";
            results.push_back(result);
            continue;
        }

        // Parse the KeyValues data
        auto kvData = ArcadeKeyValues::ParseFromHex(instanceData.second);
        if (!kvData) {
            result.error = "Failed to parse KeyValues data";
            results.push_back(result);
            continue;
        }

        // Navigate to the instance section (root -> "instance")
        ArcadeKeyValues* instanceSection = kvData->GetFirstSubKey();
        if (!instanceSection) {
            result.error = "Invalid data structure";
            results.push_back(result);
            continue;
        }

        // Collect all anomalous keys (keys that are not in the expected set)
        std::vector<std::string> keysToRemove;
        ArcadeKeyValues* child = instanceSection->GetFirstSubKey();
        while (child) {
            const char* keyName = child->GetName();
            if (keyName && keyName[0] != '\0') {
                std::string key(keyName);
                if (expectedKeys.find(key) == expectedKeys.end()) {
                    keysToRemove.push_back(key);
                }
            }
            child = child->GetNextKey();
        }

        // Remove all anomalous keys
        bool allRemoved = true;
        for (const auto& keyToRemove : keysToRemove) {
            if (!instanceSection->RemoveKey(keyToRemove.c_str())) {
                allRemoved = false;
                OutputDebugStringA(("[Library] dbtRemoveAnomalousKeys: Failed to remove key '" + keyToRemove + "' from " + id + "\n").c_str());
            }
        }

        if (!allRemoved) {
            result.error = "Failed to remove some keys";
            results.push_back(result);
            continue;
        }

        // Serialize the modified KeyValues back to hex
        std::string updatedHex = kvData->SerializeToHex();

        // Update in database
        if (dbManager_->updateEntryById("instances", id, updatedHex)) {
            result.success = true;
            result.error = "";
            OutputDebugStringA(("[Library] dbtRemoveAnomalousKeys: Successfully removed " + std::to_string(keysToRemove.size()) + " keys from " + id + "\n").c_str());
        } else {
            result.error = "Failed to update database";
            OutputDebugStringA(("[Library] dbtRemoveAnomalousKeys: Failed to update database for " + id + "\n").c_str());
        }

        results.push_back(result);
    }

    OutputDebugStringA(("[Library] dbtRemoveAnomalousKeys: Processed " + std::to_string(results.size()) + " instances\n").c_str());

    return results;
}
