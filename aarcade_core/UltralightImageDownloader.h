#ifndef ULTRALIGHTIMAGEDOWNLOADER_H
#define ULTRALIGHTIMAGEDOWNLOADER_H

#include "IImageDownloader.h"
#include <Ultralight/Ultralight.h>
#include <AppCore/AppCore.h>
#include <windows.h>

using namespace ultralight;

/**
 * Ultralight-based image downloader implementation
 * Downloads and renders images to a specific size, useful for thumbnails
 */
class UltralightImageDownloader : public IImageDownloader, public LoadListener {
private:
    RefPtr<Renderer> renderer_;
    RefPtr<View> view_;
    bool done_;
    bool success_;

    std::string currentUrl_;
    std::string currentOutputPath_;
    std::function<void(bool, const std::string&)> currentCallback_;

    int targetWidth_;
    int targetHeight_;

    void debugOutput(const std::string& message) {
        std::string debugMsg = "[UltralightDownloader] " + message;
        OutputDebugStringA((debugMsg + "\n").c_str());
    }

    // Create HTML wrapper that properly sizes the image
    std::string createImageHTML(const std::string& imageUrl, int width, int height) {
        return R"(<!DOCTYPE html>
<html>
<head>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        html, body {
            width: 100%;
            height: 100%;
            overflow: hidden;
            background: transparent;
        }
        #image-container {
            width: 100%;
            height: 100%;
            display: flex;
            align-items: center;
            justify-content: center;
        }
        img {
            max-width: 100%;
            max-height: 100%;
            width: auto;
            height: auto;
            object-fit: contain;
        }
    </style>
</head>
<body>
    <div id="image-container">
        <img src=")" + imageUrl + R"(" id="main-image" />
    </div>
    <script>
        // Log when image loads
        document.getElementById('main-image').addEventListener('load', function() {
            console.log('Image loaded successfully');
        });
        document.getElementById('main-image').addEventListener('error', function() {
            console.error('Failed to load image');
        });
    </script>
</body>
</html>)";
    }

    void renderAndSave() {
        debugOutput("Rendering image...");

        // Render the view
        renderer_->RefreshDisplay(0);
        renderer_->Render();

        // Get the rendered bitmap
        BitmapSurface* bitmap_surface = (BitmapSurface*)view_->surface();
        RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();

        // Save to file
        bitmap->WritePNG(currentOutputPath_.c_str());

        debugOutput("Image rendered and saved: " + currentOutputPath_);
        success_ = true;
    }

public:
    UltralightImageDownloader(RefPtr<Renderer> renderer, int width = 512, int height = 512)
        : renderer_(renderer), done_(false), success_(false), targetWidth_(width), targetHeight_(height) {
        debugOutput("Ultralight downloader initialized (" +
                   std::to_string(width) + "x" + std::to_string(height) + ")");

        if (!renderer_) {
            debugOutput("ERROR: Renderer is null!");
        }
    }

    virtual ~UltralightImageDownloader() {
        view_ = nullptr;
        // Don't clear renderer_ - it's owned by the main app
        debugOutput("Ultralight downloader destroyed");
    }

    virtual std::string getName() const override {
        return "Ultralight";
    }

    void setTargetSize(int width, int height) {
        targetWidth_ = width;
        targetHeight_ = height;
        debugOutput("Target size set to " + std::to_string(width) + "x" + std::to_string(height));
    }

    virtual void downloadImage(const std::string& url,
                               const std::string& outputPath,
                               std::function<void(bool, const std::string&)> callback) override {
        debugOutput("Rendering image from URL: " + url + " -> " + outputPath);

        currentUrl_ = url;
        currentOutputPath_ = outputPath;
        currentCallback_ = callback;
        done_ = false;
        success_ = false;

        // Create view for rendering
        ViewConfig view_config;
        view_config.initial_device_scale = 1.0;
        view_config.is_accelerated = false;

        view_ = renderer_->CreateView(targetWidth_, targetHeight_, view_config, nullptr);
        view_->set_load_listener(this);

        // Load the image wrapped in HTML
        std::string html = createImageHTML(url, targetWidth_, targetHeight_);
        view_->LoadHTML(html.c_str());

        // Note: In a real implementation, this would need to run async
        // For now, we'll process synchronously
        runRenderLoop();
    }

    virtual void cancelAll() override {
        debugOutput("Cancel all downloads requested");
        done_ = true;
    }

    // LoadListener implementation
    virtual void OnFinishLoading(ultralight::View* caller, uint64_t frame_id,
                                bool is_main_frame, const String& url) override {
        if (is_main_frame) {
            debugOutput("Page finished loading");

            // Give images a moment to load
            Sleep(100);

            renderAndSave();
            done_ = true;
        }
    }

    virtual void OnFailLoading(ultralight::View* caller, uint64_t frame_id,
                              bool is_main_frame, const String& url,
                              const String& description, const String& error_domain,
                              int error_code) override {
        debugOutput("Page failed to load");
        success_ = false;
        done_ = true;
    }

private:
    void runRenderLoop() {
        debugOutput("Starting render loop...");

        // Update until loading is done
        while (!done_) {
            renderer_->Update();
            Sleep(10);
        }

        // Final render
        if (success_) {
            debugOutput("Render completed successfully");
            if (currentCallback_) {
                currentCallback_(true, currentOutputPath_);
                currentCallback_ = nullptr;
            }
        } else {
            debugOutput("Render failed");
            if (currentCallback_) {
                currentCallback_(false, "");
                currentCallback_ = nullptr;
            }
        }

        // Clean up view
        view_ = nullptr;
    }
};

#endif // ULTRALIGHTIMAGEDOWNLOADER_H
