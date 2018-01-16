/**
 * copy from: https://bugzilla.redhat.com/show_bug.cgi?id=185056
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#define NLOOPS 10000

int err(const char *msg, const char *a1, const char *a2)
{
	int e = errno;

	perror(msg);

	fprintf(stderr, "errno: %d\n", e);
	if (a1)
		fprintf(stderr, "arg1: \"%s\"\n", a1);
	if (a2)
		fprintf(stderr, "arg2: \"%s\"\n", a2);

	return -1;
}

int main(int argc, char *argv[])
{
	int i, cnt = 1, loop_cnt = NLOOPS;
	char f1[100], f2[100];
	int fd, ret;

	if (argc == 2)
		cnt = strtol(argv[1], NULL, 10);

	loop_cnt *= cnt;

	sprintf(f1, "f%d_%d", cnt, 0);
	fd = creat(f1, S_IRUSR);
	if (fd < 0)
		return err("create failed", f1, 0);
	else
		printf("WHACK: create file '%s'\n", f1);

	ret = close(fd);
	if (fd < 0)
		return err("close failed", f1, 0);

	printf("WHACK: rename %d times from foo#i to foo#(i+1)...\n", loop_cnt);
	for (i = 0; i < loop_cnt; i++) {
		sprintf(f1, "f%d_%d", cnt, i);
		sprintf(f2, "f%d_%d", cnt, i+1);
		if (rename(f1, f2) < 0) {
			err("rename failed", f1, f2);
			printf("WHACK: try to rename back immediately...\n");
			if(rename(f2, f1) < 0)
				err("rename failed", f2, f1);
			return -1;
		}
	}

	ret = unlink(f2);
	if (fd < 0)
		return err("unlink failed", f2, 0);

	printf("WHACK: done w/o errors, test file removed\n");
	return ret;
}
