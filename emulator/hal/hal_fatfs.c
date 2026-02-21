/*
 * hal_fatfs.c - FatFS to POSIX Wrapper for CRC-25R Emulator
 *
 * Replaces fatfs/src/ff.c and fatfs/src/ff_diskio.c. Provides the same
 * FatFS API (f_open, f_read, f_write, etc.) but maps all filesystem
 * operations to POSIX file I/O against a local "data/" directory.
 *
 * The firmware uses FatFS to access an SD card filesystem. In the emulator,
 * firmware paths like "/data/capintec.db" or "0:/filename.txt" are mapped
 * to the local "data/" directory on disk.
 *
 * Path mapping rules:
 *   - Strip leading "0:/" or "0:" prefix (drive letter notation)
 *   - Strip leading "/" prefix
 *   - Prepend "data/"
 *   Example: "0:/data/capintec.db" -> "data/data/capintec.db"
 *            "/somefile.txt"       -> "data/somefile.txt"
 *
 * IMPORTANT: FatFS ff.h defines its own DIR type, which conflicts with
 * the POSIX DIR from <dirent.h>. To avoid this, we include <dirent.h>
 * BEFORE ff.h and use the native DIR typedef via a wrapper.
 * All POSIX directory handles are stored as void* in the side table.
 */

/* FatFS DIR is renamed to FF_DIR in ff.h to avoid POSIX conflict. */
#include "compat.h"
#include "crc.h"
#include "ff.h"

#include <sys/types.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

/* ---- Open File Table ----
 * FatFS FIL structs don't have room for a native FILE* pointer.
 * We maintain a side table mapping slot indices to FILE* handles.
 * The slot index is stored in FIL.pad1 (a BYTE field).
 */
#define MAX_OPEN_FILES 32
#define SLOT_UNUSED    0
#define SLOT_USED      1

static FILE *open_files[MAX_OPEN_FILES];
static int   slot_used[MAX_OPEN_FILES];

/* ---- Open Directory Table ----
 * POSIX DIR* handles are stored as void* to avoid the type name
 * collision with FatFS DIR struct. */
#define MAX_OPEN_DIRS  8

static void *open_dirs_native[MAX_OPEN_DIRS];
static int   dir_slot_used[MAX_OPEN_DIRS];
static char  dir_paths[MAX_OPEN_DIRS][512];

/* ---- Static FATFS instance ---- */
static FATFS hal_fatfs_obj;

/* ---- Helper: Allocate a file slot ---- */
static int alloc_file_slot(void)
{
    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (!slot_used[i]) {
            slot_used[i] = SLOT_USED;
            open_files[i] = NULL;
            return i;
        }
    }
    return -1; /* no free slots */
}

/* ---- Helper: Free a file slot ---- */
static void free_file_slot(int slot)
{
    if (slot >= 0 && slot < MAX_OPEN_FILES) {
        slot_used[slot] = SLOT_UNUSED;
        open_files[slot] = NULL;
    }
}

/* ---- Helper: Allocate a directory slot ---- */
static int alloc_dir_slot(void)
{
    int i;
    for (i = 0; i < MAX_OPEN_DIRS; i++) {
        if (!dir_slot_used[i]) {
            dir_slot_used[i] = 1;
            open_dirs_native[i] = NULL;
            return i;
        }
    }
    return -1;
}

/* ---- Helper: Free a directory slot ---- */
static void free_dir_slot(int slot)
{
    if (slot >= 0 && slot < MAX_OPEN_DIRS) {
        dir_slot_used[slot] = 0;
        open_dirs_native[slot] = NULL;
    }
}

/* ---- SD Root Path ----
 * The process starts by chdir("data") into the SD card root directory.
 * We record the absolute POSIX path of this root so that absolute FatFS
 * paths (like "0:/data/temp.db") can be resolved correctly even after
 * subsequent f_chdir() calls change the process CWD.
 */
static char sd_root_path[512] = "";

void hal_fatfs_set_sd_root(void)
{
    if (getcwd(sd_root_path, sizeof(sd_root_path)) == NULL) {
        sd_root_path[0] = '\0';
    }
}

/* ---- Path Mapping ----
 * Convert firmware FatFS path to local POSIX path.
 *
 * There are two cases:
 * 1. Simple relative paths (e.g., "temp.db", "capintec.db") - these are
 *    resolved relative to the current process CWD (set by f_chdir).
 * 2. Absolute FatFS paths (e.g., "0:/data/temp.db") - these are paths
 *    from the FatFS root. We resolve them against the stored SD root path
 *    to produce an absolute POSIX path, avoiding double-directory issues
 *    when the CWD has been changed by f_chdir.
 */
