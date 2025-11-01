#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <string>
#include <functional>
#include <windows.h>
#include <queue>
#include <mutex>
#include <memory>
#include <algorithm>
#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>

using namespace ultralight;

// Forward declaration
class JSBridge;

// Result structure for image loading
struct ImageLoadResult {
    bool success;
    std::string filePath;
    std::string url;
};

/**
 * ImageLoader - Ultralight view-based image renderer
 *
 * This class manages a dedicated 512x512 Ultralight view for rendering images to cache.
 * It loads image-loader.html and communicates via JS bridge to load images on-demand.
 *
 * Architecture:
 * - Single offscreen View (512x512)
 * - Loads images through HTML/JS (no direct HTTP)
 * - Renders and saves to cache when JS notifies image is loaded
 * - Uses Kodi-style CRC32 hashing for cache paths
 * - No threading (Ultralight handles async)
 */
class ImageLoader : public LoadListener {
private:
    RefPtr<Renderer> renderer_;
    RefPtr<View> view_;
    JSBridge* jsBridge_;  // Weak pointer, owned by MainApp

    std::string cacheBasePath_;
    bool isInitialized_;
    bool isImageReady_;

    std::string currentUrl_;
    std::function<void(const ImageLoadResult&)> currentCallback_;

    // Queue for pending load requests
    struct LoadJob {
        std::string url;
        std::function<void(const ImageLoadResult&)> callback;
    };

    std::queue<LoadJob> jobQueue_;
    std::queue<ImageLoadResult> completionQueue_;
    std::mutex queueMutex_;
    std::mutex completionMutex_;

    void debugOutput(const std::string& message) {
        std::string debugMsg = "[ImageLoader] " + message;
        OutputDebugStringA((debugMsg + "\n").c_str());
    }

    // Normalize URL for consistent hashing (case insensitive, forward slashes)
    std::string normalizeUrl(const std::string& url) {
        std::string normalized = url;

        // Convert to lowercase
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);

        // Replace backslashes with forward slashes
        for (char& c : normalized) {
            if (c == '\\') {
                c = '/';
            }
        }

