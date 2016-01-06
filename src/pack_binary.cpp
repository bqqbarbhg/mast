#include "json_write.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int success = 1;

	size_t json_size = 2048 * 2048;
	char *json_buffer = (char*)malloc(json_size);
	Json_Writer json_writer, *j = &json_writer;
	json_init(j, json_buffer, json_size);

	json_begin_object(j);

	FILE *out = fopen(argv[2], "wb");

	size_t offset = 0;

	char filename[1024];
	while (fgets(filename, sizeof(filename), stdin)) {
		trimr(filename);

		FILE *in = fopen(filename, "rb");
		if (!in) {
			fprintf(stderr, "Failed to open '%s', skipping...\n", filename);
			success = 0;
			continue;
		}

		size_t total_size = 0;
		char buffer[2048];
		size_t num_read;
		while (num_read = fread(buffer, 1, sizeof(buffer), in)) {
			fwrite(buffer, 1, num_read, out);
			total_size += num_read;
		}

		const char *name = skip_prefix(filename, argv[3]);
		json_key_begin_object(j, name);

		json_key_int(j, "offset", (int)offset);
		json_key_int(j, "size", (int)total_size);

		json_end_object(j);
		
		offset += total_size;
	}

	json_end_object(j);

	FILE *json_file = fopen(argv[1], "w");
	fwrite(json_buffer, json_length(j), 1, json_file);
	fclose(json_file);

	return success ? 0 : 1;
}


