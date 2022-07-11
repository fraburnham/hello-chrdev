#!/bin/bash

set -euo pipefail

[[ "$UID" == "0" ]] || (echo "Must run as root" && exit 1)

MODULE=${MODULE:-"hello-dev"}
DEVICE=${DEVICE:-"hello-dev"}
/sbin/insmod ./$MODULE.ko $@
major=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
mknod "/dev/${DEVICE}0" "c" "$major" "0"

MODE=${MODE:-"666"} # rw-rw-rw-
chmod $MODE /dev/${DEVICE}0