static void map_path(const TCHAR *fatfs_path, char *posix_path, int maxlen)
{
    const char *p = (const char *)fatfs_path;
    int is_absolute = 0;

    /* Detect and strip drive letter prefix "0:" or "0:/" */
    if (p[0] >= '0' && p[0] <= '9' && p[1] == ':') {
        is_absolute = 1;
        p += 2;
        if (*p == '/' || *p == '\\')
            p++;
    }

    /* Detect leading slash (also absolute from FatFS root) */
    if (*p == '/' || *p == '\\') {
        is_absolute = 1;
        while (*p == '/' || *p == '\\')
            p++;
    }

    if (is_absolute && sd_root_path[0]) {
        /* Absolute FatFS path: resolve against the stored SD root */
        if (*p)
            snprintf(posix_path, maxlen, "%s/%s", sd_root_path, p);
        else
            snprintf(posix_path, maxlen, "%s", sd_root_path);
    } else {
        /* Relative path: resolve against CWD as before */
        snprintf(posix_path, maxlen, "%s", *p ? p : ".");
    }

    /* Convert backslashes to forward slashes */
    {
        char *s;
        for (s = posix_path; *s; s++) {
            if (*s == '\\') *s = '/';
        }
    }
}

/* ---- Helper: Create parent directories recursively ---- */
static void ensure_parent_dirs(const char *path)
{
    char tmp[512];
    char *p;

    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    /* Find last slash */
    p = strrchr(tmp, '/');
    if (!p) return;
    *p = '\0';

    /* Recursively create directories */
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

/* ================================================================
 * FatFS API Implementation
 * ================================================================ */

/*
 * f_mount - Mount/Unmount a logical drive
 *
 * In the emulator, this creates the "data/" directory if it does
 * not exist and initializes the FATFS structure.
 */
FRESULT f_mount(BYTE drive, FATFS *fs)
{
    (void)drive;

    /* CWD is already the SD card root ("data/" from original dir).
     * No directory creation needed here. */

    if (fs) {
        memset(fs, 0, sizeof(FATFS));
        fs->fs_type = FS_FAT32; /* Pretend we have a FAT32 volume */
        fs->free_clust = 1000000; /* Large free space */
        fs->n_fatent = 1000000;
        fs->csize = 8; /* 4KB clusters */
    }

    /* Initialize our internal FATFS object as well */
    memset(&hal_fatfs_obj, 0, sizeof(FATFS));
    hal_fatfs_obj.fs_type = FS_FAT32;
    hal_fatfs_obj.free_clust = 1000000;
    hal_fatfs_obj.n_fatent = 1000000;
    hal_fatfs_obj.csize = 8;

    /* Clear file and directory tables */
    memset(open_files, 0, sizeof(open_files));
    memset(slot_used, 0, sizeof(slot_used));
    memset(open_dirs_native, 0, sizeof(open_dirs_native));
    memset(dir_slot_used, 0, sizeof(dir_slot_used));

    return FR_OK;
}

/*
 * f_open - Open or create a file
 *
 * Maps the FatFS path to a local path under "data/" and opens the
 * file with the appropriate POSIX mode flags.
 */
FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode)
{
    char posix_path[512];
    const char *fmode;
    FILE *f;
    int slot;
    long fsize;

    if (!fp || !path)
        return FR_INVALID_OBJECT;

    map_path(path, posix_path, sizeof(posix_path));

    /* Determine fopen mode string from FatFS flags */
    if ((mode & FA_CREATE_ALWAYS) && (mode & FA_WRITE)) {
        /* Create always + write: truncate or create */
        if (mode & FA_READ)
            fmode = "w+b";
        else
            fmode = "wb";
    } else if ((mode & FA_CREATE_NEW) && (mode & FA_WRITE)) {
        /* Create new: fail if exists */
        FILE *test = fopen(posix_path, "rb");
        if (test) {
            fclose(test);
            return FR_EXIST;
        }
        if (mode & FA_READ)
            fmode = "w+b";
        else
            fmode = "wb";
    } else if ((mode & FA_OPEN_ALWAYS) && (mode & FA_WRITE)) {
        /* Open always: open existing or create, don't truncate */
        if (mode & FA_READ)
            fmode = "r+b";
        else
            fmode = "r+b"; /* need read capability for append positioning */
        /* Try to open existing first */
        f = fopen(posix_path, fmode);
        if (!f) {
            /* File doesn't exist, create it */
            ensure_parent_dirs(posix_path);
            if (mode & FA_READ)
                fmode = "w+b";
            else
                fmode = "wb";
        } else {
            /* File existed, use it directly */
            goto have_file;
        }
    } else if (mode & FA_WRITE) {
        /* Write to existing file */
        if (mode & FA_READ)
            fmode = "r+b";
        else
            fmode = "r+b";
    } else {
        /* Read only */
        fmode = "rb";
    }

    /* Create parent directories if writing */
    if (mode & (FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW | FA_OPEN_ALWAYS)) {
        ensure_parent_dirs(posix_path);
    }

    f = fopen(posix_path, fmode);
    if (!f) {
        /* If opening for read+write failed, file may not exist */
        if (errno == ENOENT)
            return FR_NO_FILE;
        return FR_DISK_ERR;
    }

have_file:
    /* Allocate a slot */
    slot = alloc_file_slot();
    if (slot < 0) {
        fclose(f);
        return FR_TOO_MANY_OPEN_FILES;
    }

    open_files[slot] = f;

    /* Get file size */
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* Initialize the FIL struct */
    memset(fp, 0, sizeof(FIL));
    fp->fs = &hal_fatfs_obj;
    fp->flag = mode;
    fp->pad1 = (BYTE)slot; /* Store slot index in pad1 */
    fp->fptr = 0;
    fp->fsize = (DWORD)(fsize >= 0 ? fsize : 0);

    return FR_OK;
}

