#!/bin/bash

# Compile OpenArena Game Script (mods/openarena)
set -e

# Usage: ./compile_oa.sh [Debug|Release] [clean]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OA_ROOT="$SCRIPT_DIR"
PROJECT_ROOT="$(cd "$OA_ROOT/../.." && pwd)"

MOD_NAME="openarena"
BUILD_TYPE="Release"
CLEAN=0
PK3_ENABLED=1

normalize_build_type() {
    local arg=$(echo "$1" | tr '[:upper:]' '[:lower:]')
    case "$arg" in
        debug|dbg|d) echo "Debug" ;;
        release|rel|r) echo "Release" ;;
        *) echo "" ;;
    esac
}

for arg in "$@"; do
    norm_bt="$(normalize_build_type "$arg")"
    if [ -n "$norm_bt" ]; then
        BUILD_TYPE="$norm_bt"
        continue
    fi
    if [ "$arg" = "clean" ]; then
        CLEAN=1
        continue
    fi
done

MOD_SOURCE_DIR="$OA_ROOT/gamesrc"
MOD_BUILD_DIR="$MOD_SOURCE_DIR/build"
MOD_VM_DIR="$OA_ROOT/vm"
RELEASE_MOD_DIR="$PROJECT_ROOT/release/$MOD_NAME"
RELEASE_VM_DIR="$RELEASE_MOD_DIR/vm"
RELEASE_PK3="$RELEASE_MOD_DIR/$MOD_NAME.pk3"
RUNTIME_VM_DIR="$HOME/.$MOD_NAME/vm"

if [ ! -d "$MOD_SOURCE_DIR" ]; then
    echo "Error: ${MOD_SOURCE_DIR} not found."
    echo "Usage: $0 [Debug|Release] [clean]"
    exit 1
fi

echo "Building OpenArena modules..."
echo "Project root: $PROJECT_ROOT"
echo "OA root: $OA_ROOT"
echo "Build type: $BUILD_TYPE"
echo "Module sources: $MOD_SOURCE_DIR"
echo "Release destination: $RELEASE_MOD_DIR"

cd "$MOD_SOURCE_DIR"

CMAKE_CACHE="$MOD_BUILD_DIR/CMakeCache.txt"
if [ -f "$CMAKE_CACHE" ]; then
    CACHE_SRC_DIR=$(grep '^CMAKE_HOME_DIRECTORY:INTERNAL=' "$CMAKE_CACHE" | head -n1 | cut -d= -f2)
    if [ -n "$CACHE_SRC_DIR" ] && [ "$CACHE_SRC_DIR" != "$MOD_SOURCE_DIR" ]; then
        echo ""
        echo "Warning: CMake cache source mismatch detected."
        echo "  CMakeCache.txt: $CMAKE_CACHE"
        echo "  Current source: $MOD_SOURCE_DIR"
        echo "  Cached source:  $CACHE_SRC_DIR"
        echo "Removing bad CMake cache. (This happens when moving or renaming directories.)"
        rm -rf "$MOD_BUILD_DIR"
    fi
fi

if [ $CLEAN -eq 1 ] && [ -d "$MOD_BUILD_DIR" ]; then
    echo "Cleaning old build directory..."
    rm -rf "$MOD_BUILD_DIR"
fi

