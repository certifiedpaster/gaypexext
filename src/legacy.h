#include <windows.h>
#include "vmprotect.h"

// meine proffessional processhacker copy-pasta from Rust project

typedef struct _ModuleDll
{
	uintptr_t baseaddress;
	char* name;
} ModuleDll;

#define MODULE_MAX 64
typedef struct _ModuleInfo 
{
	ModuleDll list[MODULE_MAX];
} ModuleInfo;

typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
    MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
    MemoryMappedFilenameInformation, // UNICODE_STRING
    MemoryRegionInformation, // MEMORY_REGION_INFORMATION
    MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
    MemorySharedCommitInformation, // MEMORY_SHARED_COMMIT_INFORMATION
    MemoryImageInformation, // MEMORY_IMAGE_INFORMATION
    MemoryRegionInformationEx,
    MemoryPrivilegedBasicInformation,
    MemoryEnclaveImageInformation, // MEMORY_ENCLAVE_IMAGE_INFORMATION // since REDSTONE3
    MemoryBasicInformationCapped
} MEMORY_INFORMATION_CLASS;

/*typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    _Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, * PUNICODE_STRING;*/


extern "C" 
{
    NTSYSCALLAPI
    NTSTATUS
    NTAPI
    NtQueryVirtualMemory(
        _In_ HANDLE ProcessHandle,
        _In_opt_ PVOID BaseAddress,
        _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
        _Out_writes_bytes_(MemoryInformationLength) PVOID MemoryInformation,
        _In_ SIZE_T MemoryInformationLength,
        _Out_opt_ PSIZE_T ReturnLength
    );

    NTSYSAPI
    PVOID
    NTAPI
    RtlAllocateHeap(
        _In_ PVOID HeapHandle,
        _In_opt_ ULONG Flags,
        _In_ SIZE_T Size
    );
}


typedef NTSTATUS(NTAPI* PRTL_HEAP_COMMIT_ROUTINE)(
    _In_ PVOID Base,
    _Inout_ PVOID* CommitAddress,
    _Inout_ PSIZE_T CommitSize
    );

/*typedef struct _RTL_HEAP_PARAMETERS
{
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[2];
} RTL_HEAP_PARAMETERS, * PRTL_HEAP_PARAMETERS;*/

#define HEAP_SETTABLE_USER_VALUE 0x00000100
#define HEAP_SETTABLE_USER_FLAG1 0x00000200
#define HEAP_SETTABLE_USER_FLAG2 0x00000400
#define HEAP_SETTABLE_USER_FLAG3 0x00000800
#define HEAP_SETTABLE_USER_FLAGS 0x00000e00

#define HEAP_CLASS_0 0x00000000 // Process heap
#define HEAP_CLASS_1 0x00001000 // Private heap
#define HEAP_CLASS_2 0x00002000 // Kernel heap
#define HEAP_CLASS_3 0x00003000 // GDI heap
#define HEAP_CLASS_4 0x00004000 // User heap
#define HEAP_CLASS_5 0x00005000 // Console heap
#define HEAP_CLASS_6 0x00006000 // User desktop heap
#define HEAP_CLASS_7 0x00007000 // CSR shared heap
#define HEAP_CLASS_8 0x00008000 // CSR port heap
#define HEAP_CLASS_MASK 0x0000f000

NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHeap(
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    PVOID BaseAddress
);

bool MemoryReadable(char* ptr, size_t byteCount)
{
    ProtectStart();
    
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == 0)
        return false;

    if (mbi.State != MEM_COMMIT)
        return false;

    if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_EXECUTE)
        return false;

    if (!(mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_READONLY)) 
        return false;

    // This checks that the start of memory block is in the same "region" as the
    // end. If it isn't you "simplify" the problem into checking that the rest of 
    // the memory is readable.
    size_t blockOffset = (size_t)((char *)ptr - (char *)mbi.AllocationBase);
    size_t blockBytesPostPtr = mbi.RegionSize - blockOffset;

    if (blockBytesPostPtr < byteCount)
        return MemoryReadable(ptr + blockBytesPostPtr,
                            byteCount - blockBytesPostPtr);

    return true;

    ProtectEnd();
}

