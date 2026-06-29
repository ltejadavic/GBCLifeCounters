#!/bin/sh
set -eu

GBDK_VERSION="4.5.0"
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
INSTALL_DIR="$ROOT_DIR/.tools/gbdk"

if [ -x "$INSTALL_DIR/bin/lcc" ]; then
    echo "GBDK already available at $INSTALL_DIR"
    exit 0
fi

SYSTEM=$(uname -s)
MACHINE=$(uname -m)

case "$SYSTEM:$MACHINE" in
    Darwin:arm64)
        ARCHIVE_NAME="gbdk-macos-arm64.tar.gz"
        EXPECTED_SHA256="289ee60e46c5a2785a21e35533f84a5131ed4a063b21b0dbdedc9a10af15bf78"
        ;;
    Darwin:x86_64)
        ARCHIVE_NAME="gbdk-macos.tar.gz"
        EXPECTED_SHA256="1aa549d12032d8f6509d11923bb28b1a453098f42597feb378e9a42541f8fd89"
        ;;
    Linux:aarch64|Linux:arm64)
        ARCHIVE_NAME="gbdk-linux-arm64.tar.gz"
        EXPECTED_SHA256="31eb2235f0fdb60163d0b1e9574a022098d6069cd56606a1daca4478a46e0439"
        ;;
    Linux:x86_64)
        ARCHIVE_NAME="gbdk-linux64.tar.gz"
        EXPECTED_SHA256="d7857a5f6d135ee4c249043ca26aad9f2ec8ab5d4106d97720d404114f42605c"
        ;;
    *)
        echo "Unsupported host: $SYSTEM $MACHINE" >&2
        echo "Install GBDK manually and set GBDK_HOME when running make." >&2
        exit 1
        ;;
esac

DOWNLOAD_URL="https://github.com/gbdk-2020/gbdk-2020/releases/download/$GBDK_VERSION/$ARCHIVE_NAME"
TEMP_DIR=$(mktemp -d)
trap 'rm -rf "$TEMP_DIR"' EXIT HUP INT TERM
ARCHIVE_PATH="$TEMP_DIR/$ARCHIVE_NAME"

echo "Downloading GBDK $GBDK_VERSION for $SYSTEM $MACHINE..."
curl -L --fail --show-error --output "$ARCHIVE_PATH" "$DOWNLOAD_URL"

ACTUAL_SHA256=$(shasum -a 256 "$ARCHIVE_PATH" | awk '{print $1}')
if [ "$ACTUAL_SHA256" != "$EXPECTED_SHA256" ]; then
    echo "GBDK archive checksum mismatch." >&2
    exit 1
fi

mkdir -p "$ROOT_DIR/.tools"
tar -xzf "$ARCHIVE_PATH" -C "$ROOT_DIR/.tools"
test -x "$INSTALL_DIR/bin/lcc"
echo "GBDK $GBDK_VERSION installed at $INSTALL_DIR"