/*
 * f_close - Close an open file object
 */
FRESULT f_close(FIL *fp)
{
    int slot;
    FILE *f;

    if (!fp)
        return FR_INVALID_OBJECT;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (f) {
        fclose(f);
    }

    free_file_slot(slot);
    fp->fs = NULL;

    return FR_OK;
}

/*
 * f_read - Read data from a file
 */
FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br)
{
    int slot;
    FILE *f;
    size_t n;

    if (!fp || !buff || !br)
        return FR_INVALID_OBJECT;

    *br = 0;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (!f)
        return FR_INVALID_OBJECT;

    n = fread(buff, 1, (size_t)btr, f);
    *br = (UINT)n;
    fp->fptr += (DWORD)n;

    return FR_OK;
}

/*
 * f_write - Write data to a file
 */
FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw)
{
    int slot;
    FILE *f;
    size_t n;

    if (!fp || !buff || !bw)
        return FR_INVALID_OBJECT;

    *bw = 0;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (!f)
        return FR_INVALID_OBJECT;

    n = fwrite(buff, 1, (size_t)btw, f);
    *bw = (UINT)n;
    fp->fptr += (DWORD)n;

    /* Update file size if we wrote past the end */
    if (fp->fptr > fp->fsize)
        fp->fsize = fp->fptr;

    return FR_OK;
}

/*
 * f_lseek - Move file read/write pointer
 */
FRESULT f_lseek(FIL *fp, DWORD ofs)
{
    int slot;
    FILE *f;

    if (!fp)
        return FR_INVALID_OBJECT;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (!f)
        return FR_INVALID_OBJECT;

    if (fseek(f, (long)ofs, SEEK_SET) != 0)
        return FR_DISK_ERR;

    fp->fptr = ofs;
    return FR_OK;
}

/*
 * f_sync - Flush cached data of a writing file
 */
FRESULT f_sync(FIL *fp)
{
    int slot;
    FILE *f;

    if (!fp)
        return FR_INVALID_OBJECT;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (f)
        fflush(f);

    return FR_OK;
}

/*
 * f_truncate - Truncate file at current position
 */
FRESULT f_truncate(FIL *fp)
{
    int slot;
    FILE *f;
    int fd;

    if (!fp)
        return FR_INVALID_OBJECT;

    slot = (int)fp->pad1;
    if (slot < 0 || slot >= MAX_OPEN_FILES || !slot_used[slot])
        return FR_INVALID_OBJECT;

    f = open_files[slot];
    if (!f)
        return FR_INVALID_OBJECT;

    /* Flush before truncating */
    fflush(f);

    fd = fileno(f);
    if (fd >= 0) {
        if (ftruncate(fd, (off_t)fp->fptr) != 0)
            return FR_DISK_ERR;
    }

    fp->fsize = fp->fptr;
    return FR_OK;
}

