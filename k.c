#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include <stdlib.h>
#include <string.h>


/* Helpers ------------------------------------------------------------------ */

/* Format the filemode / permissions ---------- */
char *format_filemode (const mode_t st_mode)
{
  static char filemode_str[11];

  char *filetype;
  char *mode2  = (st_mode & S_IRUSR) ? "r" : "-";
  char *mode3  = (st_mode & S_IWUSR) ? "w" : "-";
  char *mode4  = (st_mode & S_IXUSR) ? "x" : "-";
  char *mode5  = (st_mode & S_IRGRP) ? "r" : "-";
  char *mode6  = (st_mode & S_IWGRP) ? "w" : "-";
  char *mode7  = (st_mode & S_IXGRP) ? "x" : "-";
  char *mode8  = (st_mode & S_IROTH) ? "r" : "-";
  char *mode9  = (st_mode & S_IWOTH) ? "w" : "-";
  char *mode10 = (st_mode & S_IXOTH) ? "x" : "-";

  if (S_ISDIR(st_mode)) {
    filetype = "d";
  } else if (S_ISLNK(st_mode)) {
    filetype = "l";
  } else {
    filetype = "-";
  }

  sprintf(filemode_str, "%s%s%s%s%s%s%s%s%s%s", \
    filetype, mode2, mode3, mode4, mode5, mode6, mode7, mode8, mode9, mode10);

  char *filemode = filemode_str;
  return filemode;
}

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
  int numfiles;
  int exists;

  d = opendir(".");
  if (d == NULL) {
    perror("prsize");
    return 1;
  }

  numfiles = 0;

  while ((dir = readdir(d)) != NULL)
  {
    exists = lstat(dir->d_name, &buf);
    if (exists < 0) {
      fprintf(stderr, "Couldn't find %s\n", dir->d_name);
    } else {

      // User and group names
      struct passwd *pw = getpwuid(buf.st_uid);
      struct group  *gr = getgrgid(buf.st_gid);

      char *date_output = format_date(buf.st_mtime);
      char *filemode_output = format_filemode(buf.st_mode);
      char *symlink_output = (S_ISLNK(buf.st_mode)) ? format_symlink(dir->d_name) : "";

      printf("%s  %u %s %s  %lld %s %s %s\n", \
        filemode_output, \
        buf.st_nlink, \
        pw->pw_name, \
        gr->gr_name, \
        buf.st_size, \
        date_output, \
        dir->d_name, \
        symlink_output);

      numfiles += buf.st_size;
    }
  }

  closedir(d);

  return 0;
}
