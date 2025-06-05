// SPDX-License-Identifier: BSD-3-Clause

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

#include "consumer.h"
#include "ring_buffer.h"
#include "packet.h"
#include "utils.h"

struct packet_data {
	char data[256];
	unsigned long timestamp;
};


struct packet_data *packet_data_array;

int comp(const void *a, const void *b)
{
	return (*(struct packet_data *)a).timestamp - (*(struct packet_data *)b).timestamp;
}


void consumer_thread(so_consumer_ctx_t *ctx)
{
	so_packet_t packet;

	while (1) {
		if (ring_buffer_dequeue(ctx->producer_rb, &packet, sizeof(packet)) <= 0)
			break;

		so_action_t action = process_packet(&packet);
		unsigned long hash = packet_hash(&packet);
		unsigned long timestamp = packet.hdr.timestamp;

		pthread_mutex_lock(&ctx->log_mutex);
		snprintf(packet_data_array[ctx->log_index].data, 256, "%s %016lx %lu\n",
			RES_TO_STR(action), hash, timestamp);
		packet_data_array[ctx->log_index].timestamp = timestamp;
		ctx->log_index++;
		// FILE *log_file = fopen(ctx->log_filename, "a");
		// fprintf(log_file, "%s %016lx %lu\n", RES_TO_STR(action), hash, timestamp);
		// fclose(log_file);
		pthread_mutex_unlock(&ctx->log_mutex);

		pthread_barrier_wait(&ctx->expected_log);

		pthread_mutex_lock(&ctx->log_mutex);
		if (ctx->log_index == ctx->threads) {
			qsort(packet_data_array, ctx->threads, sizeof(struct packet_data), comp);
			for (int i = 0; i < ctx->log_index; i++) {
				FILE *log_file = fopen(ctx->log_filename, "a");

				fprintf(log_file, "%s", packet_data_array[i].data);
				fclose(log_file);
			}
			ctx->log_index = 0;
		}
		pthread_mutex_unlock(&ctx->log_mutex);
	}
}


int create_consumers(pthread_t *tids,
					 int num_consumers,
					 struct so_ring_buffer_t *rb,
					 const char *out_filename)
{
	so_consumer_ctx_t *ctx = malloc(sizeof(so_consumer_ctx_t));

	packet_data_array = malloc(num_consumers * sizeof(struct packet_data));

	ctx->log_index = 0;
	ctx->threads = num_consumers;
	ctx->producer_rb = rb;
	ctx->log_filename = out_filename;
	pthread_mutex_init(&ctx->log_mutex, NULL);
	pthread_barrier_init(&ctx->expected_log, NULL, num_consumers);

	for (int i = 0; i < num_consumers; i++)
		pthread_create(&tids[i], NULL, (void *)consumer_thread, ctx);

	return num_consumers;
}

