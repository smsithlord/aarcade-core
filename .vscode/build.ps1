# Build AARcade Core
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot | Split-Path -Parent

Write-Host 'Building AARcade Core...'

# Set up Visual Studio environment
$vsPath = "F:\Installed Programs\Visual Studio Community 2022\VC\Auxiliary\Build\vcvarsall.bat"
$vcpkgInclude = "$root\vcpkg_installed\x64-windows\include"
$vcpkgLib = "$root\vcpkg_installed\x64-windows\lib"

# Build command
$buildCmd = @"
"$vsPath" x64 && cl.exe /Zi /EHsc /nologo /std:c++17 /I"$root\include" /I"$root\aarcade_core" /I"$vcpkgInclude" /Fe"$root\x64\Release\aarcade-core.exe" "$root\aarcade-core.cpp" "$root\aarcade_core\MainApp.cpp" "$root\aarcade_core\Library.cpp" "$root\aarcade_core\JSBridge.cpp" "$root\aarcade_core\ImageLoader.cpp" "$root\aarcade_core\ConsoleLogger.cpp" /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /MACHINE:X64 /LIBPATH:"$root\lib" /LIBPATH:"$vcpkgLib" AppCore.lib Ultralight.lib UltralightCore.lib WebCore.lib sqlite3.lib
"@

# Execute build
cmd.exe /c $buildCmd

if ($LASTEXITCODE -eq 0) {
    Write-Host 'Build complete!'
} else {
    Write-Host 'Build failed!' -ForegroundColor Red
    exit $LASTEXITCODE
}
