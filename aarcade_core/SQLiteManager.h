#ifndef SQLITE_MANAGER_H
#define SQLITE_MANAGER_H

#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "sqlite/sqlite3.h"

class SQLiteManager {
private:
    sqlite3* db;

    // Entry browsing state
    sqlite3_stmt* activeEntryStmt;
    std::string currentEntryType;
    bool hasActiveQuery;

    // Search state
    sqlite3_stmt* activeSearchStmt;
    std::string currentSearchTerm;
    std::string currentSearchEntryType;
    bool hasActiveSearchQuery;
    int searchOffset;

    void debugOutput(const std::string& message) {
        std::string debugMsg = "[SQLiteManager] " + message;
        OutputDebugStringA((debugMsg + "\n").c_str());
    }

    // Helper to get table name from entry type
    std::string getTableNameForType(const std::string& entryType) {
        if (entryType == "items") {
            return "items";
        }
        else if (entryType == "apps") {
            return "apps";
        }
        else if (entryType == "instances") {
            return "instances";
        }
        else if (entryType == "maps") {
            return "maps";
        }
        else if (entryType == "models") {
            return "models";
        }
        else if (entryType == "platforms") {
            return "platforms";
        }
        else if (entryType == "types") {
            return "types";
        }

        // Unknown type
        debugOutput("Unknown entry type: " + entryType);
        return "";
    }

    // Helper to extract title from KeyValues hex data for search
    std::string extractTitleFromKeyValues(const std::string& hexData) {
        if (hexData.empty()) {
            return "";
        }

        try {
            // Convert hex to bytes
            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < hexData.length(); i += 2) {
                if (i + 1 < hexData.length()) {
                    uint8_t byte = static_cast<uint8_t>(std::stoi(hexData.substr(i, 2), nullptr, 16));
                    bytes.push_back(byte);
                }
            }

            if (bytes.empty()) {
                return "";
            }

            // Simple approach: scan for "title" string anywhere in the data
            const std::string titleKey = "title";
            for (size_t pos = 0; pos < bytes.size() - titleKey.length() - 3; pos++) {
                // Look for the title key
                bool foundTitleKey = true;
                for (size_t i = 0; i < titleKey.length(); i++) {
                    if (bytes[pos + i] != static_cast<uint8_t>(titleKey[i])) {
                        foundTitleKey = false;
                        break;
                    }
                }

                if (foundTitleKey) {
                    // Check if followed by null terminator
                    size_t afterKey = pos + titleKey.length();
                    if (afterKey < bytes.size() && bytes[afterKey] == 0) {
                        // Check if there's a string value after it
                        size_t valueStart = afterKey + 1;
                        if (valueStart < bytes.size()) {
                            // Read the string value
                            std::string title;
                            for (size_t i = valueStart; i < bytes.size() && bytes[i] != 0; i++) {
                                title += static_cast<char>(bytes[i]);
                            }

                            if (!title.empty()) {
                                return title;
                            }
                        }
                    }
                }
            }
        }
        catch (const std::exception& e) {
            debugOutput("Error extracting title from KeyValues: " + std::string(e.what()));
        }

        return "";
    }

