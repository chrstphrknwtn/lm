#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char *argv[])
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
    exists = stat(dir->d_name, &buf);
    if (exists < 0) {
      fprintf(stderr, "Couldn't find %s\n", dir->d_name);
    } else {

      // Print Permissions
      printf( (S_ISDIR(buf.st_mode)) ? "d" : "-");
      printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
      printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
      printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
      printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
      printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
      printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
      printf( (buf.st_mode & S_IROTH) ? "r" : "-");
      printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
      printf( (buf.st_mode & S_IXOTH) ? "x" : "-");

      // Get User and group
      struct passwd *pw = getpwuid(buf.st_uid);
      struct group  *gr = getgrgid(buf.st_gid);

      // Format Modified Date
      time_t t = buf.st_mtime;
      struct tm *tm;
      char date_formatted[13];
      tm = localtime(&t);
      strftime(date_formatted, sizeof(date_formatted), "%d %b %H:%M", tm);

      printf("  %u %s %s  %lld %s %s\n", buf.st_nlink, pw->pw_name, gr->gr_name, buf.st_size, date_formatted, dir->d_name);

      numfiles += buf.st_size;
    }
  }

  closedir(d);

  return 0;
}
