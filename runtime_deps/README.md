# Runtime Dependencies

This document lists all runtime dependencies (DLLs) required for AARcade Core to run.

## Ultralight SDK DLLs

These DLLs are manually copied from the Ultralight SDK to `lib/` directory:

- **AppCore.dll** - Ultralight Application Core library
- **Ultralight.dll** - Ultralight main rendering library
- **UltralightCore.dll** - Ultralight core functionality
- **WebCore.dll** - Web rendering engine

**Source**: Manually copied from Ultralight SDK installation
**Deployment**: Copied from `lib/` to `x64/Release/` during build

## vcpkg Dependencies

These DLLs are provided by vcpkg packages:

- **libcurl.dll** - HTTP/network library
- **zlib1.dll** - Compression library

**Source**: `vcpkg_installed/x64-windows/bin/`
**Deployment**: Copied during build process

## SQLite Database

- **sqlite3.dll** - SQLite database engine

**Source**: vcpkg package or manual installation
**Location**: `vcpkg_installed/x64-windows/bin/sqlite3.dll` or manually placed in `lib/`
**Deployment**: Copied to `x64/Release/` during build

## Deployment Process

All DLLs are automatically copied to `x64/Release/` by the build process defined in `.vscode/tasks.json`.

### Build Task Order:
1. **Deploy Runtime Files** - Copies DLLs, resources, config, and assets
2. **Compile C++** - Builds aarcade-core.exe

## Adding New Dependencies

When adding new DLL dependencies:

1. Add the DLL to either `lib/` (for manual dependencies) or install via vcpkg
2. Update the "Deploy Runtime Files" task in `.vscode/tasks.json` to copy the new DLL
3. Document the dependency in this README
4. Ensure the DLL is in the same directory as `aarcade-core.exe` at runtime
