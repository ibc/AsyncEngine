#!/bin/bash

set -e

FILE=$1

gcc $FILE ../uv.a -I../include/ -L../ -lm -lpthread -lrt -o /tmp/ibc_test_uv
/tmp/ibc_test_uv