        return normalized;
    }

    // XBMC/Kodi CRC32 hashing algorithm
    std::string calculateKodiHash(const std::string& normalizedUrl) {
        // CRC32 lookup table (standard CRC32 polynomial 0xEDB88320)
        static const uint32_t crc32_table[256] = {
            0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
            0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
            0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
            0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
            0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
            0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
            0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
            0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
            0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
            0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
            0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
            0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
            0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
            0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
            0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
            0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
            0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
            0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
            0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
            0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
            0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
            0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
            0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
            0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
            0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
            0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
            0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
            0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
            0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
            0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
            0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
            0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
            0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
            0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
            0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
            0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
            0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
            0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
            0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
            0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
            0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
            0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
            0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
        };

        uint32_t crc = 0xFFFFFFFF;
        for (char c : normalizedUrl) {
            crc = crc32_table[(crc ^ static_cast<uint8_t>(c)) & 0xFF] ^ (crc >> 8);
        }
        crc ^= 0xFFFFFFFF;

        // Convert to 8-character hex string (lowercase)
        char hash[9];
        sprintf_s(hash, sizeof(hash), "%08x", crc);
        return std::string(hash);
    }

    // Get cache file path using Kodi-style directory structure
    std::string getCacheFilePath(const std::string& url) {
        std::string normalized = normalizeUrl(url);
        std::string hash = calculateKodiHash(normalized);

        // Create subfolder based on first character of hash (like Kodi)
        std::string subfolder = hash.substr(0, 1);
        std::string cachePath = cacheBasePath_ + "\\" + subfolder;

        // Ensure directory exists
        CreateDirectoryA(cachePath.c_str(), NULL);

        // ImageLoader always saves as PNG
        return cachePath + "\\" + hash + ".png";
    }

    // Check if cached file exists
    std::string getCachedFilePath(const std::string& url) {
        std::string filePath = getCacheFilePath(url);

        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        if (GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &fileInfo) != 0) {
            return filePath;
        }

        return "";
    }

    // Process the next job in queue
    void processNextJob() {
        LoadJob job;

        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            if (jobQueue_.empty()) {
                debugOutput("No more jobs in queue");
                return;
            }

            job = jobQueue_.front();
            jobQueue_.pop();
        }

        debugOutput("Processing job for URL: " + job.url);

        // Check cache first
        std::string cachedPath = getCachedFilePath(job.url);
        if (!cachedPath.empty()) {
            debugOutput("Image already cached: " + cachedPath);
            std::lock_guard<std::mutex> lock(completionMutex_);
            completionQueue_.push({ true, cachedPath, job.url });

            // Process next job
            processNextJob();
            return;
        }

        // Not cached, need to load and render
        currentUrl_ = job.url;
        currentCallback_ = job.callback;
        isImageReady_ = false;

        // Call JS to load the image
        loadImageInView(job.url);
    }

    // Call JavaScript to load image in the view
    void loadImageInView(const std::string& url) {
        if (!view_ || !isInitialized_) {
            debugOutput("ERROR: View not initialized!");
            std::lock_guard<std::mutex> lock(completionMutex_);
            completionQueue_.push({ false, "", url });
            processNextJob();
            return;
        }

        debugOutput("Calling JS to load image: " + url);

        // Acquire JS context
        auto scoped_context = view_->LockJSContext();
        JSContextRef ctx = (*scoped_context);
        JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

        // Get the loadImageUrl function
        JSStringRef funcName = JSStringCreateWithUTF8CString("loadImageUrl");
        JSValueRef loadImageFunc = JSObjectGetProperty(ctx, globalObj, funcName, nullptr);
        JSStringRelease(funcName);

        if (JSValueIsObject(ctx, loadImageFunc)) {
            // Create URL argument
            JSStringRef urlStr = JSStringCreateWithUTF8CString(url.c_str());
            JSValueRef urlArg = JSValueMakeString(ctx, urlStr);
            JSStringRelease(urlStr);

            // Call the function
            JSValueRef args[] = { urlArg };
            JSObjectCallAsFunction(ctx, (JSObjectRef)loadImageFunc, nullptr, 1, args, nullptr);

            debugOutput("JS function called successfully");
        } else {
            debugOutput("ERROR: loadImageUrl function not found!");
            std::lock_guard<std::mutex> lock(completionMutex_);
            completionQueue_.push({ false, "", url });
            processNextJob();
        }
    }

    // Render and save the current image
    void renderAndSave() {
        debugOutput("Rendering image...");

        // Render the view
        renderer_->RefreshDisplay(0);
        renderer_->Render();

        // Get the rendered bitmap
        BitmapSurface* bitmap_surface = (BitmapSurface*)view_->surface();
        RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();

        // Get output path
        std::string outputPath = getCacheFilePath(currentUrl_);

        // Save to file
        bitmap->WritePNG(outputPath.c_str());

        debugOutput("Image rendered and saved: " + outputPath);

        // Queue completion
        {
            std::lock_guard<std::mutex> lock(completionMutex_);
            completionQueue_.push({ true, outputPath, currentUrl_ });
        }

        // Process next job
        processNextJob();
    }

