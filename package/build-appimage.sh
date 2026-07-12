#!/bin/sh
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
APPDIR=$(mktemp -d /tmp/calculator-appdir-XXXXXXXX)
TOOLS_DIR="$SCRIPT_DIR/.appimage-tools"

LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_PLUGIN_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
APPIMAGETOOL_URL="https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"

cleanup() {
    rm -rf "$APPDIR"
}
trap cleanup EXIT

echo "==> Setting up tools..."
mkdir -p "$TOOLS_DIR"
for url in "$LINUXDEPLOY_URL" "$LINUXDEPLOY_PLUGIN_QT_URL" "$APPIMAGETOOL_URL"; do
    name=$(basename "$url")
    if [ ! -f "$TOOLS_DIR/$name" ]; then
        curl -fsSL -o "$TOOLS_DIR/$name" "$url"
        chmod +x "$TOOLS_DIR/$name"
    fi
done

extract_appimage() {
    local appimage="$1"
    local outdir="$2"
    if [ -d "$outdir" ]; then
        return 0
    fi
    "$appimage" --appimage-extract >/dev/null 2>&1
    mv squashfs-root "$outdir"
}

extract_appimage "$TOOLS_DIR/linuxdeploy-x86_64.AppImage" "$TOOLS_DIR/linuxdeploy"
extract_appimage "$TOOLS_DIR/linuxdeploy-plugin-qt-x86_64.AppImage" "$TOOLS_DIR/linuxdeploy-plugin-qt"
extract_appimage "$TOOLS_DIR/appimagetool-x86_64.AppImage" "$TOOLS_DIR/appimagetool"

echo "==> Creating AppDir..."
mkdir -p "$APPDIR/usr/bin" "$APPDIR/usr/share/applications" \
    "$APPDIR/usr/share/icons/hicolor/256x256/apps" \
    "$APPDIR/usr/share/icons/hicolor/scalable/apps"

cp "$PROJECT_DIR/build-release/Calculator" "$APPDIR/usr/bin/"
cp "$SCRIPT_DIR/calculator-1.0-linux-x86_64/share/applications/calculator.desktop" \
    "$APPDIR/usr/share/applications/"
cp "$SCRIPT_DIR/calculator-1.0-linux-x86_64/share/icons/hicolor/256x256/apps/calculator.png" \
    "$APPDIR/usr/share/icons/hicolor/256x256/apps/"
cp "$SCRIPT_DIR/calculator-1.0-linux-x86_64/share/icons/hicolor/scalable/apps/calculator.svg" \
    "$APPDIR/usr/share/icons/hicolor/scalable/apps/"

echo "==> Deploying dependencies with linuxdeploy..."
export NO_STRIP=1
"$TOOLS_DIR/linuxdeploy/AppRun" --appdir "$APPDIR"

echo "==> Deploying Qt plugins..."
mkdir -p "$APPDIR/usr/plugins/platforms" \
    "$APPDIR/usr/plugins/imageformats" \
    "$APPDIR/usr/plugins/styles" \
    "$APPDIR/usr/plugins/iconengines" \
    "$APPDIR/usr/plugins/platforminputcontexts" \
    "$APPDIR/usr/plugins/xcbglintegrations" \
    "$APPDIR/usr/plugins/wayland-shell-integration" \
    "$APPDIR/usr/plugins/wayland-graphics-integration-client" \
    "$APPDIR/usr/plugins/wayland-decoration-client" \
    "$APPDIR/usr/plugins/generic"

cp /usr/lib/qt6/plugins/platforms/libqwayland.so "$APPDIR/usr/plugins/platforms/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/platforms/libqxcb.so "$APPDIR/usr/plugins/platforms/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/platforms/libqoffscreen.so "$APPDIR/usr/plugins/platforms/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/platforms/libqminimal.so "$APPDIR/usr/plugins/platforms/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/platforminputcontexts/* "$APPDIR/usr/plugins/platforminputcontexts/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/styles/* "$APPDIR/usr/plugins/styles/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/iconengines/* "$APPDIR/usr/plugins/iconengines/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/generic/* "$APPDIR/usr/plugins/generic/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/xcbglintegrations/* "$APPDIR/usr/plugins/xcbglintegrations/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/wayland-shell-integration/* "$APPDIR/usr/plugins/wayland-shell-integration/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/wayland-graphics-integration-client/* "$APPDIR/usr/plugins/wayland-graphics-integration-client/" 2>/dev/null || true
cp /usr/lib/qt6/plugins/wayland-decoration-client/* "$APPDIR/usr/plugins/wayland-decoration-client/" 2>/dev/null || true

# Redeploy dependencies for newly added plugins
"$TOOLS_DIR/linuxdeploy/AppRun" --appdir "$APPDIR"

echo "==> Creating AppImage..."
export VERSION=1.0
"$TOOLS_DIR/appimagetool/AppRun" "$APPDIR" "$SCRIPT_DIR/Calculator-x86_64.AppImage"

echo "==> Done! AppImage created: $SCRIPT_DIR/Calculator-x86_64.AppImage"
