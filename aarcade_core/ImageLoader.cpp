#include "ImageLoader.h"
#include "JSBridge.h"

// LoadListener implementation
void ImageLoader::OnDOMReady(ultralight::View* caller, uint64_t frame_id,
                           bool is_main_frame, const String& url) {
    // DOM is ready, now set up the JS bridge
    if (is_main_frame && jsBridge_) {
        debugOutput("DOM ready, setting up JS bridge");
        jsBridge_->setupImageLoaderBridge(caller);
    }
}

void ImageLoader::OnFinishLoading(ultralight::View* caller, uint64_t frame_id,
                                bool is_main_frame, const String& url) {
    if (is_main_frame) {
        debugOutput("Image loader HTML finished loading");
        // The HTML will call onImageLoaderReady via JS bridge
    }
}

void ImageLoader::OnFailLoading(ultralight::View* caller, uint64_t frame_id,
                              bool is_main_frame, const String& url,
                              const String& description, const String& error_domain,
                              int error_code) {
    if (is_main_frame) {
        debugOutput("ERROR: Failed to load image-loader.html");
        isInitialized_ = false;
    }
}
