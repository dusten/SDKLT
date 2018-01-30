/*! \file bcma_ha.c
 *
 * HA file I/O.
 */
/*
 * Copyright: (c) 2018 Broadcom. All Rights Reserved. "Broadcom" refers to 
 * Broadcom Limited and/or its subsidiaries.
 * 
 * Broadcom Switch Software License
 * 
 * This license governs the use of the accompanying Broadcom software. Your 
 * use of the software indicates your acceptance of the terms and conditions 
 * of this license. If you do not agree to the terms and conditions of this 
 * license, do not use the software.
 * 1. Definitions
 *    "Licensor" means any person or entity that distributes its Work.
 *    "Software" means the original work of authorship made available under 
 *    this license.
 *    "Work" means the Software and any additions to or derivative works of 
 *    the Software that are made available under this license.
 *    The terms "reproduce," "reproduction," "derivative works," and 
 *    "distribution" have the meaning as provided under U.S. copyright law.
 *    Works, including the Software, are "made available" under this license 
 *    by including in or with the Work either (a) a copyright notice 
 *    referencing the applicability of this license to the Work, or (b) a copy 
 *    of this license.
 * 2. Grant of Copyright License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    copyright license to reproduce, prepare derivative works of, publicly 
 *    display, publicly perform, sublicense and distribute its Work and any 
 *    resulting derivative works in any form.
 * 3. Grant of Patent License
 *    Subject to the terms and conditions of this license, each Licensor 
 *    grants to you a perpetual, worldwide, non-exclusive, and royalty-free 
 *    patent license to make, have made, use, offer to sell, sell, import, and 
 *    otherwise transfer its Work, in whole or in part. This patent license 
 *    applies only to the patent claims licensable by Licensor that would be 
 *    infringed by Licensor's Work (or portion thereof) individually and 
 *    excluding any combinations with any other materials or technology.
 *    If you institute patent litigation against any Licensor (including a 
 *    cross-claim or counterclaim in a lawsuit) to enforce any patents that 
 *    you allege are infringed by any Work, then your patent license from such 
 *    Licensor to the Work shall terminate as of the date such litigation is 
 *    filed.
 * 4. Redistribution
 *    You may reproduce or distribute the Work only if (a) you do so under 
 *    this License, (b) you include a complete copy of this License with your 
 *    distribution, and (c) you retain without modification any copyright, 
 *    patent, trademark, or attribution notices that are present in the Work.
 * 5. Derivative Works
 *    You may specify that additional or different terms apply to the use, 
 *    reproduction, and distribution of your derivative works of the Work 
 *    ("Your Terms") only if (a) Your Terms provide that the limitations of 
 *    Section 7 apply to your derivative works, and (b) you identify the 
 *    specific derivative works that are subject to Your Terms. 
 *    Notwithstanding Your Terms, this license (including the redistribution 
 *    requirements in Section 4) will continue to apply to the Work itself.
 * 6. Trademarks
 *    This license does not grant any rights to use any Licensor's or its 
 *    affiliates' names, logos, or trademarks, except as necessary to 
 *    reproduce the notices described in this license.
 * 7. Limitations
 *    Platform. The Work and any derivative works thereof may only be used, or 
 *    intended for use, with a Broadcom switch integrated circuit.
 *    No Reverse Engineering. You will not use the Work to disassemble, 
 *    reverse engineer, decompile, or attempt to ascertain the underlying 
 *    technology of a Broadcom switch integrated circuit.
 * 8. Termination
 *    If you violate any term of this license, then your rights under this 
 *    license (including the license grants of Sections 2 and 3) will 
 *    terminate immediately.
 * 9. Disclaimer of Warranty
 *    THE WORK IS PROVIDED "AS IS" WITHOUT WARRANTIES OR CONDITIONS OF ANY 
 *    KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WARRANTIES OR CONDITIONS OF 
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE OR 
 *    NON-INFRINGEMENT. YOU BEAR THE RISK OF UNDERTAKING ANY ACTIVITIES UNDER 
 *    THIS LICENSE. SOME STATES' CONSUMER LAWS DO NOT ALLOW EXCLUSION OF AN 
 *    IMPLIED WARRANTY, SO THIS DISCLAIMER MAY NOT APPLY TO YOU.
 * 10. Limitation of Liability
 *    EXCEPT AS PROHIBITED BY APPLICABLE LAW, IN NO EVENT AND UNDER NO LEGAL 
 *    THEORY, WHETHER IN TORT (INCLUDING NEGLIGENCE), CONTRACT, OR OTHERWISE 
 *    SHALL ANY LICENSOR BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY DIRECT, 
 *    INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF 
 *    OR RELATED TO THIS LICENSE, THE USE OR INABILITY TO USE THE WORK 
 *    (INCLUDING BUT NOT LIMITED TO LOSS OF GOODWILL, BUSINESS INTERRUPTION, 
 *    LOST PROFITS OR DATA, COMPUTER FAILURE OR MALFUNCTION, OR ANY OTHER 
 *    COMMERCIAL DAMAGES OR LOSSES), EVEN IF THE LICENSOR HAS BEEN ADVISED OF 
 *    THE POSSIBILITY OF SUCH DAMAGES.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <shr/shr_error.h>
#include <bcmdrd_config.h>
#include <shr/shr_ha.h>
#include <bcma/ha/bcma_ha.h>

/* Maximal name length for shared file */
#define MAX_FILE_NAME_LEN   256

