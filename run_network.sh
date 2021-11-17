#!/bin/bash
# Launches headless qemu instance and opens port 2222 for ssh
# serial output from UEFI written to file in VM/log
# regular output written to stdio

# Log files are numbered starting at 0, never overwritten
# Different folder for each date
log_file_number=0
date_prefix=$( date +%Y.%m.%d )
log_path="/home/gttyson/edk2/VM/log/$date_prefix"
mkdir -p $log_path
while [ -e "$log_path/$log_file_number" ]; do
  (( ++log_file_number ))
done

touch "$log_path/$log_file_number"
qemu-system-x86_64 \
  -debugcon file:"$log_path/$log_file_number" \
  -global isa-debugcon.iobase=0x402 \
  -drive file=/home/gttyson/edk2/VM/vm.disk,if=virtio,format=raw \
  -enable-kvm \
  -m 4G \
  -machine q35,smm=on \
  -global driver=cfi.pflash01,property=secure,value=on \
  -drive if=pflash,format=raw,unit=0,file=/home/gttyson/edk2/Build/Ovmf3264/DEBUG_GCC5/FV/OVMF_CODE.fd,readonly=on \
  -drive if=pflash,format=raw,unit=1,file=/home/gttyson/edk2/Build/Ovmf3264/DEBUG_GCC5/FV/OVMF_VARS.fd \
  -vga std \
  -net user,hostfwd=tcp::2222-:22 \
  -net nic \
  -nographic