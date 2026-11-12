
#include <efi.h>
#include <efilib.h>
#include <stdint.h>

#define ELF_MAGIC 0x464C457F
#define ELF_CLASS_64 2
#define ELF_MACHINE_X86_64 62
#define ELF_PT_LOAD 1

#define PAGE_SIZE 4096

typedef struct
{
    unsigned char ident[16];

    uint16_t type;
    uint16_t machine;

    uint32_t version;

    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;

    uint32_t flags;

    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;

    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;

} Elf64Header;

typedef struct
{
    uint32_t type;
    uint32_t flags;

    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;

    uint64_t filesz;
    uint64_t memsz;

    uint64_t align;

} Elf64ProgramHeader;

typedef struct
{
    uint64_t magic;
    uint64_t framebuffer;
    uint64_t framebuffer_size;
    uint64_t width;
    uint64_t height;
    uint64_t pixels_per_scanline;

} SunBootInfo;

extern void switch_and_jump(
    uint64_t cr3,
    uint64_t stack,
    SunBootInfo *info,
    uint64_t entry
) __attribute__((noreturn));

static EFI_STATUS load_kernel(
    EFI_BOOT_SERVICES *bs,
    EFI_FILE_HANDLE root,
    Elf64Header **header_out
)
{
    EFI_FILE_HANDLE file = NULL;

    EFI_STATUS status = uefi_call_wrapper(
        root->Open,
        5,
        root,
        &file,
        L"\\kernel.elf",
        EFI_FILE_MODE_READ,
        0
    );

    if (EFI_ERROR(status))
        return status;

    UINTN info_size = 512;
    EFI_FILE_INFO *info = NULL;

    status = uefi_call_wrapper(
        bs->AllocatePool,
        3,
        EfiLoaderData,
        info_size,
        (void **)&info
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        file->GetInfo,
        4,
        file,
        &FileInfoGuid,
        &info_size,
        info
    );

    if (EFI_ERROR(status))
        return status;

    UINTN size = (UINTN)info->FileSize;

    void *buffer = NULL;

    status = uefi_call_wrapper(
        bs->AllocatePool,
        3,
        EfiLoaderData,
        size,
        &buffer
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        file->Read,
        3,
        file,
        &size,
        buffer
    );

    if (EFI_ERROR(status))
        return status;

    uefi_call_wrapper(file->Close, 1, file);

    Elf64Header *elf = (Elf64Header *)buffer;

    if (*(uint32_t *)elf->ident != ELF_MAGIC ||
        elf->ident[4] != ELF_CLASS_64 ||
        elf->machine != ELF_MACHINE_X86_64 ||
        elf->phentsize != sizeof(Elf64ProgramHeader))
    {
        return EFI_LOAD_ERROR;
    }

    if (elf->phoff > size ||
        elf->phnum > (size - elf->phoff) /
                    sizeof(Elf64ProgramHeader))
    {
        return EFI_LOAD_ERROR;
    }

    Elf64ProgramHeader *ph =
        (Elf64ProgramHeader *)
        ((uint8_t *)buffer + elf->phoff);

    for (uint16_t i = 0; i < elf->phnum; i++)
    {
        if (ph[i].type != ELF_PT_LOAD)
            continue;

        if (ph[i].filesz > ph[i].memsz ||
            ph[i].offset > size ||
            ph[i].filesz > size - ph[i].offset ||
            ph[i].memsz == 0 ||
            ph[i].paddr != ph[i].vaddr ||
            ph[i].paddr % PAGE_SIZE != 0)
        {
            return EFI_LOAD_ERROR;
        }

        UINTN pages =
            (UINTN)((ph[i].memsz + PAGE_SIZE - 1)
                    / PAGE_SIZE);

        EFI_PHYSICAL_ADDRESS address = ph[i].paddr;

        status = uefi_call_wrapper(
            bs->AllocatePages,
            4,
            AllocateAddress,
            EfiLoaderData,
            pages,
            &address
        );

        if (EFI_ERROR(status))
            return status;

        CopyMem(
            (void *)(UINTN)address,
            (uint8_t *)buffer + ph[i].offset,
            (UINTN)ph[i].filesz
        );

        SetMem(
            (void *)(UINTN)(address + ph[i].filesz),
            (UINTN)(ph[i].memsz - ph[i].filesz),
            0
        );
    }

    *header_out = elf;

    return EFI_SUCCESS;
}

EFI_STATUS EFIAPI efi_main(
    EFI_HANDLE image,
    EFI_SYSTEM_TABLE *system
)
{
    InitializeLib(image, system);

    EFI_BOOT_SERVICES *bs = system->BootServices;

    EFI_LOADED_IMAGE *loaded = NULL;

    EFI_STATUS status = uefi_call_wrapper(
        bs->HandleProtocol,
        3,
        image,
        &LoadedImageProtocol,
        (void **)&loaded
    );

    if (EFI_ERROR(status))
        return status;

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = NULL;

    status = uefi_call_wrapper(
        bs->HandleProtocol,
        3,
        loaded->DeviceHandle,
        &FileSystemProtocol,
        (void **)&fs
    );

    if (EFI_ERROR(status))
        return status;

    EFI_FILE_HANDLE root = NULL;

    status = uefi_call_wrapper(
        fs->OpenVolume,
        2,
        fs,
        &root
    );

    if (EFI_ERROR(status))
        return status;

    Elf64Header *elf = NULL;

    status = load_kernel(bs, root, &elf);

    if (EFI_ERROR(status))
        return status;

    Print(L"sunOS: ELF64 kernel loaded.\r\n");

    return EFI_SUCCESS;
}
