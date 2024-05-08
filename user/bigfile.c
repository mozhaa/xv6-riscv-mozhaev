#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

typedef uint64 batch_t;
#define BATCHSIZE 4096
#define BATCHBYTES (BATCHSIZE * sizeof(batch_t))

uint state1 = 777;

static char rndchr() {
	state1 = state1 * 1664525 + 1013904223;
	return ((state1 >> 24) - 'a') % ('z' - 'a') + 'a';
}

batch_t state2 = 777;

static batch_t rndint() { return state2 = state2 * 1664525 + 1013904223; }

static void restart() { state2 = 777; }

static void rndstr(char* buf, int size) {
	for (int i = 0; i < size - 1; ++i)
		buf[i] = rndchr();
	buf[size - 1] = 0;
}

static int getfd(char* fn, int size) {
	int fd;
	while (1) {
		rndstr(fn, size);
		fd = open(fn, O_WRONLY);
		if (fd < 0)
			break;
		close(fd);
	}
	return open(fn, O_WRONLY | O_CREATE);
}

static void fill_batch(batch_t* batch, int bytes) {
    for (int i = 0; i < BATCHSIZE && (i + 1) * sizeof(batch_t) < bytes; ++i) {
        batch[i] = rndint();
    }
}

static int check_batch(batch_t* batch, int bytes) {
    for (int i = 0; i < BATCHSIZE && (i + 1) * sizeof(batch_t) < bytes; ++i) {
        batch_t rnd = rndint();
        if (batch[i] != rnd) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(2, "Usage: bigfile <fsize>\n");
		exit(-1);
	}

	int fsize = atoi(argv[1]);
	int ret = 0;

	char name[10];
	int fd = getfd(name, 10);
	fprintf(2, "Temporary file name: \"%s\"\n", name);
	
    batch_t* batch = malloc(BATCHSIZE * sizeof(batch_t));
    if (batch == 0) {
        fprintf(2, "Error during malloc()\n");
        ret = -1;
        goto end;
    }

    int prev_perc = 0;
    printf("Writing to file...\n");
	for (int i = fsize; i > 0; i -= BATCHBYTES) {
        int perc = (fsize - i) * 100 / fsize;
        if (perc - prev_perc >= 10) {
            prev_perc = perc;
            printf("%d%%\n", perc);
        }
		int bytes_to_write = (i < BATCHBYTES) ? i : BATCHBYTES;
		fill_batch(batch, bytes_to_write);
		if (write(fd, batch, bytes_to_write) != bytes_to_write) {
			fprintf(2, "Error during write()\n");
			ret = -1;
			goto end;
		}
	}

	close(fd);
    printf("Writing to file done\n");

	fd = open(name, O_RDONLY);
	if (fd < 0) {
		fprintf(2, "Error during open()\n");
		ret = -1;
		goto end_no_close;
	}
	restart();

    prev_perc = 0;
    printf("Reading from file\n");
	for (int i = fsize; i > 0; i -= BATCHBYTES) {
        int perc = (fsize - i) * 100 / fsize;
        if (perc - prev_perc >= 10) {
            prev_perc = perc;
            printf("%d%%\n", perc);
        }
		int bytes_to_read = (i < BATCHBYTES) ? i : BATCHBYTES;
		if (read(fd, batch, bytes_to_read) != bytes_to_read) {
			fprintf(2, "Error during read()\n");
			ret = -1;
			goto end;
		}
        if (!check_batch(batch, bytes_to_read)) {
            fprintf(2, "Test failed! Data is incorrect\n");
            ret = 1;
            goto end;
        }
	}
    printf("Reading from file done\n");

	printf("Test successed!\n");
end:
	close(fd);
end_no_close:
	unlink(name);
	free(batch);
	exit(ret);
}