public:
    ImageLoader(RefPtr<Renderer> renderer, JSBridge* jsBridge)
        : renderer_(renderer), jsBridge_(jsBridge), isInitialized_(false), isImageReady_(false) {

        debugOutput("Initializing ImageLoader...");

        // Set default cache path
        cacheBasePath_ = ".\\cache\\urls";

        // Ensure cache directory exists
        CreateDirectoryA(".\\cache", NULL);
        CreateDirectoryA(cacheBasePath_.c_str(), NULL);

        // Create 512x512 offscreen view
        ViewConfig view_config;
        view_config.initial_device_scale = 1.0;
        view_config.is_accelerated = false;

        view_ = renderer_->CreateView(512, 512, view_config, nullptr);
        view_->set_load_listener(this);

        // Load the image-loader.html file (relative to executable)
        std::string resourcePath = "..\\..\\resources\\image-loader.html";

        // Get absolute path for debug output
        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        std::string fullPath = std::string(currentDir) + "\\" + resourcePath;
        debugOutput("Loading HTML from: " + fullPath);

        // Load using relative path
        String file_url = String("file:///") + String(fullPath.c_str());
        view_->LoadURL(file_url);
    }

    virtual ~ImageLoader() {
        view_ = nullptr;
        debugOutput("ImageLoader destroyed");
    }

    // Set custom cache directory
    void setCacheDirectory(const std::string& path) {
        cacheBasePath_ = path;
        CreateDirectoryA(cacheBasePath_.c_str(), NULL);
        debugOutput("Cache directory set to: " + cacheBasePath_);
    }

    // Get the view (for JS bridge setup)
    View* getView() const {
        return view_.get();
    }

    // Load and cache an image URL
    void loadAndCacheImage(const std::string& url, std::function<void(const ImageLoadResult&)> callback) {
        debugOutput("Request to load image: " + url);

        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            jobQueue_.push({ url, callback });

            // If this is the first job and we're initialized, start processing
            if (jobQueue_.size() == 1 && isInitialized_) {
                processNextJob();
            }
        }
    }

    // Called from JS bridge when image is loaded and ready
    void onImageLoaded(bool success, const std::string& url) {
        debugOutput("onImageLoaded called: " + url + " (success: " + (success ? "true" : "false") + ")");

        if (url != currentUrl_) {
            debugOutput("WARNING: URL mismatch! Expected: " + currentUrl_ + ", Got: " + url);
        }

        if (success) {
            isImageReady_ = true;
            renderAndSave();
        } else {
            debugOutput("Image load failed");
            std::lock_guard<std::mutex> lock(completionMutex_);
            completionQueue_.push({ false, "", url });
            processNextJob();
        }
    }

    // Called from JS bridge when image loader HTML is ready
    void onImageLoaderReady() {
        debugOutput("Image loader HTML ready");
        isInitialized_ = true;

        // Start processing queued jobs if any
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (!jobQueue_.empty()) {
            processNextJob();
        }
    }

    // Process completed renders - MUST be called from main thread
    void processCompletions() {
        std::lock_guard<std::mutex> lock(completionMutex_);

        while (!completionQueue_.empty()) {
            ImageLoadResult result = completionQueue_.front();
            completionQueue_.pop();

            // Find and call the callback
            // Note: The callback is stored in currentCallback_ during processing
            if (currentCallback_) {
                currentCallback_(result);
                currentCallback_ = nullptr;
            }
        }
    }

    // LoadListener implementation
    virtual void OnDOMReady(ultralight::View* caller, uint64_t frame_id,
                           bool is_main_frame, const String& url) override {
        // DOM is ready, now set up the JS bridge
        if (is_main_frame && jsBridge_) {
            debugOutput("DOM ready, setting up JS bridge");
            jsBridge_->setupImageLoaderBridge(caller);
        }
    }

    virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id,
                                bool is_main_frame, const String& url) override {
        if (is_main_frame) {
            debugOutput("Image loader HTML finished loading");
            // The HTML will call onImageLoaderReady via JS bridge
        }
    }

    virtual void OnFailLoading(ultralight::View* caller, uint64_t frame_id,
                              bool is_main_frame, const String& url,
                              const String& description, const String& error_domain,
                              int error_code) override {
        if (is_main_frame) {
            debugOutput("ERROR: Failed to load image-loader.html");
            isInitialized_ = false;
        }
    }
};

#endif // IMAGELOADER_H
