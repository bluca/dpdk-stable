/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright 2017 Cavium, Inc.
 */

#ifndef _TEST_PIPELINE_COMMON_
#define _TEST_PIPELINE_COMMON_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <rte_cycles.h>
#include <rte_ethdev.h>
#include <rte_eventdev.h>
#include <rte_event_eth_rx_adapter.h>
#include <rte_lcore.h>
#include <rte_malloc.h>
#include <rte_mempool.h>
#include <rte_prefetch.h>
#include <rte_spinlock.h>
#include <rte_service.h>
#include <rte_service_component.h>

#include "evt_common.h"
#include "evt_options.h"
#include "evt_test.h"

struct test_pipeline;

struct worker_data {
	uint64_t processed_pkts;
	uint8_t dev_id;
	uint8_t port_id;
	struct test_pipeline *t;
} __rte_cache_aligned;

struct tx_service_data {
	uint8_t dev_id;
	uint8_t queue_id;
	uint8_t port_id;
	uint32_t service_id;
	uint64_t processed_pkts;
	uint16_t nb_ethports;
	struct rte_eth_dev_tx_buffer *tx_buf[RTE_MAX_ETHPORTS];
	struct test_pipeline *t;
} __rte_cache_aligned;

struct test_pipeline {
	/* Don't change the offset of "done". Signal handler use this memory
	 * to terminate all lcores work.
	 */
	int done;
	uint8_t nb_workers;
	uint8_t mt_unsafe;
	enum evt_test_result result;
	uint32_t nb_flows;
	uint64_t outstand_pkts;
	struct rte_mempool *pool;
	struct worker_data worker[EVT_MAX_PORTS];
	struct tx_service_data tx_service;
	struct evt_options *opt;
	uint8_t sched_type_list[EVT_MAX_STAGES] __rte_cache_aligned;
} __rte_cache_aligned;

#define BURST_SIZE 16

static inline int
pipeline_nb_event_ports(struct evt_options *opt)
{
	return evt_nr_active_lcores(opt->wlcores);
}

int pipeline_test_result(struct evt_test *test, struct evt_options *opt);
int pipeline_opt_check(struct evt_options *opt, uint64_t nb_queues);
int pipeline_test_setup(struct evt_test *test, struct evt_options *opt);
int pipeline_ethdev_setup(struct evt_test *test, struct evt_options *opt);
int pipeline_event_rx_adapter_setup(struct evt_options *opt, uint8_t stride,
		struct rte_event_port_conf prod_conf);
int pipeline_event_tx_service_setup(struct evt_test *test,
		struct evt_options *opt, uint8_t tx_queue_id,
		uint8_t tx_port_id, const struct rte_event_port_conf p_conf);
int pipeline_mempool_setup(struct evt_test *test, struct evt_options *opt);
int pipeline_event_port_setup(struct evt_test *test, struct evt_options *opt,
		uint8_t *queue_arr, uint8_t nb_queues,
		const struct rte_event_port_conf p_conf);
int pipeline_launch_lcores(struct evt_test *test, struct evt_options *opt,
		int (*worker)(void *));
void pipeline_opt_dump(struct evt_options *opt, uint8_t nb_queues);
void pipeline_test_destroy(struct evt_test *test, struct evt_options *opt);
void pipeline_eventdev_destroy(struct evt_test *test, struct evt_options *opt);
void pipeline_ethdev_destroy(struct evt_test *test, struct evt_options *opt);
void pipeline_mempool_destroy(struct evt_test *test, struct evt_options *opt);

#endif /* _TEST_PIPELINE_COMMON_ */
