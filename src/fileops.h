/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_fileops_h__
#define INCLUDE_fileops_h__

#include "common.h"
#include "map.h"
#include "posix.h"
#include "path.h"

/**
 * Filebuffer methods
 *
 * Read whole files into an in-memory buffer for processing
 */
extern int git_futils_readbuffer(git_buf *obj, const char *path);
extern int git_futils_readbuffer_updated(
	git_buf *obj, const char *path, time_t *mtime, size_t *size, int *updated);
extern int git_futils_readbuffer_fd(git_buf *obj, git_file fd, size_t len);

extern int git_futils_writebuffer(
	const git_buf *buf, const char *path, int open_flags, mode_t mode);

/**
 * File utils
 *
 * These are custom filesystem-related helper methods. They are
 * rather high level, and wrap the underlying POSIX methods
 *
 * All these methods return 0 on success,
 * or an error code on failure and an error message is set.
 */

/**
 * Create and open a file, while also
 * creating all the folders in its path
 */
extern int git_futils_creat_withpath(const char *path, const mode_t dirmode, const mode_t mode);

/**
 * Create an open a process-locked file
 */
extern int git_futils_creat_locked(const char *path, const mode_t mode);

/**
 * Create an open a process-locked file, while
 * also creating all the folders in its path
 */
extern int git_futils_creat_locked_withpath(const char *path, const mode_t dirmode, const mode_t mode);

/**
 * Create a path recursively
 *
 * If a base parameter is being passed, it's expected to be valued with a
 * path pointing to an already existing directory.
 */
extern int git_futils_mkdir_r(const char *path, const char *base, const mode_t mode);

/**
 * Flags to pass to `git_futils_mkdir`.
 *
 * * GIT_MKDIR_EXCL is "exclusive" - i.e. generate an error if dir exists.
 * * GIT_MKDIR_PATH says to make all components in the path.
 * * GIT_MKDIR_CHMOD says to chmod the final directory entry after creation
 * * GIT_MKDIR_CHMOD_PATH says to chmod each directory component in the path
 * * GIT_MKDIR_SKIP_LAST says to leave off the last element of the path
 * * GIT_MKDIR_SKIP_LAST2 says to leave off the last 2 elements of the path
 * * GIT_MKDIR_VERIFY_DIR says confirm final item is a dir, not just EEXIST
 *
 * Note that the chmod options will be executed even if the directory already
 * exists, unless GIT_MKDIR_EXCL is given.
 */
typedef enum {
	GIT_MKDIR_EXCL = 1,
	GIT_MKDIR_PATH = 2,
	GIT_MKDIR_CHMOD = 4,
	GIT_MKDIR_CHMOD_PATH = 8,
	GIT_MKDIR_SKIP_LAST = 16,
	GIT_MKDIR_SKIP_LAST2 = 32,
	GIT_MKDIR_VERIFY_DIR = 64,
} git_futils_mkdir_flags;

/**
 * Create a directory or entire path.
 *
 * This makes a directory (and the entire path leading up to it if requested),
 * and optionally chmods the directory immediately after (or each part of the
 * path if requested).
 *
 * @param path The path to create.
 * @param base Root for relative path.  These directories will never be made.
 * @param mode The mode to use for created directories.
 * @param flags Combination of the mkdir flags above.
 * @return 0 on success, else error code
 */
extern int git_futils_mkdir(const char *path, const char *base, mode_t mode, uint32_t flags);

/**
 * Create all the folders required to contain
 * the full path of a file
 */
extern int git_futils_mkpath2file(const char *path, const mode_t mode);

/**
 * Flags to pass to `git_futils_rmdir_r`.
 *
 * * GIT_RMDIR_EMPTY_HIERARCHY - the default; remove hierarchy of empty
 *       dirs and generate error if any files are found.
 * * GIT_RMDIR_REMOVE_FILES    - attempt to remove files in the hierarchy.
 * * GIT_RMDIR_SKIP_NONEMPTY   - skip non-empty directories with no error.
 * * GIT_RMDIR_EMPTY_PARENTS   - remove containing directories up to base
 *       if removing this item leaves them empty
 * * GIT_RMDIR_REMOVE_BLOCKERS - remove blocking file that causes ENOTDIR
 * * GIT_RMDIR_SKIP_ROOT       - don't remove root directory itself
 */
typedef enum {
	GIT_RMDIR_EMPTY_HIERARCHY = 0,
	GIT_RMDIR_REMOVE_FILES    = (1 << 0),
	GIT_RMDIR_SKIP_NONEMPTY   = (1 << 1),
	GIT_RMDIR_EMPTY_PARENTS   = (1 << 2),
	GIT_RMDIR_REMOVE_BLOCKERS = (1 << 3),
	GIT_RMDIR_SKIP_ROOT       = (1 << 4),
} git_futils_rmdir_flags;