/*
 * f_getfree - Get number of free clusters on the drive
 *
 * In the emulator, we report a large amount of free space
 * since we're backed by the host filesystem.
 */
FRESULT f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs)
{
    (void)path;

    if (nclst)
        *nclst = 500000; /* ~2GB free at 4KB clusters */

    if (fatfs)
        *fatfs = &hal_fatfs_obj;

    return FR_OK;
}

/*
 * f_opendir - Open a directory for reading
 *
 * Note: The 'dp' parameter is FatFS DIR (from ff.h), not POSIX DIR.
 * The native POSIX DIR* is stored as void* in our side table.
 */
FRESULT f_opendir(FF_DIR *dp, const TCHAR *path)
{
    char posix_path[512];
    DIR *native_dir;
    int slot;

    if (!dp || !path)
        return FR_INVALID_OBJECT;

    map_path(path, posix_path, sizeof(posix_path));

    /* Ensure the directory exists */
    mkdir(posix_path, 0755);

    native_dir = opendir(posix_path);
    if (!native_dir)
        return FR_NO_PATH;

    slot = alloc_dir_slot();
    if (slot < 0) {
        closedir(native_dir);
        return FR_TOO_MANY_OPEN_FILES;
    }

    open_dirs_native[slot] = (void *)native_dir;
    strncpy(dir_paths[slot], posix_path, sizeof(dir_paths[slot]) - 1);
    dir_paths[slot][sizeof(dir_paths[slot]) - 1] = '\0';

    /* Initialize the FatFS DIR struct */
    memset(dp, 0, sizeof(*dp));
    dp->fs = &hal_fatfs_obj;
    dp->index = (WORD)slot; /* Store slot index */

    return FR_OK;
}

/*
 * f_readdir - Read a directory entry
 *
 * When fno is NULL, the directory read index is rewound.
 * When fno->fname[0] is set to '\0', there are no more entries.
 */
FRESULT f_readdir(FF_DIR *dp, FILINFO *fno)
{
    int slot;
    DIR *native_dir;
    struct dirent *entry;
    struct stat st;
    char fullpath[768];

    if (!dp)
        return FR_INVALID_OBJECT;

    slot = (int)dp->index;
    if (slot < 0 || slot >= MAX_OPEN_DIRS || !dir_slot_used[slot])
        return FR_INVALID_OBJECT;

    native_dir = (DIR *)open_dirs_native[slot];
    if (!native_dir)
        return FR_INVALID_OBJECT;

    /* NULL fno means rewind */
    if (!fno) {
        rewinddir(native_dir);
        return FR_OK;
    }

    /* Read next entry, skipping "." and ".." */
    for (;;) {
        entry = readdir(native_dir);
        if (!entry) {
            /* No more entries */
            fno->fname[0] = '\0';
            return FR_OK;
        }

        /* Skip . and .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        break;
    }

    /* Fill in FILINFO */
    strncpy(fno->fname, entry->d_name, 12);
    fno->fname[12] = '\0';

    /* Get file stats */
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_paths[slot], entry->d_name);
    if (stat(fullpath, &st) == 0) {
        fno->fsize = (DWORD)st.st_size;
        fno->fattrib = S_ISDIR(st.st_mode) ? AM_DIR : 0;

        /* Convert modification time to FAT date/time format */
        {
            struct tm *tm_info = localtime(&st.st_mtime);
            if (tm_info) {
                fno->fdate = (WORD)(((tm_info->tm_year - 80) << 9) |
                                    ((tm_info->tm_mon + 1) << 5) |
                                    tm_info->tm_mday);
                fno->ftime = (WORD)((tm_info->tm_hour << 11) |
                                    (tm_info->tm_min << 5) |
                                    (tm_info->tm_sec / 2));
            }
        }
    } else {
        fno->fsize = 0;
        fno->fattrib = 0;
        fno->fdate = 0;
        fno->ftime = 0;
    }

    return FR_OK;
}

/*
 * f_stat - Get file status
 */
