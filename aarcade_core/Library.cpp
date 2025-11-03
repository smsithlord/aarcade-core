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

std::vector<std::string> Library::constructSchema(const std::string& entryType) {
    OutputDebugStringA(("[Library] constructSchema: Constructing schema for '" + entryType + "'\n").c_str());

    std::vector<std::string> schema;
    std::set<std::string> fieldSet; // Use set to track unique fields

    // Open database if not already open
    if (!openDatabase()) {
        OutputDebugStringA("[Library] constructSchema: Failed to open database\n");
        return schema;
    }

    // Get all entries for this type
    std::vector<std::pair<std::string, std::string>> allEntries = dbManager_->getFirstEntries(entryType, 10000);

    OutputDebugStringA(("[Library] constructSchema: Analyzing " + std::to_string(allEntries.size()) + " entries\n").c_str());

    // Iterate through all entries and collect field names
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

        // Get all child keys (fields) from the data section
        ArcadeKeyValues* field = dataSection->GetFirstSubKey();
        while (field != nullptr) {
            std::string fieldName = field->GetName();
            if (!fieldName.empty()) {
                fieldSet.insert(fieldName);
            }
            field = field->GetNextKey();
        }
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
