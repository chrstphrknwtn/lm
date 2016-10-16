#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	// The following line expects the socket path to be first argument
	char *mysocketpath = argv[1];
	struct sockaddr_un namesock;
	int fd;
	namesock.sun_family = AF_UNIX;
	strncpy(namesock.sun_path, (char *)mysocketpath, sizeof(namesock.sun_path));
	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	bind(fd, (struct sockaddr *) &namesock, sizeof(struct sockaddr_un));
	close(fd);
	return 0;
}
