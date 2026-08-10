#!/bin/sh
set -e

# Builds Calculator.app and Calculator-<version>-macos-<arch>.dmg on macOS.
# Usage: build-macos.sh [BUILD_DIR] [ARCH]
#   BUILD_DIR  build directory (default: $PROJECT_DIR/build-macos)
#   ARCH       target architecture, x86_64 or arm64 (default: host arch)
# Requires: CMake, a Qt6 install with macdeployqt (qmake6/qmake on PATH),
# rsvg-convert (brew install librsvg), and the macOS developer tools
# (sips, iconutil, hdiutil).

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
VERSION="1.01"
BUILD_DIR="${1:-$PROJECT_DIR/build-macos}"
ARCH="${2:-$(uname -m)}"
APPDIR="$BUILD_DIR/Calculator.app"
DMG="$SCRIPT_DIR/Calculator-$VERSION-macos-$ARCH.dmg"
ICONSET="$BUILD_DIR/Calculator.iconset"
ICNS="$BUILD_DIR/calculator.icns"
SVG="$PROJECT_DIR/resources/icons/calculator.svg"
RSVG="${RSVG_CONVERT_EXECUTABLE:-rsvg-convert}"

echo "==> Configuring Release build ($ARCH)..."
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DCMAKE_OSX_ARCHITECTURES="$ARCH"

echo "==> Building Calculator.app..."
cmake --build "$BUILD_DIR" -j"$(sysctl -n hw.ncpu)"

echo "==> Generating icon (.icns)..."
rm -rf "$ICONSET"
mkdir -p "$ICONSET"
"$RSVG" -w 1024 -h 1024 -o "$ICONSET/source.png" "$SVG"
for s in 16 32 128 256 512; do
    sips -z "$s" "$s" "$ICONSET/source.png" --out "$ICONSET/icon_${s}x${s}.png" >/dev/null
    d=$((s * 2))
    sips -z "$d" "$d" "$ICONSET/source.png" --out "$ICONSET/icon_${s}x${s}@2x.png" >/dev/null
done
rm -f "$ICONSET/source.png"
iconutil -c icns "$ICONSET" -o "$ICNS"
mkdir -p "$APPDIR/Contents/Resources"
cp "$ICNS" "$APPDIR/Contents/Resources/"

echo "==> Deploying Qt dependencies (macdeployqt)..."
QT_BIN_DIR="${QT_BIN_DIR:-$(dirname "$(command -v qmake6 || command -v qmake)")}"
"$QT_BIN_DIR/macdeployqt" "$APPDIR" -always-overwrite

echo "==> Re-signing with ad-hoc signature..."
codesign --force --deep --sign - "$APPDIR"

echo "==> Creating DMG..."
rm -f "$DMG"
hdiutil create -volname "Calculator" -srcfolder "$APPDIR" -ov -format UDZO "$DMG"

echo "==> Done! DMG created: $DMG"
