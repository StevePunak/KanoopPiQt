#!/bin/bash
# usage deploy.sh [target host]

TARGET_HOST=$1
REMOTE_DIR=/usr/local/lib
rsync --rsync-path="sudo rsync" -avh $HOME/lib/arm/*so* $TARGET_HOST:$REMOTE_DIR
