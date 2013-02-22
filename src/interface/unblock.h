#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include <linux/rfkill.h>

#define RF_PATH "/dev/rfkill"

static void rfkill_block(__u32 idx, __u8 block) {
	struct rfkill_event event;
	ssize_t len;
	int fd;

	fd = open(RF_PATH, O_RDWR);
	if (fd < 0) {
		perror("Can't open RFKILL control device");
		return;
	}

	memset(&event, 0, sizeof(event));
	event.idx = idx;
	event.op = RFKILL_OP_CHANGE;
	event.soft = block;

	len = write(fd, &event, sizeof(event));
	if (len < 0)
		perror("Failed to change RFKILL state");

	close(fd);
}
