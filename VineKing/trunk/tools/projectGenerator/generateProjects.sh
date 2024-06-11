#!/bin/sh

pushd ../../build/
source common.sh
popd

if [ ! -f "$1" ]; then
   echo "Error: project config $1 does not exist!"
   exit 1
fi

CONFIG=`getPlatformPath "$1"`
$PHP ./projectGenerator.php "$CONFIG"