char* GetFileNameFromMemory(HANDLE proc, PVOID baseaddress) 
{
	ProtectStart();
    
    int bufferSize = 0x100;
	char* buffer = (char*)malloc(bufferSize);
	SIZE_T returnLength;

	NTSTATUS status = NtQueryVirtualMemory(proc, baseaddress, MemoryMappedFilenameInformation, buffer, bufferSize, &returnLength);
	
	if (status == ((NTSTATUS)0x80000005L)) // STATUS_BUFFER_OVERFLOW 
	{
		free(buffer);
		bufferSize = returnLength;
		buffer = (char*)malloc(bufferSize);

		status = NtQueryVirtualMemory(
			proc,
			baseaddress,
			MemoryMappedFilenameInformation,
			buffer,
			bufferSize,
			&returnLength
		);
	}

	if (status != 0x0) 
	{
		return 0;
	}

    // completely retarded access violation
    if (!(MemoryReadable((char*)buffer, (sizeof(USHORT) * 2)))) 
    {
        printf(E("[e] Invalid allocation at: %p\n"), (char*)buffer);
        return 0;
    }

	PUNICODE_STRING unistr = (PUNICODE_STRING)buffer;
    if (unistr->Length < 5 || unistr->Length >= 250) 
    {
        printf(E("[e] Invalid size for: %p\n"), (char*)unistr);
        return 0;
    }

    if (MemoryReadable((char*)unistr->Buffer, unistr->Length)) 
    {
        char* multi = (char*)malloc((int)unistr->Length);
        if (!multi) return 0;
        memset(multi, 0, unistr->Length);

        WideCharToMultiByte(CP_ACP, 0, unistr->Buffer, unistr->Length, multi, unistr->Length, 0, 0);
        free(buffer);

        printf(E("[m] %s\n"), multi);

        return multi;
    } else 
    {
        printf(E("[e] Invalid memory region at: %p\n"), (char*)unistr);
        return 0;
    }

    ProtectEnd();
}

#define PTR_ADD_OFFSET(Pointer, Offset) ((PVOID)((ULONG_PTR)(Pointer) + (ULONG_PTR)(Offset)))
ModuleInfo GetProcessModules(int pid) 
{
	ProtectStart();
    
    ModuleInfo mi = { 0 };
	MEMORY_BASIC_INFORMATION basicInfo;
	int total = 0;

	HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
	if (proc == 0 || proc == INVALID_HANDLE_VALUE) 
	{
        CloseHandle(proc);
        return mi;
	}

	PVOID baseAddress = (PVOID)0;

	if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION))) 
	{
		CloseHandle(proc);
        return mi;
	}

	int querySucceeded = 1;

    while (querySucceeded)
    {
        PVOID allocationBase;
        SIZE_T allocationSize;
        char* fileName;

        if (basicInfo.Type == MEM_MAPPED || basicInfo.Type == MEM_IMAGE)
        {
            allocationBase = basicInfo.AllocationBase;
            allocationSize = 0;

            do
            {
                baseAddress = PTR_ADD_OFFSET(baseAddress, basicInfo.RegionSize);
                allocationSize += basicInfo.RegionSize;

                /*if (!NT_SUCCESS(NtQueryVirtualMemory(
                    ProcessHandle,
                    baseAddress,
                    MemoryBasicInformation,
                    &basicInfo,
                    sizeof(MEMORY_BASIC_INFORMATION),
                    NULL
                )))
                {
                    querySucceeded = FALSE;
                    break;
                }*/
				if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION)))
				{
					querySucceeded = FALSE;
					break;
				}
            } while (basicInfo.AllocationBase == allocationBase);

            /*if (!NT_SUCCESS(PhGetProcessMappedFileName(
                ProcessHandle,
                allocationBase,
                &fileName
            )))
            {
                continue;
            }*/

			char* filename = GetFileNameFromMemory(proc, allocationBase);
			if (filename != 0) 
			{
				mi.list[total].name = filename;
				mi.list[total].baseaddress = (uintptr_t)allocationBase;
				total++;
                if (strstr(filename, E("r5apex.exe"))) 
                {
                    CloseHandle(proc);
	                return mi;
                }
				if (total >= MODULE_MAX) 
				{
					CloseHandle(proc);
	                return mi;
				}
			}
        }
        else
        {
            baseAddress = PTR_ADD_OFFSET(baseAddress, basicInfo.RegionSize);

            /*if (!NT_SUCCESS(NtQueryVirtualMemory(
                ProcessHandle,
                baseAddress,
                MemoryBasicInformation,
                &basicInfo,
                sizeof(MEMORY_BASIC_INFORMATION),
                NULL
            )))
            {
                querySucceeded = FALSE;
            }*/
			if (!VirtualQueryEx(proc, baseAddress, &basicInfo, sizeof(MEMORY_BASIC_INFORMATION)))
			{
				querySucceeded = FALSE;
			}
        }
    }

	/*
	if (!NT_SUCCESS(NtQueryVirtualMemory(
		proc,
		baseAddress,
		MemoryBasicInformation,
		&basicInfo,
		sizeof(MEMORY_BASIC_INFORMATION),
		NULL
	)))
	{
		return;
	}*/

	CloseHandle(proc);
	return mi;

    ProtectEnd();
}