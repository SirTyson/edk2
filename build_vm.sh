#!/bin/sh
# Builds OVMF image and copies into VM folder with correct name

build && cp Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd VM/bios.bin
