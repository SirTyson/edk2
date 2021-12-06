
/** @file
  GUID for UEFI variables related to SecureKernel.
**/

#ifndef __SECURE_KERNEL_SHARED_MEMORY_H__
#define __SECURE_KERNEL_SHARED_MEMORY_H__

// {67021946-346c-4805-ac0b-d112a2f14481}
#define SECURE_KERNEL_SHARED_MEMORY_GUID \
{0x67021946, 0x346c, 0x4805, {0xac, 0x0b, 0xd1, 0x12, 0xa2, 0xf1, 0x44, 0x81}};


extern EFI_GUID gSecureKernelSharedMemoryGuid;

#endif
