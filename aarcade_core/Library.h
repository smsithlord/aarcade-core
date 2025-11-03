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

    // Legacy method for backwards compatibility
    std::pair<std::string, std::string> getFirstItem();
};

#endif // LIBRARY_H