/**
 * Remove path and any files and directories beneath it.
 *
 * @param path Path to the top level directory to process.
 * @param base Root for relative path.
 * @param flags Combination of git_futils_rmdir_flags values
 * @return 0 on success; -1 on error.
 */
extern int git_futils_rmdir_r(const char *path, const char *base, uint32_t flags);

/**
 * Create and open a temporary file with a `_git2_` suffix.
 * Writes the filename into path_out.
 * @return On success, an open file descriptor, else an error code < 0.
 */
extern int git_futils_mktmp(git_buf *path_out, const char *filename, mode_t mode);

/**
 * Move a file on the filesystem, create the
 * destination path if it doesn't exist
 */
extern int git_futils_mv_withpath(const char *from, const char *to, const mode_t dirmode);

/**
 * Copy a file
 *
 * The filemode will be used for the newly created file.
 */
extern int git_futils_cp(
	const char *from,
	const char *to,
	mode_t filemode);

/**
 * Flags that can be passed to `git_futils_cp_r`.
 *
 * - GIT_CPDIR_CREATE_EMPTY_DIRS: create directories even if there are no
 *   files under them (otherwise directories will only be created lazily
 *   when a file inside them is copied).
 * - GIT_CPDIR_COPY_SYMLINKS: copy symlinks, otherwise they are ignored.
 * - GIT_CPDIR_COPY_DOTFILES: copy files with leading '.', otherwise ignored.
 * - GIT_CPDIR_OVERWRITE: overwrite pre-existing files with source content,
 *   otherwise they are silently skipped.
 * - GIT_CPDIR_CHMOD_DIRS: explicitly chmod directories to `dirmode`
 * - GIT_CPDIR_SIMPLE_TO_MODE: default tries to replicate the mode of the
 *   source file to the target; with this flag, always use 0666 (or 0777 if
 *   source has exec bits set) for target.
 */
typedef enum {
	GIT_CPDIR_CREATE_EMPTY_DIRS = (1u << 0),
	GIT_CPDIR_COPY_SYMLINKS     = (1u << 1),
	GIT_CPDIR_COPY_DOTFILES     = (1u << 2),
	GIT_CPDIR_OVERWRITE         = (1u << 3),
	GIT_CPDIR_CHMOD_DIRS        = (1u << 4),
	GIT_CPDIR_SIMPLE_TO_MODE    = (1u << 5),
} git_futils_cpdir_flags;

/**
 * Copy a directory tree.
 *
 * This copies directories and files from one root to another.  You can
 * pass a combinationof GIT_CPDIR flags as defined above.
 *
 * If you pass the CHMOD flag, then the dirmode will be applied to all
 * directories that are created during the copy, overiding the natural
 * permissions.  If you do not pass the CHMOD flag, then the dirmode
 * will actually be copied from the source files and the `dirmode` arg
 * will be ignored.
 */
extern int git_futils_cp_r(
	const char *from,
	const char *to,
	uint32_t flags,
	mode_t dirmode);

/**
 * Open a file readonly and set error if needed.
 */
extern int git_futils_open_ro(const char *path);

/**
 * Get the filesize in bytes of a file
 */
extern git_off_t git_futils_filesize(git_file fd);

#define GIT_PERMS_IS_EXEC(MODE)		(((MODE) & 0111) != 0)
#define GIT_PERMS_CANONICAL(MODE)	(GIT_PERMS_IS_EXEC(MODE) ? 0755 : 0644)
#define GIT_PERMS_FOR_WRITE(MODE)   (GIT_PERMS_IS_EXEC(MODE) ? 0777 : 0666)

#define GIT_MODE_PERMS_MASK			0777
#define GIT_MODE_TYPE_MASK			0170000
#define GIT_MODE_TYPE(MODE)			((MODE) & GIT_MODE_TYPE_MASK)
#define GIT_MODE_ISBLOB(MODE)		(GIT_MODE_TYPE(MODE) == GIT_MODE_TYPE(GIT_FILEMODE_BLOB))

/**
 * Convert a mode_t from the OS to a legal git mode_t value.
 */
extern mode_t git_futils_canonical_mode(mode_t raw_mode);


/**
 * Read-only map all or part of a file into memory.
 * When possible this function should favor a virtual memory
 * style mapping over some form of malloc()+read(), as the
 * data access will be random and is not likely to touch the
 * majority of the region requested.
 *
 * @param out buffer to populate with the mapping information.
 * @param fd open descriptor to configure the mapping from.
 * @param begin first byte to map, this should be page aligned.
 * @param len number of bytes to map.
 * @return
 * - 0 on success;
 * - -1 on error.
 */
extern int git_futils_mmap_ro(
	git_map *out,
	git_file fd,
	git_off_t begin,
	size_t len);

/**
 * Read-only map an entire file.
 *
 * @param out buffer to populate with the mapping information.
 * @param path path to file to be opened.
 * @return
 * - 0 on success;
 * - GIT_ENOTFOUND if not found;
 * - -1 on an unspecified OS related error.
 */
