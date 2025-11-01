#ifndef IIMAGEDOWNLOADER_H
#define IIMAGEDOWNLOADER_H

#include <string>
#include <functional>

/**
 * Interface for image downloading implementations.
 * Allows swapping between different download backends (libcurl, Steamworks HTTP, etc.)
 */
class IImageDownloader {
public:
    virtual ~IImageDownloader() = default;

    /**
     * Get the name of this downloader implementation
     */
    virtual std::string getName() const = 0;

    /**
     * Download an image from a URL and save it to the specified path.
     *
     * @param url The URL to download from
     * @param outputPath The file path where the image should be saved
     * @param callback Callback function(success, filePath) called when download completes
     *                 NOTE: Callback may be called from a background thread!
     *                       Use a completion queue for thread-safe callback invocation.
     */
    virtual void downloadImage(const std::string& url,
                               const std::string& outputPath,
                               std::function<void(bool, const std::string&)> callback) = 0;

    /**
     * Cancel all pending downloads (called on shutdown)
     */
    virtual void cancelAll() = 0;
};

#endif // IIMAGEDOWNLOADER_H
