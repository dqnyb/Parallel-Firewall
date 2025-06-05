/* SPDX-License-Identifier: BSD-3-Clause */

#include <pthread.h>
#include <semaphore.h>
#include "ring_buffer.h"
#include "packet.h"


#ifndef __SO_CONSUMER_H__
#define __SO_CONSUMER_H__

typedef struct so_consumer_ctx_t {
    struct so_ring_buffer_t *producer_rb;

    pthread_mutex_t log_mutex;
    pthread_barrier_t expected_log;
    const char *log_filename;
    int log_index, threads;
} so_consumer_ctx_t;

int create_consumers(pthread_t *tids,
                     int num_consumers,
                     so_ring_buffer_t *rb,
                     const char *out_filename);

#endif /* __SO_CONSUMER_H__ */
