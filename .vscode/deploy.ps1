# Deploy Runtime Files
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot | Split-Path -Parent

Write-Host 'Deploying runtime files...'

# Create output directory
New-Item -ItemType Directory -Path "$root\x64\Release" -Force | Out-Null

# Copy everything from src/
Copy-Item -Path "$root\src\*" -Destination "$root\x64\Release\" -Recurse -Force

# Copy DLLs from lib/
Copy-Item -Path "$root\lib\*.dll" -Destination "$root\x64\Release\" -Force

# Copy vcpkg DLLs
$vcpkgBin = "$root\vcpkg_installed\x64-windows\bin"
if (Test-Path $vcpkgBin) {
    Get-ChildItem "$vcpkgBin\*.dll" -ErrorAction SilentlyContinue | Copy-Item -Destination "$root\x64\Release\" -Force
}

Write-Host 'Deployment complete!'
