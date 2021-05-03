/* SPDX-License-Identifier: LGPL-3.0-or-later */

#ifndef COORDINATOOL_H
#define COORDINATOOL_H

// for urcu slight optimization
#define _LGPL_SOURCE

#include <errno.h>
#include <linux/lustre/lustre_idl.h>
#include <lustre/lustreapi.h>
#include <sys/socket.h>
#include <urcu/wfcqueue.h>

#include "logs.h"
#include "protocol.h"
#include "utils.h"


/* queue types */

struct hsm_action_node {
	struct cds_wfcq_node node;
	/* hsm_action_item is variable length and MUST be last */
	struct hsm_action_item hai;
};

#define ARCHIVE_ID_UNINIT ((unsigned int)-1)
struct hsm_action_queues {
	struct cds_wfcq_head restore_head;
	struct cds_wfcq_tail restore_tail;
	struct cds_wfcq_head archive_head;
	struct cds_wfcq_tail archive_tail;
	struct cds_wfcq_head remove_head;
	struct cds_wfcq_tail remove_tail;
	struct cds_wfcq_head running_head;
	struct cds_wfcq_tail running_tail;
	char *fsname;
	unsigned long long hal_flags;
	unsigned int archive_id;
};

/* common types */

struct state {
	// options
	const char *mntpath;
	int archive_cnt;
	int archive_id[LL_HSM_MAX_ARCHIVES_PER_AGENT];
	const char *host;
	const char *port;
	// states value
	struct hsm_copytool_private *ctdata;
	int epoll_fd;
	int hsm_fd;
	int listen_fd;
	struct hsm_action_queues queues;
	struct ct_stats stats;
};



/* lhsm */

int handle_ct_event(struct state *state);
int ct_register(struct state *state);
int ct_start(struct state *state);

/* protocol */
extern protocol_read_cb protocol_cbs[];

/**
 * send status reply
 *
 * @param fd socket to write on
 * @param ct_stats stats to get stats to send from
 * @param status error code
 * @param error nul-terminated error string, can be NULL
 * @return 0 on success, -errno on error
 */
int protocol_reply_status(int fd, struct ct_stats *ct_stats, int status, char *error);
int protocol_reply_recv(int fd, json_t *hal, int status, char *error);
int protocol_reply_queue(int fd, int enqueued, int status, char *error);


/* tcp */

int tcp_listen(struct state *state);
char *sockaddr2str(struct sockaddr_storage *addr, socklen_t len);
int handle_client_connect(struct state *state);

/* queue */

void queue_node_free(struct hsm_action_item *hai);
struct hsm_action_queues *hsm_action_queues_get(struct state *state,
						unsigned int archive_id,
						unsigned long long flags,
						const char *fsname);
void hsm_action_queues_init(struct hsm_action_queues *queues);
int hsm_action_enqueue(struct hsm_action_queues *queues,
		       struct hsm_action_item *hai);
struct hsm_action_item *hsm_action_dequeue(struct hsm_action_queues *queues,
					   enum hsm_copytool_action action);

/* common */

int epoll_addfd(int epoll_fd, int fd);
int epoll_delfd(int epoll_fd, int fd);

#endif
