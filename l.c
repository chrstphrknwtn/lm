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


/* Main --------------------------------------------------------------------- */

int main (int argc, char *argv[])
{
  DIR *d;
  struct dirent *dir;
  struct stat buf;
  int exists;

  d = opendir(".");
  if (d == NULL) {
    perror("prsize");
    return 1;
  }

  printf("\n");
  while ((dir = readdir(d)) != NULL)
  {
    exists = lstat(dir->d_name, &buf);
    if (exists < 0) {
      fprintf(stderr, "Couldn't find %s\n", dir->d_name);
    } else {

      char *date_output = format_date(buf.st_mtime);
      char *symlink_output = (S_ISLNK(buf.st_mode)) ? format_symlink(dir->d_name) : "";

      printf("\033[38;5;241m%s\033[0m  %s %s\n", \
        date_output, \
        dir->d_name, \
        symlink_output);
    }
  }

  closedir(d);

  return 0;
}
