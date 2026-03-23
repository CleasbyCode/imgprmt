#!/bin/bash

# compile_imgprmt.sh — Release build script for imgprmt

set -euo pipefail

CXX="${CXX:-g++}"
TARGET="${TARGET:-imgprmt}"
SRCDIR="."

COMMON_WARNINGS="-std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat -Wformat-security"
RELEASE_CXXFLAGS="-O3 -march=native -pipe -DNDEBUG -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -s -flto=auto -fuse-linker-plugin -fstack-protector-strong -fstack-clash-protection -ftrivial-auto-var-init=zero -fPIE"
RELEASE_LDFLAGS="-pie -Wl,-z,relro,-z,now,-z,noexecstack"

CXXFLAGS="$COMMON_WARNINGS $RELEASE_CXXFLAGS"
LDFLAGS="$RELEASE_LDFLAGS -lturbojpeg"

SOURCES=(
    "$SRCDIR/imgprmt.cpp"
    "$SRCDIR/imgprmt_args.cpp"
    "$SRCDIR/imgprmt_cli.cpp"
    "$SRCDIR/imgprmt_io.cpp"
    "$SRCDIR/imgprmt_jpeg.cpp"
    "$SRCDIR/imgprmt_pipeline.cpp"
    "$SRCDIR/imgprmt_template.cpp"
    "$SRCDIR/imgprmt_text.cpp"
    "$SRCDIR/imgprmt_url.cpp"
    "$SRCDIR/imgprmt_main.cpp"
)

echo "Compiling $TARGET (release)..."
$CXX $CXXFLAGS "${SOURCES[@]}" $LDFLAGS -o "$TARGET"
echo "Compilation successful. Executable '$TARGET' created."
