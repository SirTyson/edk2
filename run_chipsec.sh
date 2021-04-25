#!/bin/sh
rm -rf ../support/chipsec_drive
cp -r ../support/chipsec_bootable ../support/chipsec_drive
stuart_build -c OvmfPkg/PlatformCI/PlatformBuild.py TOOL_CHAIN_TAG=GCC5 -a IA32,X64 'BLD_*_SMM_REQUIRE=1' 'BLD_*_TPM_ENABLE=1' 'BLD_*_TPM_CONFIG_ENABLE=1' --FlashRom