public:
    SQLiteManager() : db(nullptr), activeEntryStmt(nullptr), hasActiveQuery(false),
        activeSearchStmt(nullptr), hasActiveSearchQuery(false), searchOffset(0) {
    }

    ~SQLiteManager() {
        resetEntryQuery();
        resetSearchQuery();
        if (db) {
            sqlite3_close(db);
            debugOutput("Database connection closed.");
        }
    }

    bool openDatabase(const std::string& dbPath) {
        int rc = sqlite3_open(dbPath.c_str(), &db);

        if (rc != SQLITE_OK) {
            debugOutput("Cannot open database: " + std::string(sqlite3_errmsg(db)));
            return false;
        }

        debugOutput("Database opened successfully: " + dbPath);
        return true;
    }

    std::string getVersion() {
        return std::string(sqlite3_libversion());
    }

    bool getDatabaseInfo() {
        if (!db) {
            debugOutput("No database connection available.");
            return false;
        }

        debugOutput("=== Database Information ===");

        // Get database file size
        const char* dbPath = sqlite3_db_filename(db, "main");
        if (dbPath) {
            WIN32_FILE_ATTRIBUTE_DATA fileInfo;
            if (GetFileAttributesExA(dbPath, GetFileExInfoStandard, &fileInfo)) {
                LARGE_INTEGER fileSize;
                fileSize.LowPart = fileInfo.nFileSizeLow;
                fileSize.HighPart = fileInfo.nFileSizeHigh;

                std::string sizeMsg = "Database file size: " + std::to_string(fileSize.QuadPart) + " bytes";
                debugOutput(sizeMsg);
            }
        }

        // Get page count and page size
        sqlite3_stmt* stmt;
        const char* sql = "PRAGMA page_count;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int pageCount = sqlite3_column_int(stmt, 0);
                debugOutput("Total pages: " + std::to_string(pageCount));
            }
            sqlite3_finalize(stmt);
        }

        sql = "PRAGMA page_size;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int pageSize = sqlite3_column_int(stmt, 0);
                debugOutput("Page size: " + std::to_string(pageSize) + " bytes");
            }
            sqlite3_finalize(stmt);
        }

        // Get encoding
        sql = "PRAGMA encoding;";
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const char* encoding = (const char*)sqlite3_column_text(stmt, 0);
                debugOutput("Text encoding: " + std::string(encoding ? encoding : "Unknown"));
            }
            sqlite3_finalize(stmt);
        }

        return true;
    }

    bool listTables() {
        if (!db) {
            debugOutput("No database connection available.");
            return false;
        }

        debugOutput("=== Database Tables ===");

        const char* sql = "SELECT name, type FROM sqlite_master WHERE type IN ('table', 'view') ORDER BY name;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            debugOutput("Failed to prepare table list query: " + std::string(sqlite3_errmsg(db)));
            return false;
        }

        int tableCount = 0;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* name = (const char*)sqlite3_column_text(stmt, 0);
            const char* type = (const char*)sqlite3_column_text(stmt, 1);

            std::string tableInfo = std::string(type ? type : "unknown") + ": " + std::string(name ? name : "unnamed");
            debugOutput("  " + tableInfo);
            tableCount++;
        }

        sqlite3_finalize(stmt);

        if (tableCount == 0) {
            debugOutput("  No tables found in database.");
        }
        else {
            debugOutput("Total tables/views: " + std::to_string(tableCount));
        }

        return true;
    }

    bool getTableInfo(const std::string& tableName) {
        if (!db) {
            debugOutput("No database connection available.");
            return false;
        }

        debugOutput("=== Table Info: " + tableName + " ===");

        // Get row count
        std::string countSql = "SELECT COUNT(*) FROM \"" + tableName + "\";";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, countSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int rowCount = sqlite3_column_int(stmt, 0);
                debugOutput("  Row count: " + std::to_string(rowCount));
            }
            sqlite3_finalize(stmt);
        }

        // Get column info
        std::string pragmaSql = "PRAGMA table_info(\"" + tableName + "\");";
        if (sqlite3_prepare_v2(db, pragmaSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            debugOutput("  Columns:");
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                const char* type = (const char*)sqlite3_column_text(stmt, 2);
                int notNull = sqlite3_column_int(stmt, 3);
                int pk = sqlite3_column_int(stmt, 5);

                std::string columnInfo = "    " + std::string(name ? name : "unnamed") +
                    " (" + std::string(type ? type : "unknown") + ")";
                if (pk) columnInfo += " PRIMARY KEY";
                if (notNull) columnInfo += " NOT NULL";

                debugOutput(columnInfo);
            }
            sqlite3_finalize(stmt);
        }

        return true;
    }

    // Get list of supported entry types
    std::vector<std::string> getSupportedEntryTypes() {
        return {
            "items",
            "apps",
            "instances",
            "maps",
            "models",
            "platforms",
            "types"
        };
    }

    // Search entries by title with pagination
    std::vector<std::pair<std::string, std::string>> getFirstSearchResults(const std::string& entryType, const std::string& searchTerm, int count = 50) {
        if (!prepareSearchQuery(entryType, searchTerm)) {
            return {};
        }

        return getNextSearchResults(count);
    }

    std::vector<std::pair<std::string, std::string>> getNextSearchResults(int count) {
        if (!hasActiveSearchQuery || !activeSearchStmt) {
            debugOutput("No active search query.");
            return {};
        }

        if (count <= 0 || count > 1000) {
            count = 50;
        }

        std::vector<std::pair<std::string, std::string>> results;

        // Convert search term to lowercase for case-insensitive search
        std::string lowerSearchTerm = currentSearchTerm;
        std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(), ::tolower);

        int foundCount = 0;
        int checkedCount = 0;

        // Continue until we find enough matches OR reach end of database
        while (foundCount < count) {
            int stepResult = sqlite3_step(activeSearchStmt);

            if (stepResult == SQLITE_ROW) {
                checkedCount++;

                const char* id = (const char*)sqlite3_column_text(activeSearchStmt, 0);
                const void* valueBlob = sqlite3_column_blob(activeSearchStmt, 1);
                int valueSize = sqlite3_column_bytes(activeSearchStmt, 1);

                if (!id || !valueBlob || valueSize <= 0) {
                    continue;
                }

                // Convert binary data to hex string
                const unsigned char* bytes = static_cast<const unsigned char*>(valueBlob);
                std::string hexString;
                hexString.reserve(valueSize * 2);

                for (int i = 0; i < valueSize; i++) {
                    char hexByte[3];
                    sprintf_s(hexByte, sizeof(hexByte), "%02x", bytes[i]);
                    hexString += hexByte;
                }

                // Extract title and check if it matches search term
                std::string title = extractTitleFromKeyValues(hexString);

                if (!title.empty()) {
                    // Convert title to lowercase for case-insensitive comparison
                    std::string lowerTitle = title;
                    std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);

                    // Check if search term is found in title
                    if (lowerTitle.find(lowerSearchTerm) != std::string::npos) {
                        results.push_back({ std::string(id), hexString });
                        foundCount++;
                    }
                }

                // Log progress every 5000 entries for very large searches
                if (checkedCount % 5000 == 0) {
                    debugOutput("Search progress: checked " + std::to_string(checkedCount) + " entries, found " + std::to_string(foundCount) + " matches");
                }
            }
            else if (stepResult == SQLITE_DONE) {
                // Reached end of database
                break;
            }
            else {
                // Database error
                debugOutput("Database error during search: " + std::string(sqlite3_errmsg(db)));
                break;
            }
        }

        if (results.empty() || foundCount < count) {
            // Either no results found, or we reached end of database
            debugOutput("Search completed for '" + currentSearchTerm + "' - found " + std::to_string(foundCount) + " final results");
            resetSearchQuery();
        }

        return results;
    }

    // Search query management
    void resetSearchQuery() {
        if (activeSearchStmt) {
            sqlite3_finalize(activeSearchStmt);
            activeSearchStmt = nullptr;
        }
        hasActiveSearchQuery = false;
        currentSearchTerm.clear();
        currentSearchEntryType.clear();
        searchOffset = 0;
        debugOutput("Search query reset");
    }

    bool prepareSearchQuery(const std::string& entryType, const std::string& searchTerm) {
        if (!db) {
            debugOutput("No database connection available.");
            return false;
        }

        if (searchTerm.empty()) {
            debugOutput("Empty search term provided.");
            return false;
        }

        // Reset any existing search query
        resetSearchQuery();

        std::string tableName = getTableNameForType(entryType);
        if (tableName.empty()) {
            return false;
        }

        // Prepare SQL to get all entries - we'll filter in code since title is embedded in binary data
        std::string sql = "SELECT id, value FROM " + tableName + " ORDER BY id;";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &activeSearchStmt, nullptr) != SQLITE_OK) {
            debugOutput("Failed to prepare search query for " + entryType + ": " + std::string(sqlite3_errmsg(db)));
            return false;
        }

        currentSearchTerm = searchTerm;
        currentSearchEntryType = entryType;
        hasActiveSearchQuery = true;
        searchOffset = 0;

        debugOutput("Prepared search query for type: " + entryType + ", term: '" + searchTerm + "'");
        return true;
    }

    // Entry browsing system
    void resetEntryQuery() {
        if (activeEntryStmt) {
            sqlite3_finalize(activeEntryStmt);
            activeEntryStmt = nullptr;
        }
        hasActiveQuery = false;
        currentEntryType.clear();
        debugOutput("Entry query reset");
    }

    bool prepareEntryQuery(const std::string& entryType) {
        if (!db) {
            debugOutput("No database connection available.");
            return false;
        }

        // Reset any existing query
        resetEntryQuery();

        std::string tableName = getTableNameForType(entryType);
        if (tableName.empty()) {
            return false;  // Error already logged in getTableNameForType
        }

        std::string sql = "SELECT id, value FROM " + tableName + " ORDER BY id;";

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &activeEntryStmt, nullptr) != SQLITE_OK) {
            debugOutput("Failed to prepare entry query for " + entryType + ": " + std::string(sqlite3_errmsg(db)));
            return false;
        }

        currentEntryType = entryType;
        hasActiveQuery = true;
        debugOutput("Prepared entry query for type: " + entryType + " (table: " + tableName + ")");
        return true;
    }

    std::pair<std::string, std::string> getFirstEntry(const std::string& entryType) {
        if (!prepareEntryQuery(entryType)) {
            return { "", "" };
        }

        return getNextEntry();
    }

    std::pair<std::string, std::string> getNextEntry() {
        if (!hasActiveQuery || !activeEntryStmt) {
            debugOutput("No active entry query.");
            return { "", "" };
        }

        bool bEntryExists = false;
        while (!bEntryExists) {
            int result = sqlite3_step(activeEntryStmt);
            if (result == SQLITE_ROW) {
                const char* id = (const char*)sqlite3_column_text(activeEntryStmt, 0);
                const void* valueBlob = sqlite3_column_blob(activeEntryStmt, 1);
                int valueSize = sqlite3_column_bytes(activeEntryStmt, 1);

                std::pair<std::string, std::string> entry = { "", "" };

                if (id) {
                    entry.first = std::string(id);
                    if (entry.first == "") {
                        continue;
                    }
                    else {
                        bEntryExists = true;
                    }
                }

                if (valueBlob && valueSize > 0) {
                    // Convert binary data to hex string
                    const unsigned char* bytes = static_cast<const unsigned char*>(valueBlob);
                    std::string hexString;
                    hexString.reserve(valueSize * 2);

                    for (int i = 0; i < valueSize; i++) {
                        char hexByte[3];
                        sprintf_s(hexByte, sizeof(hexByte), "%02x", bytes[i]);
                        hexString += hexByte;
                    }

                    entry.second = hexString;
                }

                debugOutput("Retrieved " + currentEntryType + " entry with ID: " + entry.first);
                return entry;
            }
            else if (result == SQLITE_DONE) {
                debugOutput("No more " + currentEntryType + " entries available.");
                resetEntryQuery();
                return { "", "" };
            }
            else {
                debugOutput("Error stepping through " + currentEntryType + " entries: " + std::string(sqlite3_errmsg(db)));
                resetEntryQuery();
                return { "", "" };
            }
        }
    }

    std::vector<std::pair<std::string, std::string>> getFirstEntries(const std::string& entryType, int count) {
        if (!prepareEntryQuery(entryType)) {
            return {};
        }

        return getNextEntries(count);
    }

    std::vector<std::pair<std::string, std::string>> getNextEntries(int count) {
        std::vector<std::pair<std::string, std::string>> entries;

        for (int i = 0; i < count; i++) {
            std::pair<std::string, std::string> entry = getNextEntry();
            if (entry.first.empty() && entry.second.empty()) {
                // No more entries
                break;
            }
            entries.push_back(entry);
        }

        std::string typeInfo = hasActiveQuery ? currentEntryType : "unknown";
        debugOutput("Retrieved " + std::to_string(entries.size()) + " " + typeInfo + " entries");
        return entries;
    }

    // Legacy method - keep for backwards compatibility with existing debug functionality
    std::pair<std::string, std::string> getFirstItem() {
        return getFirstEntry("items");
    }
};

#endif