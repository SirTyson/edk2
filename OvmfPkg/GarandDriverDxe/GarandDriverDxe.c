/** @file

Basic dxe drive, prints to console, lets me know that everything built ok.

**/
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>

EFI_STATUS
EFIAPI
GarandDriverInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  DEBUG ((DEBUG_INFO, "GarandDriverDxe: YEET\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GarandDriverUnload (
  IN EFI_HANDLE   ImageHandle
  )
{
  return EFI_SUCCESS;
}