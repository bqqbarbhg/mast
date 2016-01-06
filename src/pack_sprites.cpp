#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_rect_pack.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "json_write.h"
#include "util.h"

static char str_buffer[1024 * 1024], *str_buffer_head = str_buffer;

struct Image
{
	const char *name;
	unsigned char *data;
	int width, height;
};

char *push_str(char *str)
{
	char *ptr = str_buffer_head;
	size_t len = strlen(str);
	memcpy(ptr, str, len + 1);
	str_buffer_head += len + 1;
	return ptr;
}

int main(int argc, char **argv)
{
	static stbrp_rect rects[4096];
	static Image images[4096];

	int success = 1;

	char buffer[1024];
	int count = 0;
	while (fgets(buffer, sizeof(buffer), stdin)) {
		trimr(buffer);

		char *filename = push_str(buffer);

		int width, height, channels;
		unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
		if (channels != 4) {
			fprintf(stderr, "Wrong number of components in file '%s', skipping...\n", filename);
			success = 0;
			continue;
		}

		rects[count].id = count;
		rects[count].w = (stbrp_coord)width;
		rects[count].h = (stbrp_coord)height;

		images[count].name = filename;
		images[count].data = data;
		images[count].width = width;
		images[count].height = height;
		count++;
	}

	int atlas_size = 1024;

	stbrp_node *nodes = (stbrp_node*)malloc(sizeof(stbrp_node) * atlas_size);
	stbrp_context rp_ctx;
	stbrp_init_target(&rp_ctx, atlas_size, atlas_size, nodes, atlas_size);

	stbrp_pack_rects(&rp_ctx, rects, count);

	unsigned char *data = (unsigned char*)calloc(atlas_size * atlas_size, 4);

	for (int i = 0; i < count; i++) {
		Image *image = images + rects[i].id;
		if (!rects[i].was_packed) {
			fprintf(stderr, "Could not pack '%s', skipping...\n", image->name);
			success = 0;
			continue;
		}

		int bx = rects[i].x;
		int by = rects[i].y;
		int w = image->width;
		int h = image->height;

		unsigned char *src = images[i].data;

		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				int si = (y * w + x) * 4;
				int di = ((by + y) * atlas_size + x + bx) * 4;

				uint8_t alpha = src[si + 3];
				data[di + 0] = (uint8_t)(src[si + 0] * alpha / 256);
				data[di + 1] = (uint8_t)(src[si + 1] * alpha / 256);
				data[di + 2] = (uint8_t)(src[si + 2] * alpha / 256);
				data[di + 3] = alpha;
			}
		}
	}

	free(nodes);

	size_t json_size = 2048 * 2048;
	char *json_buffer = (char*)malloc(json_size);
	Json_Writer json_writer, *j = &json_writer;
	json_init(j, json_buffer, json_size);

	json_begin_object(j);

	for (int i = 0; i < count; i++) {
		Image *image = images + rects[i].id;
		if (!rects[i].was_packed)
			continue;

		const char *name = skip_prefix(image->name, argv[3]);

		json_key_begin_object(j, name);

		json_key_int(j, "x", rects[i].x);
		json_key_int(j, "y", rects[i].y);
		json_key_int(j, "w", rects[i].w);
		json_key_int(j, "h", rects[i].h);

		json_end_object(j);
	}

	json_end_object(j);

	FILE *json_file = fopen(argv[1], "w");
	fwrite(json_buffer, json_length(j), 1, json_file);
	fclose(json_file);

	stbi_write_png(argv[2], atlas_size, atlas_size, 4, data, atlas_size * 4);

	return success ? 0 : 1;
}

