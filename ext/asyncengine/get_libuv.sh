#!/bin/bash

set -e

LIBUV_GIT_REPO="https://github.com/joyent/libuv.git"
LIBUV_GIT_BRANCH="master"
LIBUV_GIT_REVISION="9984d15"
LIBUV_DIR="libuv"

rm -rf $LIBUV_DIR
git clone -b $LIBUV_GIT_BRANCH $LIBUV_GIT_REPO $LIBUV_DIR
cd $LIBUV_DIR
git checkout $LIBUV_GIT_REVISION -q
