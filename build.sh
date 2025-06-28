#!/usr/bin/bash

# This is the Linux/BSD building entry point
# build dependencies: wget, tar, 7z, gcc

# Current only supports linux x86_64
if [ ! -f lib/libraylib.a ]; then
    wget "https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz"
    tar -zxf "raylib-5.5_linux_amd64.tar.gz"
    mv "raylib-5.5_linux_amd64/lib" lib
    mv "raylib-5.5_linux_amd64/include" inc
    rm -rf "raylib-5.5_linux_amd64" "raylib-5.5_linux_amd64.tar.gz"
fi

if [ ! -f res/ttf/FiraCode-Medium.ttf ]; then
    cd res
        mkdir aux
        cd aux
            wget "https://github.com/tonsky/FiraCode/releases/download/6.2/Fira_Code_v6.2.zip"
            7z x "Fira_Code_v6.2.zip"
            mv ttf ..
        cd ..
        rm -rf aux
    cd ..
fi

# Some window managers don't use the XDG protocol, handle this latter
if [ "$XDG_SESSION_TYPE" == "wayland" ]; then
    gcc main.c lib/libraylib.a -lm -lGL -lpthread -ldl -lrt -o squara
fi

if [ "$XDG_SESSION_TYPE" == "x11" ]; then
    gcc main.c lib/libraylib.a -lm -lGL -lpthread -ldl -lrt -lX11 -o squara
fi
