// SPDX-License-Identifier: BSD-3-Clause

#include <pthread.h>
#include "ring_buffer.h"
#include "stdlib.h"



int ring_buffer_init(so_ring_buffer_t *rb, size_t cap)
{
	if (!rb || cap == 0)
		return -1;

	rb->data = malloc(cap);

	rb->read_pos = 0;
	rb->write_pos = 0;
	rb->len = 0;
	rb->cap = cap;
	rb->is_closed = 0;
	rb->num_threads = 0;

	pthread_mutex_init(&rb->mutex, NULL);
	pthread_cond_init(&rb->cond_non_empty, NULL);
	pthread_cond_init(&rb->cond_non_full, NULL);

	return 0;
}


ssize_t ring_buffer_enqueue(so_ring_buffer_t *rb, void *data, size_t size)
{
	if (!rb || !data || size == 0)
		return -1;

	pthread_mutex_lock(&rb->mutex);


	while (rb->len + size > rb->cap && !rb->is_closed)
		pthread_cond_wait(&rb->cond_non_full, &rb->mutex);

	size_t bytes_to_write = size;
	size_t space_left = rb->cap - rb->write_pos;
	size_t chunk_size = bytes_to_write < space_left ? bytes_to_write : space_left;

	memcpy(rb->data + rb->write_pos, data, chunk_size);
	rb->write_pos = (rb->write_pos + chunk_size) % rb->cap;
	rb->len += chunk_size;
	bytes_to_write -= chunk_size;
	data = (char *)data + chunk_size;

	pthread_cond_signal(&rb->cond_non_empty);
	pthread_mutex_unlock(&rb->mutex);

	return size;
}


ssize_t ring_buffer_dequeue(so_ring_buffer_t *rb, void *data, size_t size)
{
	if (!rb || !data || size == 0)
		return -1;

	pthread_mutex_lock(&rb->mutex);

	while (rb->len < size && !rb->is_closed)
		pthread_cond_wait(&rb->cond_non_empty, &rb->mutex);

	if (rb->is_closed && rb->len == 0) {
		pthread_mutex_unlock(&rb->mutex);
		return 0;
	}

	size_t bytes_to_read = size;
	size_t space_left = rb->cap - rb->read_pos;
	size_t chunk_size = bytes_to_read < space_left ? bytes_to_read : space_left;

	memcpy(data, rb->data + rb->read_pos, chunk_size);
	rb->read_pos = (rb->read_pos + chunk_size) % rb->cap;
	rb->len -= chunk_size;
	bytes_to_read -= chunk_size;
	data = (char *)data + chunk_size;

	pthread_cond_signal(&rb->cond_non_full);
	pthread_mutex_unlock(&rb->mutex);

	return size;
}

void ring_buffer_destroy(so_ring_buffer_t *rb)
{
	pthread_mutex_destroy(&rb->mutex);
	pthread_cond_destroy(&rb->cond_non_empty);
	pthread_cond_destroy(&rb->cond_non_full);

	free(rb->data);
	rb->data = NULL;
	rb->cap = rb->len = rb->read_pos = rb->write_pos = 0;
}


void ring_buffer_stop(so_ring_buffer_t *rb)
{
	pthread_mutex_lock(&rb->mutex);
	rb->is_closed = 1;
	pthread_mutex_unlock(&rb->mutex);
}

