#include "ImageLoader.h"
#include "JSBridge.h"

// LoadListener implementation
void ImageLoader::OnDOMReady(ultralight::View* caller, uint64_t frame_id,
                           bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    debugOutput("OnDOMReady - URL: " + urlStr + ", main_frame: " + (is_main_frame ? "true" : "false"));

    // DOM is ready, now set up the JS bridge
    if (is_main_frame && jsBridge_) {
        debugOutput("DOM ready, setting up JS bridge");
        jsBridge_->setupImageLoaderBridge(caller);
    }
}

void ImageLoader::OnFinishLoading(ultralight::View* caller, uint64_t frame_id,
                                bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    debugOutput("OnFinishLoading - URL: " + urlStr + ", main_frame: " + (is_main_frame ? "true" : "false"));

    if (is_main_frame) {
        debugOutput("Image loader HTML finished loading");
        // The HTML will call onImageLoaderReady via JS bridge
    }
}

void ImageLoader::OnFailLoading(ultralight::View* caller, uint64_t frame_id,
                              bool is_main_frame, const String& url,
                              const String& description, const String& error_domain,
                              int error_code) {
    std::string urlStr(url.utf8().data());
    std::string descStr(description.utf8().data());
    std::string domainStr(error_domain.utf8().data());

    std::string errorMsg = "OnFailLoading - URL: " + urlStr +
                          ", main_frame: " + (is_main_frame ? "true" : "false") +
                          ", description: " + descStr +
                          ", domain: " + domainStr +
                          ", code: " + std::to_string(error_code);
    debugOutput(errorMsg);

    if (is_main_frame) {
        debugOutput("ERROR: Failed to load image-loader.html (main frame)");
        isInitialized_ = false;
    }
}
