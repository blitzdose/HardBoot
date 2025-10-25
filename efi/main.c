#include <efi.h>
#include <efilib.h>

#define ASCII_ART L""\
"=================================================================\n"\
" __  __                     __  ____                    __       \n"\
"/\\ \\/\\ \\                   /\\ \\/\\  _`\\                 /\\ \\__    \n"\
"\\ \\ \\_\\ \\     __     _ __  \\_\\ \\ \\ \\L\\ \\    ___     ___\\ \\ ,_\\   \n"\
" \\ \\  _  \\  /'__`\\  /\\`'__\\/'_` \\ \\  _ <'  / __`\\  / __`\\ \\ \\/   \n"\
"  \\ \\ \\ \\ \\/\\ \\L\\.\\_\\ \\ \\//\\ \\L\\ \\ \\ \\L\\ \\/\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_  \n"\
"   \\ \\_\\ \\_\\ \\__/.\\_\\\\ \\_\\\\ \\___,_\\ \\____/\\ \\____/\\ \\____/\\ \\__\\ \n"\
"    \\/_/\\/_/\\/__/\\/_/ \\/_/ \\/__,_ /\\/___/  \\/___/  \\/___/  \\/__/ \n"\
"                                                                 \n"\
"                                                Made by blitzdose\n"\
"                                                                 \n"\
"=================================================================\n"\
"                                                                 \n"
                                                                                     

VOID wait_for_key(EFI_SYSTEM_TABLE *SystemTable) {

    Print(L"Press SPACE to continue...\n");

    UINTN Index;
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;
    while (1) {
        Status = uefi_call_wrapper(SystemTable->BootServices->WaitForEvent, 3, 1, SystemTable->ConIn->WaitForKey, &Index);
        Status = uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key);

        if (Key.UnicodeChar == L' ') break;
    }
}

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;                  
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      
  EFI_FILE_IO_INTERFACE *IOVolume;                        
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; 
  EFI_FILE_HANDLE Volume;                                 

  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
  return Volume;
}

UINT64 FileSize(EFI_FILE_HANDLE FileHandle)
{
  UINT64 ret;
  EFI_FILE_INFO *FileInfo;
  FileInfo = LibFileInfo(FileHandle);
  ret = FileInfo->FileSize;
  FreePool(FileInfo);
  return ret;
}

