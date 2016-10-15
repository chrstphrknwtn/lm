#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/** Helpers
----------------------------------------------------------------------------- */

/** Format symlink with arrow and path
----------------------------------------------- */
char *format_symlink (const char *filename)
{
	static char symlink_str[PATH_MAX];

	char *symlink_path;

	symlink_path = realpath(filename, NULL);
	sprintf(symlink_str, "→ \033[38;5;236m%s\033[0m", symlink_path);

	return symlink_str;
}

/** Wrap filename in ansi colors
----------------------------------------------- */
char *color_filename (const char *filename, const mode_t st_mode)
{
	static char filename_str[PATH_MAX];

	// TODO: dir writable by GRP & OTH
	// dir with user sticky bit
	if (st_mode & (S_ISVTX)) {
		sprintf(filename_str, "\033[42;30m%s\033[0m", filename);
		// dir
	} else if (S_ISDIR(st_mode)) {
		sprintf(filename_str, "\033[34m%s\033[0m", filename);
		// symlink
	} else if (S_ISLNK(st_mode)) {
		sprintf(filename_str, "\033[35m%s\033[0m", filename);
		// executable with set-user-id
	} else if (st_mode & (S_ISUID)) {
		sprintf(filename_str, "\033[41;30m%s\033[0m", filename);
		// executable or writable by group
	} else if (st_mode & (S_IXUSR | S_IXGRP | S_IXOTH | S_IWGRP)) {
		sprintf(filename_str, "\033[31m%s\033[0m", filename);
		// regular file
	} else {
		sprintf(filename_str, "\033[39m%s\033[0m", filename);
	}

	return filename_str;
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
		return 1;
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
	int dir_contents_length = collect_dir_contents(".", &dir_contents);
	int exists;

	printf("\n");

	for (int i = 0; i < dir_contents_length; i++) {
		exists = lstat(dir_contents[i], &buf);

		char *filename = color_filename(dir_contents[i], buf.st_mode);

		if (exists >= 0) {
			printf("\033[38;5;236m%o\033[0m %s %s\n", \
			       // File mode short format
			       buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO), \
			       // Filename
			       filename, \
			       // Symlink target
			       (S_ISLNK(buf.st_mode)) ? format_symlink(dir_contents[i]) : "");
		}
	}

	return 0;
}
