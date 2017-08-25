#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define device	"/dev/char_device"

int main(int argc, char **argv) {
	char buffer[256];
	int file_desc, i, n;
	short revents;
	struct pollfd poll_fd;

	/* Open '/dev/char_device' */
	file_desc = open(device, O_RDONLY);

	/* If file can not be opened, throw error */
	if (file_desc == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Set poll fd and events */
	/* !!! events: requested events, revents: returned events !!!*/
	poll_fd.fd = file_desc;
	poll_fd.events = POLLIN;

	while (1) {
		printf("Poll going to sleep...\n");
		i = poll(&poll_fd, 1, -1); // (fd, # of fds, timeout)

		if (i == -1) {
			perror("error @ poll");
			exit(EXIT_FAILURE);
		}

		revents = poll_fd.revents;

		/* If there is anything to read (POLLIN event), read the buffer and print */
		if (revents & POLLIN) {
			printf("Poll woke up...\n");
			n = read(poll_fd.fd, buffer, sizeof(buffer));
			printf("POLLIN - Message = %.*s", n, buffer);
		}

	}

	return 0;
}