/* Shared file path prefix */
#define SHR_FILE_PREFIX     "/dev/shm"

/* Maintain the shared file descriptors for all units */
static int active_fd[BCMDRD_CONFIG_MAX_UNITS];

/* Maintain pointer to the shared file name for each unit */
static char *active_fname[BCMDRD_CONFIG_MAX_UNITS];

static void *ha_mem_resize(uint32_t curr_file_size,
                           void *old_mmap,
                           uint32_t old_size,
                           uint32_t addition,
                           uint32_t *actual_addition,
                           void *ctx)
{
    void *mmap_ptr;
    uint32_t addition_mem_size;
    int sys_page_size = sysconf(_SC_PAGESIZE);
    int fd = *(int *)ctx;

    /* Set the size to be on a page boundary */
    addition_mem_size = sys_page_size *
                        ((addition + sys_page_size - 1) / sys_page_size);
    /* Extend the shared file size */
    if (ftruncate(fd, curr_file_size + addition_mem_size) != 0) {
        return NULL;
    }
    /* Create another memory map section */
    if ((mmap_ptr = mmap(NULL,
                       addition_mem_size,
                       PROT_WRITE | PROT_READ,
                       MAP_SHARED, fd, curr_file_size)) == MAP_FAILED) {
        return NULL;
    }

    *actual_addition = addition_mem_size;
    return mmap_ptr;
}

/*
 * This function construct the file name for the share file. For cold boot
 * it will create unique name that is not already exist. For warm boot it
 * will create the generic name (based on the user name).
*/
static int build_file_name(int unit, bool warm)
{
    struct passwd *pwd;
    int j;
    struct stat f_info;
    char *fname;
    int len;
    /* 8 is the prefix string length of SHR_FILE_PREFIX */
    char path[MAX_FILE_NAME_LEN+8];

    pwd = getpwuid(getuid());
    if (pwd == NULL) {
        return SHR_E_INTERNAL;
    }

    active_fname[unit] = malloc(MAX_FILE_NAME_LEN);
    if (!active_fname[unit]) {
        return SHR_E_MEMORY;
    }
    fname = active_fname[unit];

    len = snprintf(fname, MAX_FILE_NAME_LEN,
             "/%s_%s_%d", pwd->pw_name, SHARED_FILE_NAME, unit);
    if (len >= MAX_FILE_NAME_LEN-1) {
        return SHR_E_INTERNAL;
    }

    /* Don't care about name uniqueness for warm boot */
    if (warm) {
        return SHR_E_NONE;
    }

    j = 0;
    /* Create new file names until finding that no file has this name. */
    do {
        snprintf(path, sizeof(path), "%s%s", SHR_FILE_PREFIX, fname);
        if (stat(path, &f_info) != 0) {
            if (errno == ENOENT) {
                return SHR_E_NONE;
            } else {
                return SHR_E_INTERNAL;
            }
        }
        len = snprintf(fname, MAX_FILE_NAME_LEN,
                 "/%s_%s%d_%d", pwd->pw_name, SHARED_FILE_NAME, j++, unit);
        if (len >= MAX_FILE_NAME_LEN-1) {
            return SHR_E_INTERNAL;
        }
    } while (1);
}

