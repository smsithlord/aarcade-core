#ifndef CURLIMAGEDOWNLOADER_H
#define CURLIMAGEDOWNLOADER_H

#include "IImageDownloader.h"
#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <windows.h>

/**
 * libcurl-based image downloader implementation
 */
class CurlImageDownloader : public IImageDownloader {
private:
    void debugOutput(const std::string& message) {
        std::string debugMsg = "[CurlDownloader] " + message;
        OutputDebugStringA((debugMsg + "\n").c_str());
    }

    // Callback for curl write function
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t totalSize = size * nmemb;
        std::vector<unsigned char>* buffer = static_cast<std::vector<unsigned char>*>(userp);
        buffer->insert(buffer->end(), (unsigned char*)contents, (unsigned char*)contents + totalSize);
        return totalSize;
    }

    // Callback for curl header function
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
        size_t totalSize = size * nitems;
        std::string* contentType = static_cast<std::string*>(userdata);
        std::string header(buffer, totalSize);

        // Look for Content-Type header
        if (header.find("Content-Type:") == 0 || header.find("content-type:") == 0) {
            size_t colonPos = header.find(':');
            if (colonPos != std::string::npos) {
                *contentType = header.substr(colonPos + 1);
                // Trim whitespace
                size_t start = contentType->find_first_not_of(" \t\r\n");
                size_t end = contentType->find_last_not_of(" \t\r\n");
                if (start != std::string::npos && end != std::string::npos) {
                    *contentType = contentType->substr(start, end - start + 1);
                }
            }
        }

        return totalSize;
    }

    // Detect file extension from Content-Type header or file signature
    std::string detectImageExtension(const std::vector<unsigned char>& data, const std::string& contentType) {
        // Check magic numbers (file signatures) first
        if (data.size() >= 4) {
            // PNG signature: 89 50 4E 47
            if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47) {
                return ".png";
            }
            // JPEG signature: FF D8 FF
            if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
                return ".jpg";
            }
            // GIF signature: 47 49 46 38
            if (data[0] == 0x47 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x38) {
                return ".gif";
            }
            // WebP signature: 52 49 46 46 ... 57 45 42 50
            if (data.size() >= 12 && data[0] == 0x52 && data[1] == 0x49 &&
                data[2] == 0x46 && data[3] == 0x46 && data[8] == 0x57 &&
                data[9] == 0x45 && data[10] == 0x42 && data[11] == 0x50) {
                return ".webp";
            }
        }

        if (data.size() >= 2) {
            // BMP signature: 42 4D
            if (data[0] == 0x42 && data[1] == 0x4D) {
                return ".bmp";
            }
        }

        // Fall back to Content-Type
        if (contentType.find("image/png") != std::string::npos) return ".png";
        if (contentType.find("image/jpeg") != std::string::npos) return ".jpg";
        if (contentType.find("image/jpg") != std::string::npos) return ".jpg";
        if (contentType.find("image/gif") != std::string::npos) return ".gif";
        if (contentType.find("image/webp") != std::string::npos) return ".webp";
        if (contentType.find("image/bmp") != std::string::npos) return ".bmp";

        // Default to jpg if unknown
        return ".jpg";
    }

public:
    CurlImageDownloader() {
        debugOutput("libcurl downloader initialized");
    }

    virtual ~CurlImageDownloader() {
        debugOutput("libcurl downloader destroyed");
    }

    virtual std::string getName() const override {
        return "libcurl";
    }

    virtual void downloadImage(const std::string& url,
                               const std::string& outputPath,
                               std::function<void(bool, const std::string&)> callback) override {
        debugOutput("Downloading: " + url + " -> " + outputPath);

        CURL* curl = curl_easy_init();
        if (!curl) {
            debugOutput("Failed to initialize curl");
            callback(false, "");
            return;
        }

        std::vector<unsigned char> downloadedData;
        std::string contentType;

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &downloadedData);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &contentType);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "ArcadeCore/1.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        curl_easy_setopt(curl, CURLOPT_CAINFO, ".\\resources\\cacert.pem");

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            debugOutput("Curl failed: " + std::string(curl_easy_strerror(res)));
            curl_easy_cleanup(curl);
            callback(false, "");
            return;
        }

        // Check HTTP response code
        long responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_cleanup(curl);

        if (responseCode != 200) {
            debugOutput("HTTP error: " + std::to_string(responseCode));
            callback(false, "");
            return;
        }

        if (downloadedData.empty()) {
            debugOutput("No data downloaded");
            callback(false, "");
            return;
        }

        // Detect image format
        std::string extension = detectImageExtension(downloadedData, contentType);
        debugOutput("Detected format: " + extension + " (Content-Type: " + contentType + ")");

        // Change extension of output path to match detected format
        std::string finalOutputPath = outputPath;
        size_t lastDot = finalOutputPath.find_last_of('.');
        if (lastDot != std::string::npos) {
            finalOutputPath = finalOutputPath.substr(0, lastDot) + extension;
        } else {
            finalOutputPath += extension;
        }

        // Save to output file with correct extension
        std::ofstream outFile(finalOutputPath, std::ios::binary);
        if (!outFile) {
            debugOutput("Failed to open output file: " + finalOutputPath);
            callback(false, "");
            return;
        }

        outFile.write(reinterpret_cast<const char*>(downloadedData.data()), downloadedData.size());
        outFile.close();

        debugOutput("Image saved successfully: " + finalOutputPath);
        callback(true, finalOutputPath);
    }

    virtual void cancelAll() override {
        debugOutput("Cancel all downloads requested");
        // libcurl doesn't support cancellation in this simple implementation
        // A more complex version would track active handles and cancel them
    }
};

#endif // CURLIMAGEDOWNLOADER_H
