#!/bin/bash

PROJECT_NAME="AutoTile"
SOURCE_DIR="src"
BUILD_DIR="build"

compile() {
    
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR

    cmake .. || exit 1

    if [ -z "$1" ]; then
        make || exit 1
    else
        make -j$1 || exit 1
    fi

    cd ..
    echo
}

vscode_build() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake -DENABLE_VSCODE_TASK=ON .. || exit 1
    cmake --build . || exit 1
    cd ..
}

run() {
    cd ./$BUILD_DIR
    ./$PROJECT_NAME
    cd -
}

clean() {
    rm -rf $BUILD_DIR
}

# Parse command line arguments
case "$1" in
    "compile")
        compile $2
        ;;
    "run")
        compile $2
        run
        ;;
    "runonly")
        run
	;;
    "clean")
        clean
        ;;
    "vscode")
        vscode_build
        ;;
    *)
        echo "Usage: $0 [compile | run | runonly | clean | vscode] [num_cores]"
        exit 1
        ;;
esac
