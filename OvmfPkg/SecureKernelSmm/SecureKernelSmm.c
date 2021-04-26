/** @file

DXE/SMM Driver that secure kernel will run in. 

**/

// Flag for running driver in QEMU vs Hardware
#define QEMU_BUILD

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

#ifdef QEMU_BUILD
#include <IndustryStandard/Q35MchIch9.h>     // ICH9_APM_CNT
#include <Library/MmServicesTableLib.h>      // gMmst
#include <Protocol/MmCpuIo.h>                // EFI_MM_CPU_IO_PROTOCOL
#endif

//
// SMI value to vector to kernel init function
//
#define KERNEL_INIT_SMI_VAL 0xCC

//
// Main SMI/MMI Handler registration
//
STATIC EFI_HANDLE mDispatchHandle;

#ifdef QEMU_BUILD

//
// We use this protocol for accessing IO Ports.
//
STATIC EFI_MM_CPU_IO_PROTOCOL *mMmCpuIo;

#else
//
// Register context for SMI handler
//
EFI_SMM_SW_REGISTER_CONTEXT m_SwDispatch2InitCtx = { KERNEL_INIT_SMI_VAL };
// More interupt vectors can be defined here ...

#endif

// Handler for Kernel Init SMI
STATIC
EFI_STATUS
EFIAPI
KernelInitHandler (
  IN EFI_HANDLE               DispatchHandle,
  IN CONST VOID               *Context          OPTIONAL,
  IN OUT VOID                 *CommBuffer       OPTIONAL,
  IN OUT UINTN                *CommBufferSize   OPTIONAL
  )
{
  STATIC UINTN INTERRUPT_COUNTER = 0;

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS in kernel init handler, counter: %d\n", ++INTERRUPT_COUNTER));
  return EFI_SUCCESS;
}


#ifdef QEMU_BUILD
// Handler for MMI Events. Responsible for passing events onto child handlers.
STATIC
EFI_STATUS
EFIAPI
MainMmiHandler (
  IN EFI_HANDLE               DispatchHandle,
  IN CONST VOID               *Context          OPTIONAL,
  IN OUT VOID                 *CommBuffer       OPTIONAL,
  IN OUT UINTN                *CommBufferSize   OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT8      ApmControl;

  //
  // Assert that we are entering this function due to our root MMI handler
  // registration.
  //
  ASSERT (DispatchHandle == mDispatchHandle);
  //
  // When MmiManage() is invoked to process root MMI handlers, the caller (the
  // MM Core) is expected to pass in a NULL Context. MmiManage() then passes
  // the same NULL Context to individual handlers.
  //
  ASSERT (Context == NULL);
  //
  // Read the MMI command value from the APM Control Port, to see if this is an
  // MMI we should care about.
  //
  Status = mMmCpuIo->Io.Read (mMmCpuIo, MM_IO_UINT8, ICH9_APM_CNT, 1,
                          &ApmControl);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: failed to read ICH9_APM_CNT: %r\n", __FUNCTION__,
      Status));
    //
    // We couldn't even determine if the MMI was for us or not.
    //
    return EFI_SUCCESS; // TODO: Better error
  }

  switch (ApmControl) {
  case KERNEL_INIT_SMI_VAL:
    return KernelInitHandler(DispatchHandle, Context, CommBuffer, CommBufferSize);
  default:
    return EFI_WARN_INTERRUPT_SOURCE_QUIESCED;
  }
}
#endif


EFI_STATUS
EFIAPI
SecureKernelInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
  BOOLEAN                                   InSmram;
  EFI_BOOT_SERVICES                         *BootServices;

#ifndef QEMU_BUILD
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
#endif

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS init entry point.\n"));

  BootServices = SystemTable->BootServices;
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

#ifdef QEMU_BUILD

  Status = gMmst->MmLocateProtocol (&gEfiMmCpuIoProtocolGuid,
                    NULL /* Registration */, (VOID **)&mMmCpuIo);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: locate MmCpuIo: %r\n", __FUNCTION__, Status));
    return EFI_SUCCESS;
  }

  //
  // Register the handler for the MMI Handler.
  //
  Status = gMmst->MmiHandlerRegister (
                    MainMmiHandler,
                    NULL,            // HandlerType: root MMI handler
                    &mDispatchHandle
                    );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SecureKernelSmm: SombraOS failed to register software MMI handler.\n"));
  } else {
    DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS successfully registered software MMI handler.\n"));
  }

#else

  Register SMM protocol notification
  Status = gSmst->SmmRegisterProtocolNotify (
      &gEfiSmmSwDispatch2ProtocolGuid, 
      SwDispatch2ProtocolNotifyHandler, 
      NULL
  );

  Status = gSmst->SmmLocateProtocol (
    &gEfiSmmSwDispatch2ProtocolGuid,
    NULL,
    (VOID **) &SwDispatch
  );
  
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SecureKernelSmm: SombraOS failed to locate SMM SwDispatch interface. Status = %r\n", Status));
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS successfully located SMM SwDispatch interface.\n"));
  }

  Status = SwDispatch->Register(
    SwDispatch, 
    SwDispatch2Handler, 
    &m_SwDispatch2InitCtx, // &SwContext
    &mDispatchHandle
  );

#endif

  DEBUG ((DEBUG_INFO, "SecureKernelSmm: SombraOS finished init.\n"));
  return EFI_SUCCESS;
}