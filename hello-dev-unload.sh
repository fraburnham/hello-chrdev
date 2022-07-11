#!/bin/bash

set -xeuo pipefail

[[ "$UID" == "0" ]] || (echo "Must run as root" && exit 1)

MODULE=${MODULE:-"hello-dev"}
/sbin/rmmod $MODULE $@ || exit 1

DEVICE=${DEVICE:-"hello-dev"}
rm -f /dev/${DEVICE}?
