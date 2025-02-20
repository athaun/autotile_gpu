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
    *)
        echo "Usage: $0 [compile | run | runonly | clean] [num_cores]"
        exit 1
        ;;
esac
