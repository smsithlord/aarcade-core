# Runtime Dependencies

This document lists all runtime dependencies (DLLs) required for AARcade Core to run.

## Ultralight SDK DLLs

These DLLs are stored in `lib/` directory and tracked in source control:

- **AppCore.dll** - Ultralight Application Core library
- **Ultralight.dll** - Ultralight main rendering library
- **UltralightCore.dll** - Ultralight core functionality
- **WebCore.dll** - Web rendering engine

**Source**: Manually copied from Ultralight SDK installation to `lib/`
**Location**: `lib/` (version controlled)
**Deployment**: Copied from `lib/` to `x64/Release/` during build

## Ultralight Resources

Required resource files for Ultralight runtime:

- **cacert.pem** - CA certificate bundle for HTTPS support
- **icudt67l.dat** - ICU data file for Unicode support

**Location**: `src/resources/` (version controlled)
**Deployment**: Copied to `x64/Release/resources/` during build

## SQLite Database

The project currently has no vcpkg dependencies, but SQLite support is available:

- **sqlite3.dll** - SQLite database engine (optional, for future use)
- **sqlite3.lib** - SQLite import library

**Source**: Can be installed via vcpkg if needed
**Note**: Currently not required for the core application

## Import Libraries

The following `.lib` files are also stored in `lib/` and tracked in source control:

- **AppCore.lib** - Ultralight import library
- **Ultralight.lib** - Ultralight import library
- **UltralightCore.lib** - Ultralight import library
- **WebCore.lib** - Ultralight import library
- **sqlite3.lib** - SQLite import library

These are required for linking during compilation.

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
