#define WINAPI_PARTITION_DESKTOP   0x00000001
#define WINAPI_FAMILY_DESKTOP_APP  WINAPI_PARTITION_DESKTOP

// WINAPI_FAMILY is usually defined by the build system based on the target platform
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif

#define WINAPI_FAMILY_PARTITION(Partition)   ((WINAPI_FAMILY & Partition) == Partition)