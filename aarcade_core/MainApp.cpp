#include "MainApp.h"
#include <windows.h>

MainApp::MainApp() : jsBridge_(&dbManager_, &config_) {
    ///
    /// Debug: Show current working directory
    ///
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    std::string debugMsg = "[MainApp] Current working directory: " + std::string(currentDir) + "\n";
    OutputDebugStringA(debugMsg.c_str());

    ///
    /// Load configuration
    ///
    config_.loadFromFile("config.ini");

    ///
    /// Create our main App instance with proper settings
    ///
    Settings settings;
    settings.file_system_path = "./";  // Set filesystem base path to current directory

    app_ = App::Create(settings);

    ///
    /// Pass the app to JSBridge so it can access the renderer
    ///
    jsBridge_.setApp(app_);

    ///
    /// Initialize ImageLoader with the renderer and JSBridge
    ///
    imageLoader_ = std::make_unique<ImageLoader>(app_->renderer(), &jsBridge_);

    // Set the image loader reference in JSBridge
    jsBridge_.setImageLoader(imageLoader_.get());

    // Set cache directory
    imageLoader_->setCacheDirectory(".\\cache\\urls");

    // Note: JS bridge will be set up automatically when image-loader.html DOM is ready
    OutputDebugStringA("[MainApp] ImageLoader initialized\n");

    ///
    /// Create our Window.
    ///
    window_ = Window::Create(app_->main_monitor(), 900, 600, false, kWindowFlags_Titled);

    ///
    /// Set the title of our window.
    ///
    window_->SetTitle("Arcade Core - SQLite Debug");

    ///
    /// Create a web-content overlay that spans the entire window.
    ///
    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);

    ///
    /// Register listeners
    ///
    window_->set_listener(this);
    overlay_->view()->set_view_listener(this);
    overlay_->view()->set_load_listener(this);

    ///
    /// Load a local HTML file into our overlay's View
    ///
    std::string loadUrl = "file:///assets/library.html";
    debugMsg = "[MainApp] Loading URL: " + loadUrl + "\n";
    OutputDebugStringA(debugMsg.c_str());

    overlay_->view()->LoadURL(loadUrl.c_str());
}

MainApp::~MainApp() {}

void MainApp::OnClose(ultralight::Window* window) {
    app_->Quit();
}

void MainApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
    // Resize overlay to match window
    overlay_->Resize(width, height);
}

void MainApp::OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor) {
    window_->SetCursor(cursor);
}

void MainApp::OnBeginLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    std::string debugMsg = "[MainApp] OnBeginLoading: " + urlStr + " (main_frame=" + (is_main_frame ? "true" : "false") + ")\n";
    OutputDebugStringA(debugMsg.c_str());
}

void MainApp::OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    std::string debugMsg = "[MainApp] OnFinishLoading: " + urlStr + " (main_frame=" + (is_main_frame ? "true" : "false") + ")\n";
    OutputDebugStringA(debugMsg.c_str());
}

void MainApp::OnFailLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url, const String& description, const String& error_domain, int error_code) {
    std::string urlStr(url.utf8().data());
    std::string descStr(description.utf8().data());
    std::string domainStr(error_domain.utf8().data());
    std::string debugMsg = "[MainApp] OnFailLoading: " + urlStr + "\n";
    debugMsg += "  Error: " + descStr + "\n";
    debugMsg += "  Domain: " + domainStr + "\n";
    debugMsg += "  Code: " + std::to_string(error_code) + "\n";
    OutputDebugStringA(debugMsg.c_str());
}

void MainApp::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    std::string debugMsg = "[MainApp] OnDOMReady: " + urlStr + " (main_frame=" + (is_main_frame ? "true" : "false") + ")\n";
    OutputDebugStringA(debugMsg.c_str());
}

void MainApp::OnWindowObjectReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
    std::string urlStr(url.utf8().data());
    std::string debugMsg = "[MainApp] OnWindowObjectReady: " + urlStr + " (main_frame=" + (is_main_frame ? "true" : "false") + ")\n";
    OutputDebugStringA(debugMsg.c_str());

    // Delegate JavaScript bridge setup to JSBridge class
    jsBridge_.setupJavaScriptBridge(caller, frame_id, is_main_frame, url);
}

void MainApp::Run() {
    app_->Run();
}