FRESULT f_stat(const TCHAR *path, FILINFO *fno)
{
    char posix_path[512];
    struct stat st;

    if (!path || !fno)
        return FR_INVALID_OBJECT;

    map_path(path, posix_path, sizeof(posix_path));

    if (stat(posix_path, &st) != 0) {
        if (errno == ENOENT)
            return FR_NO_FILE;
        return FR_DISK_ERR;
    }

    fno->fsize = (DWORD)st.st_size;
    fno->fattrib = S_ISDIR(st.st_mode) ? AM_DIR : 0;

    /* Convert modification time to FAT date/time */
    {
        struct tm *tm_info = localtime(&st.st_mtime);
        if (tm_info) {
            fno->fdate = (WORD)(((tm_info->tm_year - 80) << 9) |
                                ((tm_info->tm_mon + 1) << 5) |
                                tm_info->tm_mday);
            fno->ftime = (WORD)((tm_info->tm_hour << 11) |
                                (tm_info->tm_min << 5) |
                                (tm_info->tm_sec / 2));
        } else {
            fno->fdate = 0;
            fno->ftime = 0;
        }
    }

    memset(fno->fname, 0, sizeof(fno->fname));

    return FR_OK;
}

/*
 * f_mkdir - Create a new directory
 */
FRESULT f_mkdir(const TCHAR *path)
{
    char posix_path[512];

    if (!path)
        return FR_INVALID_NAME;

    map_path(path, posix_path, sizeof(posix_path));
    ensure_parent_dirs(posix_path);

    if (mkdir(posix_path, 0755) != 0) {
        if (errno == EEXIST)
            return FR_EXIST;
        return FR_DISK_ERR;
    }

    return FR_OK;
}

/*
 * f_unlink - Delete an existing file or directory
 */
FRESULT f_unlink(const TCHAR *path)
{
    char posix_path[512];
    struct stat st;

    if (!path)
        return FR_INVALID_NAME;

    map_path(path, posix_path, sizeof(posix_path));

    if (stat(posix_path, &st) != 0)
        return FR_NO_FILE;

    if (S_ISDIR(st.st_mode)) {
        if (rmdir(posix_path) != 0)
            return FR_DENIED;
    } else {
        if (remove(posix_path) != 0)
            return FR_DENIED;
    }

    return FR_OK;
}

/*
 * f_rename - Rename/Move a file or directory
 */
FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new)
{
    char old_posix[512];
    char new_posix[512];

    if (!path_old || !path_new)
        return FR_INVALID_NAME;

    map_path(path_old, old_posix, sizeof(old_posix));
    map_path(path_new, new_posix, sizeof(new_posix));

    ensure_parent_dirs(new_posix);

    if (rename(old_posix, new_posix) != 0) {
        if (errno == ENOENT)
            return FR_NO_FILE;
        return FR_DENIED;
    }

    return FR_OK;
}

/*
 * f_chmod - Change file/directory attributes (no-op in emulator)
 */
FRESULT f_chmod(const TCHAR *path, BYTE value, BYTE mask)
{
    (void)path;
    (void)value;
    (void)mask;
    return FR_OK;
}

/*
 * f_utime - Change file/directory timestamp (no-op in emulator)
 */
FRESULT f_utime(const TCHAR *path, const FILINFO *fno)
{
    (void)path;
    (void)fno;
    return FR_OK;
}

/*
 * f_mkfs - Create a file system on the drive (no-op in emulator)
 */
FRESULT f_mkfs(BYTE drive, BYTE partition, UINT allocsize)
{
    (void)drive;
    (void)partition;
    (void)allocsize;

    /* CWD is already the SD card root. Nothing to do. */
    return FR_OK;
}

/*
 * get_fattime - Get current time in FAT timestamp format
 *
 * Called by FatFS when creating or modifying files.
 * Returns packed date/time in FAT format.
 *
 * bit[31:25] Year since 1980 (0-127)
 * bit[24:21] Month (1-12)
 * bit[20:16] Day (1-31)
 * bit[15:11] Hour (0-23)
 * bit[10:5]  Minute (0-59)
 * bit[4:0]   Second/2 (0-29)
 */
DWORD get_fattime(void)
{
    time_t now;
    struct tm *tm_info;

    now = time(NULL);
    tm_info = localtime(&now);

    if (!tm_info)
        return 0;

    return ((DWORD)(tm_info->tm_year - 80) << 25) |
           ((DWORD)(tm_info->tm_mon + 1) << 21) |
           ((DWORD)tm_info->tm_mday << 16) |
           ((DWORD)tm_info->tm_hour << 11) |
           ((DWORD)tm_info->tm_min << 5) |
           ((DWORD)(tm_info->tm_sec / 2));
}
