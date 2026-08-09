#!/bin/sh
set -e

PREFIX="${1:-$HOME/.local}"
BINDIR="$PREFIX/bin"
APPDIR="$PREFIX/share/applications"
ICONDIR="$PREFIX/share/icons/hicolor"
METAINFO_DIR="$PREFIX/share/metainfo"

echo "Installing Qt6 Calculator to $PREFIX ..."

mkdir -p "$BINDIR" "$APPDIR" "$ICONDIR/scalable/apps" "$ICONDIR/256x256/apps" "$METAINFO_DIR"

cp bin/Calculator "$BINDIR/"
cp share/applications/calculator.desktop "$APPDIR/"
cp share/icons/hicolor/scalable/apps/calculator.svg "$ICONDIR/scalable/apps/"
cp share/metainfo/calculator.metainfo.xml "$METAINFO_DIR/"

chmod +x "$BINDIR/Calculator"

echo "Done. Calculator installed to $PREFIX"
echo "You may need to run: update-desktop-database (or equivalent) to see it in your app menu."
