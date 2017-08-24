#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char buf[256];
	int fd, i, n;
	short revents;
	struct pollfd pfd;

	/* Open /dev/char_device */
	fd = open("/dev/char_device", O_RDONLY);

	/* If file can not be opened, throw error */
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Set poll fd and events */
	pfd.fd = fd;
	pfd.events = POLLIN;

	while (1) {
		i = poll(&pfd, 1, -1);

		if (i == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}

		revents = pfd.revents;

		/* If there is anything to read (POLLIN event), read the buffer and print */
		if (revents & POLLIN) {
			n = read(pfd.fd, buf, sizeof(buf));
			printf("POLLIN n=%d buf=%.*s\n", n, n, buf);
		}
	}
}
