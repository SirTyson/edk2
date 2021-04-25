/** @file

DXE/SMM Driver that secure kernel will run in. 

**/

// Base DXE driver includes
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>

// For debug print
#include <Library/DebugLib.h>

// SMM UEFI Drivers
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/SmmServicesTableLib.h>

// UEFI SMM foundation headers
//#include <FrameworkSmm.h>

// SMI value to vector to kernel init function
#define KERNEL_INIT_SMI_VAL 0xCC
// Register context for SMI handler
EFI_SMM_SW_REGISTER_CONTEXT m_SwDispatch2InitCtx = { KERNEL_INIT_SMI_VAL };
// More interupt vectors can be defined here ...


// Handler for Kernel Init SMI
EFI_STATUS
EFIAPI
SwDispatch2Handler (
  IN EFI_HANDLE               DispatchHandle,
  IN CONST VOID               *Context,
  IN OUT VOID                 *CommBuffer,      // IdK if these two are actually in/out or just in
  IN OUT UINTN                *CommBufferSize
  )
{
  static UINTN INTERRUPT_COUNTER = 0;

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS in kernel init handler, counter: %d", ++INTERRUPT_COUNTER));
  return EFI_SUCCESS;
}

// Register Software SMI Handler
EFI_STATUS
EFIAPI
SwDispatch2ProtocolNotifyHandler (
  IN CONST EFI_GUID           *Protocol,
  IN VOID                     *Interface,
  EFI_HANDLE                  Handle
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      DispatchHandle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch;


  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS about to register handler."));

  Status = EFI_SUCCESS;
  DispatchHandle = NULL;
  SwDispatch = (EFI_SMM_SW_DISPATCH2_PROTOCOL *) Interface;

  Status = SwDispatch->Register(
    SwDispatch, 
    SwDispatch2Handler, 
    &m_SwDispatch2InitCtx,
    &DispatchHandle
  );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SecureKernelSmm: SombraOS failed to register software SMI handler."));
  } else {
    DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS successfully registered software SMI handler."));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SecureKernelInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;
  VOID                    *Registration;
  EFI_SMM_BASE2_PROTOCOL  *SmmBase;
  BOOLEAN                 InSmram;
  EFI_BOOT_SERVICES       *BootServices;
  //EFI_RUNTIME_SERVICES    *RuntimeServices
  //EFI_SMM_SYSTEM_TABLE2   *gSmst;

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS init entry point.\n"));

  BootServices = SystemTable->BootServices;
  //gRT = gST->RuntimeServices;

  Status = BootServices->LocateProtocol(&gEfiSmmBase2ProtocolGuid, NULL, (VOID **) &SmmBase);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SecureKernelSmm: SombraOS failed to locate SMM Base protocol.\n"));
    return Status;
  }

  SmmBase->InSmm(SmmBase, &InSmram);
  if (InSmram) {
    DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS init in SMRAM!\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "SecureKernelSmm: SombraOS init not in SMRAM, aborting.\n"));
    return Status;
  }

  // Register SMM protocol notification
  Status = gSmst->SmmRegisterProtocolNotify (
      &gEfiSmmSwDispatch2ProtocolGuid, 
      SwDispatch2ProtocolNotifyHandler, 
      &Registration
  );

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS finished init.\n"));

  return EFI_SUCCESS;
}