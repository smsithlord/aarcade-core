#ifndef LIBRARY_H
#define LIBRARY_H

#include "SQLiteManager.h"
#include "Config.h"
#include "ArcadeKeyValues.h"
#include "ImageLoader.h"
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <set>

/**
 * Library class - Manages the arcade library functionality
 *
 * This class encapsulates all library-related operations including:
 * - Entry browsing (items, apps, instances, maps, models, platforms, types)
 * - Search functionality
 * - Image caching coordination
 * - Entry type management
 *
 * It provides a clean interface between the JavaScript bridge and the
 * underlying database/image caching systems.
 */
class Library {
private:
    SQLiteManager* dbManager_;
    ArcadeConfig* config_;
    ImageLoader* imageLoader_;

    // Helper method for recursive schema construction
    void collectFieldPathsRecursive(ArcadeKeyValues* node, const std::string& currentPath, std::set<std::string>& fieldSet, bool isInstanceData, int depth);

public:
    // Constructor - takes references to required managers
    Library(SQLiteManager* dbManager, ArcadeConfig* config);
    virtual ~Library();

    // Set the image loader (owned by MainApp)
    void setImageLoader(ImageLoader* imageLoader);

    // Entry browsing methods
    std::vector<std::pair<std::string, std::string>> getFirstEntries(const std::string& entryType, int count);
    std::vector<std::pair<std::string, std::string>> getNextEntries(int count);
    std::pair<std::string, std::string> getFirstEntry(const std::string& entryType);
    std::pair<std::string, std::string> getNextEntry();

    // Search methods
    std::vector<std::pair<std::string, std::string>> getFirstSearchResults(const std::string& entryType, const std::string& searchTerm, int count);
    std::vector<std::pair<std::string, std::string>> getNextSearchResults(int count);

    // Image caching methods
    void cacheImage(const std::string& url, std::function<void(const ImageLoadResult&)> callback);
    void processImageCompletions();

    // Utility methods
    std::vector<std::string> getSupportedEntryTypes() const;
    bool openDatabase();

    // Schema construction
    std::vector<std::string> constructSchema(const std::string& entryType);

    // Database tools: Large BLOB detection
    struct LargeBlobEntry {
        std::string id;
        std::string title;
        int sizeBytes;
    };

    std::vector<LargeBlobEntry> dbtFindLargeEntriesInTable(const std::string& tableName, int minSizeBytes);

    // Trim text fields for specified entries
    struct TrimResult {
        std::string id;
        bool success;
        std::string error;
    };

    std::vector<TrimResult> dbtTrimTextFields(const std::string& tableName, const std::vector<std::string>& entryIds, int maxLength);

    // Database maintenance
    struct DatabaseStats {
        std::string filePath;
        int64_t fileSizeBytes;
        int64_t pageCount;
        int64_t pageSize;
        int64_t freePages;
        double fragmentationPercent;
    };

    struct CompactResult {
        bool success;
        std::string error;
        int64_t beforeSizeBytes;
        int64_t afterSizeBytes;
        int64_t spaceSavedBytes;
    };

    DatabaseStats dbtGetDatabaseStats();
    CompactResult dbtCompactDatabase();

    // Anomalous instances detection
    struct AnomalousInstanceEntry {
        std::string id;
        std::vector<std::string> unexpectedKeys;
        int keyCount;
        int generation;
        int legacy;  // -1 = not found, otherwise displays the actual integer value
    };

    std::vector<AnomalousInstanceEntry> dbtFindAnomalousInstances();
    std::string dbtGetInstanceKeyValues(const std::string& instanceId);

    struct RemoveKeysResult {
        std::string id;
        bool success;
        std::string error;
    };

    std::vector<RemoveKeysResult> dbtRemoveAnomalousKeys(const std::vector<std::string>& instanceIds);

    // Database merge tool
    struct MergeEntry {
        std::string id;
        std::string action;  // "merged", "skipped", "overwritten", "failed"
        std::string error;
        int blobSizeBytes;
    };

    struct MergeResult {
        bool success;
        std::string error;
        int totalEntries;
        int mergedCount;
        int skippedCount;
        int overwrittenCount;
        int failedCount;
        std::vector<MergeEntry> entries;  // Detailed log of all operations
    };

    MergeResult dbtMergeDatabase(const std::string& sourcePath, const std::string& tableName, bool skipExisting, bool overwriteIfLarger);

private:
    // Helper function for converting KeyValues to plain text
    std::string keyValuesToPlainText(ArcadeKeyValues* kv, int indent);

public:
    // Legacy method for backwards compatibility
    std::pair<std::string, std::string> getFirstItem();
};

#endif // LIBRARY_H