int ReadBootChoice(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    CHAR16              *FileName = L"DATA.TXT";
    EFI_FILE_HANDLE     FileHandle;
    EFI_STATUS Status;

    EFI_FILE_HANDLE Volume = GetVolume(ImageHandle);
    Status = uefi_call_wrapper(Volume->Open, 5, Volume, &FileHandle, FileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    if (EFI_ERROR(Status)) {
        Print(L"Error: DATA.TXT not found. Something is very wrong...\n");
        wait_for_key(SystemTable);
        return Status;
    }
    
    UINT64 ReadSize = FileSize(FileHandle);
    UINT8  *Buffer = AllocatePool(ReadSize);

    Status = uefi_call_wrapper(FileHandle->Read, 3, FileHandle, &ReadSize, Buffer);
    if (EFI_ERROR(Status)) {
        Print(L"Error: Could not read DATA.TXT...\n");
        wait_for_key(SystemTable);
        return Status;
    }

    Status = uefi_call_wrapper(FileHandle->Close, 1, FileHandle);
    if (EFI_ERROR(Status)) {
        Print(L"Error: Could not close DATA.TXT... continuing anyway...\n");
    }

    return Buffer[0];
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{

    Print(ASCII_ART);

    InitializeLib(ImageHandle, SystemTable);
    
    EFI_STATUS Status;
    UINT16 *BootOrder = NULL;
    UINTN BootOrderSize = 0;
    EFI_GUID GlobalVar = EFI_GLOBAL_VARIABLE;
    int BOOT_ENTRY = 1;
    
    Status = uefi_call_wrapper(RT->GetVariable, 5, L"BootOrder", &GlobalVar, NULL, &BootOrderSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Error: BootOrder not found\n");
        wait_for_key(SystemTable);
        return Status;
    }

    Print(L"Hacking the Mainframe...\n");

    BOOT_ENTRY = ReadBootChoice(ImageHandle, SystemTable);
    
    BootOrder = AllocatePool(BootOrderSize);
    if (!BootOrder) {
        Print(L"Error: Could not allocate space\n");
        wait_for_key(SystemTable);
        return EFI_OUT_OF_RESOURCES;
    }

    Print(L"Stealing your data...\n");
    
    Status = uefi_call_wrapper(RT->GetVariable, 5, L"BootOrder", &GlobalVar, NULL, &BootOrderSize, BootOrder);
    if (EFI_ERROR(Status)) {
        Print(L"Error while reading BootOrder (%s)\n", Status);
        FreePool(BootOrder);
        wait_for_key(SystemTable);
        return Status;
    }
    
    UINTN BootOrderCount = BootOrderSize / sizeof(UINT16);

    Print(L"Installing rootkit...\n");
    
    if (BootOrderCount < 2)
    {
        Print(L"Error: Only found one boot entry! Can't boot anything else\n");
        wait_for_key(SystemTable);
        return EFI_ABORTED;
    }
    
    if (BootOrderCount < 3) {
        BOOT_ENTRY = 1;
        Print(L"Only found one other boot entry\n");
    }
    
    UINT16 BootEntry = BootOrder[BOOT_ENTRY];
    CHAR16 BootVarName[16];
    SPrint(BootVarName, sizeof(BootVarName), L"Boot%04x", BootEntry);
    
    Print(L"Loading boot entry variable (%s)\n", BootVarName);
    
    UINTN BootOptionSize = 0;
    UINT8 *BootOption = NULL;
    
    Status = uefi_call_wrapper(RT->GetVariable, 5, BootVarName, &GlobalVar, NULL, &BootOptionSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Error: Could not find boot entry\n");
        FreePool(BootOrder);
        wait_for_key(SystemTable);
        return Status;
    }
    
    BootOption = AllocatePool(BootOptionSize);
    if (!BootOption) {
        Print(L"Error: Could not allocate space\n");
        FreePool(BootOrder);
        wait_for_key(SystemTable);
        return EFI_OUT_OF_RESOURCES;
    }
    
    Status = uefi_call_wrapper(RT->GetVariable, 5, BootVarName, &GlobalVar, NULL, &BootOptionSize, BootOption);
    if (EFI_ERROR(Status)) {
        Print(L"Error while loading boot entry (%s)\n", Status);
        FreePool(BootOption);
        FreePool(BootOrder);
        wait_for_key(SystemTable);
        return Status;
    }
    
     Print(L"Loading boot entry: %s\n", (CHAR16*)BootOption + 3);

    EFI_DEVICE_PATH *BootX = (EFI_DEVICE_PATH*) (((UINT8*)BootOption) + (sizeof(UINT32) + sizeof(UINT16) + StrSize((UINT16 *)BootOption+3)));

    UINTN NoHandles = 0;
    EFI_HANDLE *handles = NULL;
    EFI_GUID SimpleFileSystemGUID = SIMPLE_FILE_SYSTEM_PROTOCOL;
    Status = uefi_call_wrapper(BS->LocateHandleBuffer,
          5,
          ByProtocol,
          &SimpleFileSystemGUID,
          NULL,
          &NoHandles,
          &handles
          );
    if (Status != EFI_SUCCESS) {
        Print(L"Failed to LocateHandleBuffer (%d)\n", Status);
        wait_for_key(SystemTable);
        return EFI_ABORTED;
    }

    EFI_DEVICE_PATH *prefix;
    UINTN index;
    for (index = 0; index < NoHandles; index++) {
       prefix = DevicePathFromHandle(handles[index]);
       while(!IsDevicePathEnd(NextDevicePathNode(prefix))) prefix = NextDevicePathNode(prefix);
       if(LibMatchDevicePaths(prefix, BootX)) {
          break;
       } else {
          FreePool(prefix);
       }
    }

    prefix = DevicePathFromHandle(handles[index]);
    BootX = NextDevicePathNode(BootX);
    EFI_DEVICE_PATH *fullpath = AppendDevicePath(prefix, BootX);
    Print(L"Booting: %s\n", DevicePathToStr(BootX));

    EFI_HANDLE NextHandle = NULL;

    Status = uefi_call_wrapper(BS->LoadImage, 6,
          FALSE,
          ImageHandle,
          fullpath,
          NULL,
          0,
          &NextHandle
          );

    if (Status != EFI_SUCCESS) {
       Print(L"Failed to LoadImage (%d)\n", Status);
       wait_for_key(SystemTable);
       return EFI_ABORTED;
    }

    Print(L"Starting Boot entry...\n");
    Status = uefi_call_wrapper(BS->StartImage, 3, NextHandle, NULL, NULL);

    FreePool(BootOption);
    FreePool(BootOrder);
    
    return EFI_SUCCESS;
}