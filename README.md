# AARcade Core

A desktop application for arcade game management built with C++ and Ultralight.

## Features

- **Modern UI**: Built with Ultralight for fast, web-based UI rendering
- **Database Management**: SQLite integration for game library management
- **Image Processing**: Ultralight-based image loader with aspect ratio preservation
- **JavaScript Bridge**: Seamless communication between C++ backend and JavaScript frontend

## Technology Stack

- **C++17**: Modern C++ for core application logic
- **Ultralight SDK**: Lightweight HTML/CSS/JavaScript UI framework
- **SQLite**: Embedded database for game metadata
- **vcpkg**: Dependency management (currently no dependencies required)

## Project Structure

```
aarcade-core/
├── aarcade_core/          # Core application source files
│   ├── MainApp.cpp        # Main application logic
│   ├── JSBridge.cpp       # JavaScript-C++ bridge
│   └── *.h                # Header files
├── include/               # Third-party headers
│   ├── Ultralight/        # Ultralight UI framework
│   ├── JavaScriptCore/    # JavaScript engine
│   └── sqlite/            # SQLite database
├── .vscode/               # VS Code configuration
├── aarcade-core.cpp       # Application entry point
└── vcpkg.json            # vcpkg dependency manifest

## Building

### Prerequisites

- Visual Studio 2022 or later
- vcpkg package manager
- Windows SDK 10.0.22621.0 or later

### Build Steps

1. Build using VS Code:
   - Open the project in VS Code
   - Press `Ctrl+Shift+B` to build
   - Or use the task: "Build AARcade Core"

2. Run the application:
   - Press `F5` to debug
   - Or run `x64/Release/aarcade-core.exe`

## Configuration

Configuration is managed through `x64/Release/config.ini`:

```ini
# Database path (relative or absolute)
database_path = "G:\Documents Sym Links\GitHub\aarcade-core\release\x64\library.db"
```

## License

[Add your license here]

## Contributing

[Add contribution guidelines here]
