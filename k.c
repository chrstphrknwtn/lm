#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

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
      printf("%ld %s\n", buf.st_atime, dir->d_name);
      numfiles += buf.st_size;
    }
  }

  closedir(d);

  return 0;
}
