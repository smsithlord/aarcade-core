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

std::pair<std::string, std::string> Library::getFirstItem() {
    OutputDebugStringA("[Library] getFirstItem: Getting first item (legacy method)\n");

    // Open database if not already open
    if (!openDatabase()) {
        return std::make_pair("", "");
    }

    return dbManager_->getFirstItem();
}