if [ $CLEAN -eq 1 ] && [ -d "$RELEASE_VM_DIR" ]; then
    echo "Removing old VM files from $RELEASE_VM_DIR ..."
    rm -f "$RELEASE_VM_DIR"/*.so "$RELEASE_VM_DIR"/*.dll 2>/dev/null || true
fi

mkdir -p "$MOD_BUILD_DIR"
cd "$MOD_BUILD_DIR"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Build with parallel jobs
if command -v nproc &>/dev/null; then
    CORES=$(nproc)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi
cmake --build . -- -j${CORES}

echo "Checking for compiled VM files in $MOD_VM_DIR"
mkdir -p "$MOD_VM_DIR"

# --- BEGIN: Copy QVMs to /vm after build ---
QVM_PATTERNS=("game.qvm" "cgame.qvm" "ui.qvm")

for pattern in "${QVM_PATTERNS[@]}"; do
    # Look for QVMs in standard game subdirs
    for sub in game cgame ui; do
        QVM_SRC="$MOD_BUILD_DIR/code/$sub/$pattern"
        if [ -f "$QVM_SRC" ]; then
            cp -v "$QVM_SRC" "$MOD_VM_DIR/"
        fi
    done
done
# --- END: Copy QVMs to /vm after build ---

map_vm_name() {
    local base="$1"
    case "$base" in
        gamex86_64.so)  echo "game.x86_64.so" ;;
        cgamex86_64.so) echo "cgame.x86_64.so" ;;
        uix86_64.so)    echo "ui.x86_64.so" ;;
        *)              echo "$base" ;;
    esac
}

shopt -s nullglob
ARTIFACTS=("$MOD_VM_DIR"/*.so "$MOD_VM_DIR"/*.dll "$OA_ROOT"/*x86_64.so "$OA_ROOT"/*x86_64.dll)
shopt -u nullglob

if [ ${#ARTIFACTS[@]} -eq 0 ]; then
    echo "Warning: No shared libraries found in $MOD_VM_DIR/"
else
    mkdir -p "$RELEASE_VM_DIR"
    mkdir -p "$RUNTIME_VM_DIR"
    rm -f "$RELEASE_VM_DIR"/*.qvm "$RUNTIME_VM_DIR"/*.qvm \
          "$RELEASE_VM_DIR"/gamex86_64.so "$RELEASE_VM_DIR"/cgamex86_64.so "$RELEASE_VM_DIR"/uix86_64.so \
          "$RUNTIME_VM_DIR"/gamex86_64.so "$RUNTIME_VM_DIR"/cgamex86_64.so "$RUNTIME_VM_DIR"/uix86_64.so 2>/dev/null || true
    echo "Copying files to OA release directory: $RELEASE_VM_DIR (and runtime: $RUNTIME_VM_DIR)"
    for lib in "${ARTIFACTS[@]}"; do
        libname=$(basename "$lib")
        mapped=$(map_vm_name "$libname")
        cp -v "$lib" "$RELEASE_VM_DIR/$mapped"
        cp -v "$lib" "$RUNTIME_VM_DIR/$mapped"
    done
    echo "Libraries copied to $RELEASE_VM_DIR/"
fi

# Copy QVMs to $RELEASE_VM_DIR and $RUNTIME_VM_DIR too
shopt -s nullglob
QVM_FILES=("$MOD_VM_DIR"/*.qvm)
shopt -u nullglob
if [ ${#QVM_FILES[@]} -gt 0 ]; then
    mkdir -p "$RELEASE_VM_DIR"
    mkdir -p "$RUNTIME_VM_DIR"
    rm -f "$RELEASE_VM_DIR"/*.qvm "$RUNTIME_VM_DIR"/*.qvm 2>/dev/null || true
    echo "Copying QVMs to OA release directory: $RELEASE_VM_DIR (and runtime: $RUNTIME_VM_DIR)"
    for qvm in "${QVM_FILES[@]}"; do
        cp -v "$qvm" "$RELEASE_VM_DIR/"
        cp -v "$qvm" "$RUNTIME_VM_DIR/"
    done
    echo "QVMs copied to $RELEASE_VM_DIR/"
fi

package_pk3() {
    INCLUDES=()
    add_if_exists() {
        local rel="$1"
        if [ -e "$OA_ROOT/$rel" ]; then
            INCLUDES+=("$rel")
        fi
    }

    add_if_exists "default.cfg"
    add_if_exists "autoexec.cfg"
    add_if_exists "config"
    add_if_exists "fonts"
    add_if_exists "scripts"
    add_if_exists "shaders"
    add_if_exists "textures"
    add_if_exists "models"
    add_if_exists "players"
    add_if_exists "ui"
    add_if_exists "vm"
    add_if_exists "maps"
    add_if_exists "levelshots"
    add_if_exists "gfx"
    add_if_exists "sound"

    if [ ${#INCLUDES[@]} -eq 0 ]; then
        echo "Nothing to package into pk3 for $MOD_NAME."
        return
    fi

    mkdir -p "$RELEASE_MOD_DIR"
    cd "$OA_ROOT"

    echo "Packaging ${MOD_NAME}.pk3 ..."
    zip -r "$RELEASE_PK3" "${INCLUDES[@]}" \
        -x "gamesrc/*" "gamesrc/**" \
           "build/*" "build/**" \
           "out/*" "out/**" \
           "vm/*.a" "vm/*.pdb" "vm/*.dll" \
           "**/.DS_Store" "**/.git*" "**/CMakeFiles/**" \
        >/dev/null
    echo "✓ Wrote $RELEASE_PK3"
}

if [ $PK3_ENABLED -eq 1 ]; then
    package_pk3
fi

echo ""
if [ -d "$RELEASE_VM_DIR" ]; then
    SO_COUNT=$(ls -1 "$RELEASE_VM_DIR"/*.so "$RELEASE_VM_DIR"/*.dll 2>/dev/null | wc -l)
    if [ "$SO_COUNT" -gt 0 ]; then
        echo "Libraries available in $RELEASE_VM_DIR/:"
        ls -lh "$RELEASE_VM_DIR"/*.so "$RELEASE_VM_DIR"/*.dll 2>/dev/null || true
        echo "✓ Game modules ready in $RELEASE_VM_DIR"
    else
        echo "Warning: No shared libraries found in $RELEASE_VM_DIR/"
    fi
fi

echo ""
echo "OpenArena build completed!"
echo "  Libraries: $RELEASE_VM_DIR/*.so"
echo "  Package:  $RELEASE_PK3"
echo "  Source: $MOD_SOURCE_DIR/"