/*
 * This function being called for cold boot. In this scenario, the shared
 * directory should be emptied out from all HA files associated with the
 * current user. This function therefore scans the shared directory and
 * deletes every file with prefix name matches the HA user name file.
*/
static void clear_old_files(void)
{
    struct passwd *pwd;
    int len;
    DIR *dp;
    struct dirent *ep;
    char path[MAX_FILE_NAME_LEN];

    pwd = getpwuid(getuid());
    if (pwd == NULL) {
        return;
    }

    /* Prepare the basic file name */
    len = snprintf(path, MAX_FILE_NAME_LEN,
                   "%s_%s", pwd->pw_name, SHARED_FILE_NAME);
    if (len >= MAX_FILE_NAME_LEN - 1) {
        return;
    }

    /* Search the shared file directory for files with the same prefix */
    dp = opendir(SHR_FILE_PREFIX);
    while ((ep = readdir (dp)) != NULL) {
        if (strncmp(ep->d_name, path, strlen(path)) == 0) {
            shm_unlink(ep->d_name);
        }
    }
    closedir (dp);
}

void bcma_ha_init(bool enabled, bool warm)
{
    if (!enabled || warm) {
        return;
    }

    clear_old_files();
}

int bcma_ha_unit_open(int unit, int size, bool enabled, bool warm)
{
    struct stat stat_buf;
    int fh = -1;
    void *mmap_ptr = NULL;
    long sys_page_size = sysconf(_SC_PAGESIZE);
    long file_size = sys_page_size * ((size + sys_page_size-1) / sys_page_size);
    bool new_file = false;
    char *fname;
    int rv;

    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        (active_fd[unit] > 0)) {
        return SHR_E_UNIT;
    }

    if (enabled) {
        rv = build_file_name(unit, warm);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        fname = active_fname[unit];
        if (warm) {
            /* Try to open the existing shared file */
            fh = shm_open(fname, O_RDWR, (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
        } else {
            /* Existing file can't be opened */
            fh = -1;
        }
        if (fh < 0) {
            /* Existing file is not there, create new file */
            new_file = true;
            fh = shm_open(fname, (int)(O_CREAT | O_RDWR),
                          (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
            if (fh < 0) {
                return SHR_E_FAIL;
            }
            /* When file created its size set to 0. Adjust it to the desired size */
            if (ftruncate(fh, file_size) != 0) {
                close(fh);
                return SHR_E_FAIL;
            }
        } else {   /* File opened successfully without the create flag */
            new_file = false;
            if (fstat(fh, &stat_buf) == 0) { /* Get the current file size */
                if (stat_buf.st_size < (off_t)file_size) {
                    /* Increase the file size if too small */
                    if (ftruncate(fh, file_size) != 0) {
                        close(fh);
                        return SHR_E_FAIL;
                    }
                } else {
                    /* Update the blk_len according to the file size */
                    file_size = stat_buf.st_size;
                }
            }
        }

        /* Memory map the file */
        if ((mmap_ptr = mmap(NULL, file_size,
                             PROT_WRITE | PROT_READ,
                             MAP_SHARED, fh, 0)) == MAP_FAILED) {
            close (fh);
            return SHR_E_FAIL;
        }
    }

    active_fd[unit] = fh;
    return shr_ha_init(unit,
                   mmap_ptr, file_size,
                   enabled, new_file, ha_mem_resize, &active_fd[unit]);
}

static void ha_blk_free(void *mmap_ptr, uint32_t len)
{
    munmap(mmap_ptr, len);
}


int bcma_ha_unit_close(int unit, bool warm_next)
{
    int rv;
    char *fname;

    if ((unit < 0) || (unit >= BCMDRD_CONFIG_MAX_UNITS) ||
        (active_fd[unit] <= 0)) {
        return SHR_E_UNIT;
    }
    rv = shr_ha_close(unit, ha_blk_free);
    if (rv != SHR_E_NONE) {
        return rv;
    }

    close(active_fd[unit]);
    active_fd[unit] = 0;
    fname = active_fname[unit];
    if (!warm_next) {
        shm_unlink(fname);
    }
    free(fname);
    active_fname[unit] = NULL;
    return SHR_E_NONE;
}