extern int git_futils_mmap_ro_file(
	git_map *out,
	const char *path);

/**
 * Release the memory associated with a previous memory mapping.
 * @param map the mapping description previously configured.
 */
extern void git_futils_mmap_free(git_map *map);

/**
 * Find a "global" file (i.e. one in a user's home directory).
 *
 * @param path buffer to write the full path into
 * @param filename name of file to find in the home directory
 * @return 0 if found, GIT_ENOTFOUND if not found, or -1 on other OS error
 */
extern int git_futils_find_global_file(git_buf *path, const char *filename);

/**
 * Find an "XDG" file (i.e. one in user's XDG config path).
 *
 * @param path buffer to write the full path into
 * @param filename name of file to find in the home directory
 * @return 0 if found, GIT_ENOTFOUND if not found, or -1 on other OS error
 */
extern int git_futils_find_xdg_file(git_buf *path, const char *filename);

/**
 * Find a "system" file (i.e. one shared for all users of the system).
 *
 * @param path buffer to write the full path into
 * @param filename name of file to find in the home directory
 * @return 0 if found, GIT_ENOTFOUND if not found, or -1 on other OS error
 */
extern int git_futils_find_system_file(git_buf *path, const char *filename);

/**
 * Find template directory.
 *
 * @param path buffer to write the full path into
 * @return 0 if found, GIT_ENOTFOUND if not found, or -1 on other OS error
 */
extern int git_futils_find_template_dir(git_buf *path);

typedef enum {
	GIT_FUTILS_DIR_SYSTEM = 0,
	GIT_FUTILS_DIR_GLOBAL = 1,
	GIT_FUTILS_DIR_XDG    = 2,
	GIT_FUTILS_DIR_TEMPLATE = 3,
	GIT_FUTILS_DIR__MAX   = 4,
} git_futils_dir_t;

/**
 * Configures global data for configuration file search paths.
 *
 * @return 0 on success, <0 on failure
 */
extern int git_futils_dirs_global_init(void);

/**
 * Get the search path for global/system/xdg files
 *
 * @param out pointer to git_buf containing search path
 * @param which which list of paths to return
 * @return 0 on success, <0 on failure
 */
extern int git_futils_dirs_get(const git_buf **out, git_futils_dir_t which);

/**
 * Get search path into a preallocated buffer
 *
 * @param out String buffer to write into
 * @param outlen Size of string buffer
 * @param which Which search path to return
 * @return 0 on success, GIT_EBUFS if out is too small, <0 on other failure
 */

extern int git_futils_dirs_get_str(
	char *out, size_t outlen, git_futils_dir_t which);

/**
 * Set search paths for global/system/xdg files
 *
 * The first occurrence of the magic string "$PATH" in the new value will
 * be replaced with the old value of the search path.
 *
 * @param which Which search path to modify
 * @param paths New search path (separated by GIT_PATH_LIST_SEPARATOR)
 * @return 0 on success, <0 on failure (allocation error)
 */
extern int git_futils_dirs_set(git_futils_dir_t which, const char *paths);

/**
 * Create a "fake" symlink (text file containing the target path).
 *
 * @param new symlink file to be created
 * @param old original symlink target
 * @return 0 on success, -1 on error
 */
extern int git_futils_fake_symlink(const char *new, const char *old);

/**
 * A file stamp represents a snapshot of information about a file that can
 * be used to test if the file changes.  This portable implementation is
 * based on stat data about that file, but it is possible that OS specific
 * versions could be implemented in the future.
 */
typedef struct {
	git_time_t mtime;
	git_off_t  size;
	unsigned int ino;
} git_futils_filestamp;

/**
 * Compare stat information for file with reference info.
 *
 * This function updates the file stamp to current data for the given path
 * and returns 0 if the file is up-to-date relative to the prior setting or
 * 1 if the file has been changed.  (This also may return GIT_ENOTFOUND if
 * the file doesn't exist.)
 *
 * @param stamp File stamp to be checked
 * @param path Path to stat and check if changed
 * @return 0 if up-to-date, 1 if out-of-date, <0 on error
 */
extern int git_futils_filestamp_check(
	git_futils_filestamp *stamp, const char *path);

/**
 * Set or reset file stamp data
 *
 * This writes the target file stamp.  If the source is NULL, this will set
 * the target stamp to values that will definitely be out of date.  If the
 * source is not NULL, this copies the source values to the target.
 *
 * @param tgt File stamp to write to
 * @param src File stamp to copy from or NULL to clear the target
 */
extern void git_futils_filestamp_set(
	git_futils_filestamp *tgt, const git_futils_filestamp *src);

/**
 * Free the configuration file search paths.
 */
extern void git_futils_dirs_global_shutdown(void);

#endif /* INCLUDE_fileops_h__ */
