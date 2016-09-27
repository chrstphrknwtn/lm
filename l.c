#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


/* Helpers ------------------------------------------------------------------ */

/* Format dates ------------------------------- */
char *format_date (const time_t epoch)
{
  static char date_str[15];

  time_t t = epoch;
  struct tm *tm;
  tm = localtime(&t);
  strftime(date_str, sizeof(date_str), "%d %b %H:%M", tm);

  char *date = date_str;
  return date;
}

/* Format the symlink with the little '-> ' --- */
char *format_symlink (const char *filename)
{
  static char symlink_str[100];

  char *symlink_path;

  symlink_path = realpath(filename, NULL);
  sprintf(symlink_str, "-> %s", symlink_path);

  char *symlink = symlink_str;
  return symlink;
}

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


  /* Find out how many items are in the directory */
  while ((current_directory_item = readdir(directory)) != NULL)
  {
    count++;
  }


  /* Reset the pointer to the directory */
  rewinddir(directory);


  /* Allocate the right ammount of space to the contents array */
  *contents = NULL;
  *contents = calloc(count, sizeof(char *));


  /* Loop through the directory again adding each item to the contents array */
  count = 0;
  while ((current_directory_item = readdir(directory)) != NULL)
  {
    (*contents)[count++] = strdup(current_directory_item->d_name);
  }

  closedir(directory);

  return count;
}


/* Main --------------------------------------------------------------------- */

int main (int argc, char *argv[])
{
  struct stat buf;
  char **dir_contents;
  int exists;
  int dir_contents_length;
  int i;

  dir_contents_length = collect_dir_contents(".", &dir_contents);

  printf("\n");

  for (i = 0; i < dir_contents_length; i++)
  {
    exists = lstat(dir_contents[i], &buf);
    if (exists < 0) {
      fprintf(stderr, "Couldn't find %s\n", dir_contents[i]);
    } else {
      char *date_output = format_date(buf.st_mtime);
      char *symlink_output = (S_ISLNK(buf.st_mode)) ? format_symlink(dir_contents[i]) : "";

      printf("\033[38;5;235m%s\033[0m  %s %s\n", \
        date_output, \
        dir_contents[i], \
        symlink_output);
    }
  }

  return 0;
}
