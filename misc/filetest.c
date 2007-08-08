#include <stdio.h>

#include "tiger.h"
#include "tigertree.h"
#include "base32.h"

int main(int argc, char *argv[]) {
	TT_CONTEXT ctx;
	FILE *fd;
	unsigned char inbuf[BLOCKSIZE];
	size_t read;
	unsigned char hash[TIGERSIZE];
	char res[BASE32_DESTLEN(TIGERSIZE)];
	
	if (argc != 2) {
		fprintf(stderr, "Usage: filetest FILE\n");
		return 1;
	}

	printf ("will open: %s\n", argv[1]);
	fd = fopen(argv[1], "r");
	if (fd == NULL) {
		perror("Failed to open file: ");
		return 2;
	}

	tt_init(&ctx);

	while (1) {
		read = fread(inbuf, 1, BLOCKSIZE, fd);
		if (read == 0 && feof(fd)) break;

		tt_update(&ctx, inbuf, read);
	};

	fclose(fd);

	tt_digest(&ctx, hash);

	/* base32 */

	to_base32(hash, TIGERSIZE, res);
	printf("%s\n", res);

	return 0;
}

