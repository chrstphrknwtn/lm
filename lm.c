#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

/** Helpers
----------------------------------------------------------------------------- */

/** Format short mode
----------------------------------------------- */
char *format_shortmode (const mode_t st_mode)
{
	static char shortmode_str[25];

	sprintf(shortmode_str, "\033[38;5;236m%o\033[0m",
	        st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

	return shortmode_str;
}

/** Format filenames with default macOS ls colors
----------------------------------------------- */
char *format_filename (const char *filename, const mode_t m)
{
	static char filename_str[PATH_MAX];
	int is_executable = ((m & S_IXUSR) | (m & S_IXGRP) | (m & S_IXOTH));

	// dir writable by others
	if (S_ISDIR(m) && (m & S_IWOTH)) {
		sprintf(filename_str, "\033[43;30m%s\033[0m", filename);
		// set-user-id on execution
	} else if ((m & S_ISUID) && is_executable) {
		sprintf(filename_str, "\033[41;30m%s\033[0m", filename);
		// set-group-id on execution
	} else if ((m & S_ISGID) && is_executable) {
		sprintf(filename_str, "\033[46;30m%s\033[0m", filename);
		// generally executable
	} else if (S_ISREG(m) && is_executable) {
		sprintf(filename_str, "\033[31m%s\033[0m", filename);
		// block device
	} else if (S_ISBLK(m)) {
		sprintf(filename_str, "\033[46;34m%s\033[0m", filename);
		// character device
	} else if (S_ISCHR(m)) {
		sprintf(filename_str, "\033[43;34m%s\033[0m", filename);
		// regular dir
	} else if (S_ISDIR(m)) {
		sprintf(filename_str, "\033[34m%s\033[0m", filename);
		// pipe / FIFO
	} else if (S_ISFIFO(m)) {
		sprintf(filename_str, "\033[33m%s\033[0m", filename);
		// socket
	} else if (S_ISSOCK(m)) {
		sprintf(filename_str, "\033[32m%s\033[0m", filename);
		// symlink
	} else if (S_ISLNK(m)) {
		sprintf(filename_str, "\033[35m%s\033[0m", filename);
		// Regular file
	} else {
		sprintf(filename_str, "\033[39m%s\033[0m", filename);
	}

	return filename_str;
}

/** Format symlink with arrow and path
----------------------------------------------- */
char *format_symlink (const char *filename,
                      const mode_t st_mode,
                      const size_t st_size)
{
	static char symlink_str[PATH_MAX];

	if (S_ISLNK(st_mode)) {
		char *symlink_path;
		ssize_t r;

		symlink_path = malloc(st_size + 1);

		r = readlink(filename, symlink_path, st_size + 1);

		if (r < 0) {
			perror("readlink");
			exit(EXIT_FAILURE);
		}

		symlink_path[st_size] = '\0';
		sprintf(symlink_str, "→ \033[38;5;236m%s\033[0m", symlink_path);
	} else {
		sprintf(symlink_str, "");
	}
	return symlink_str;
}

/** Readdir
----------------------------------------------------------------------------- */
char collect_dir_contents (const char *path, char ***contents)
{
	size_t count = 0;
	DIR *directory;
	struct dirent *current_directory_item = NULL;

	directory = opendir(path);

	if (directory == NULL) {
		perror("prsize");
		exit(EXIT_FAILURE);
	}

	// Find out how many items are in the directory
	while ((current_directory_item = readdir(directory)) != NULL) {
		count++;
	}

	// Reset the pointer to the directory
	rewinddir(directory);

	// Allocate the right ammount of space to the contents array
	*contents = NULL;
	*contents = calloc(count, sizeof(char *));

	// Loop through the directory again adding each item to the contents array
	count = 0;
	while ((current_directory_item = readdir(directory)) != NULL) {
		char *name = strdup(current_directory_item->d_name);
		// Reject . and ..
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
		} else {
			(*contents)[count++] = name;
		}
	}

	closedir(directory);

	return count;
}


/** Main
----------------------------------------------------------------------------- */
int main (int argc, char *argv[])
{
	struct stat buf;
	char **dir_contents;
	char *filename;
	int dir_contents_length = collect_dir_contents(".", &dir_contents);
	int exists;

	printf("\n");

	for (int i = 0; i < dir_contents_length; i++) {
		filename = dir_contents[i];
		exists = lstat(filename, &buf);

		if (exists >= 0) {
			printf("%s %s %s\n",
			       format_shortmode(buf.st_mode),
			       format_filename(filename, buf.st_mode),
			       format_symlink(filename, buf.st_mode, buf.st_size));
		}
	}

	return 0;
}
