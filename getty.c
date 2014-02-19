#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "util/util.h"

char *argv0;

void opentty(const char *tty)
{
	close(0);
	if (open(tty, O_RDWR|O_NOCTTY, 0) != 0)
		die("cannot open %s\n", tty);
	if (fchown(0, 0, 0) == -1 || fchmod(0, 0600) == -1)
		die("cannot change ownership or mode of tty\n");
	if (ioctl(0, TIOCSCTTY, 1) == -1)
		die("cannot set controlling tty\n");
	close(1);
	close(2);
	if (dup(0) != 1 || dup(0) != 2)
		die("cannot setup stdout or stderr\n");

	write(0, "\033c", 2);
}

void usage(void)
{
	die("usage: %s tty\n", argv0);
}

int main(int argc, char *argv[])
{
	char *arg[] = { "/bin/login", NULL };
	char *env[] = { "TERM=linux", NULL };

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if (argc != 1)
		usage();
	else
		opentty(argv[0]);

	tcflush(0, TCIOFLUSH);
	execve(arg[0], arg, env);
	warn("cannot execute %s\n", arg[0]);
	return 1;
}
