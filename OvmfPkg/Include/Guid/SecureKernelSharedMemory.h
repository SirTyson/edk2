
/** @file
  GUID for UEFI variables related to SecureKernel.
**/

#ifndef __SECURE_KERNEL_SHARED_MEMORY_H__
#define __SECURE_KERNEL_SHARED_MEMORY_H__

// {67021946-346c-4805-ac0b-d112a2f14481}
#define SECURE_KERNEL_SHARED_MEMORY_GUID \
{0x67021946, 0x346c, 0x4805, {0xac, 0x0b, 0xd1, 0x12, 0xa2, 0xf1, 0x44, 0x81}};

// {4f19a5a5-4f3b-411d-bdb9-6b4fd535cef8}
#define SECURE_KERNEL_KERNEL_READY_GUID \
{0x4f19a5a5, 0x4f3b, 0x411d, {0xbd, 0xb9, 0x6b, 0x4f, 0xd5, 0x35, 0xce, 0xf8}};

// {7e117759-0d50-4a66-90ec-5a2c036eed94}
#define SECURE_KERNEL_KERNEL_NVS_BASE_GUID \
{0x7e117759, 0x0d50, 0x4a66, {0x90, 0xec, 0x5a, 0x2c, 0x03, 0x6e, 0xed, 0x94}};


extern EFI_GUID gSecureKernelSharedMemoryGuid;
extern EFI_GUID gSecureKernelKernelReadyGuid;
extern EFI_GUID gSecureKernelKernelNVSBaseGuid;

#endif
