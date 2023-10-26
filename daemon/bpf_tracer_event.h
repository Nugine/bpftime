/* SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause) */
#ifndef __SYSCALL_TRACER_H
#define __SYSCALL_TRACER_H

#define TASK_COMM_LEN 16
#define NAME_MAX 255
#define INVALID_UID ((uid_t)-1)

#define MAX_INSN_SIZE 128

#define BPF_OBJ_NAME_LEN 16U

enum event_type {
	SYS_OPEN,
	SYS_CLOSE,
	SYS_BPF,
	SYS_IOCTL,
	SYS_PERF_EVENT_OPEN,
	BPF_PROG_LOAD_EVENT,
};

enum bpf_fd_type {
	BPF_FD_TYPE_OTHERS,
	BPF_FD_TYPE_PROG,
	BPF_FD_TYPE_MAP,
	BPF_FD_TYPE_MAX,
};

struct bpf_fd_data {
	enum bpf_fd_type type;
	// porg id or map id;
	int kernel_id;
};

struct event {
	enum event_type type;

	// basic info
	int pid;
	char comm[TASK_COMM_LEN];

	union {
		struct {
			int ret;
			unsigned int bpf_cmd;
			union bpf_attr attr;
			unsigned int attr_size;
			// additional field for getting map id
			int map_id;
		} bpf_data;

		struct {
			int ret;
			int flags;
			char fname[NAME_MAX];
		} open_data;

		struct {
			int ret;
			struct perf_event_attr attr;
			int pid;
			int cpu;

			// uprobe data
			char name_or_path[NAME_MAX];
		} perf_event_data;

		struct {
			unsigned int type;
			unsigned int insn_cnt;
			char prog_name[BPF_OBJ_NAME_LEN];
			// used as key for later lookup in userspace
			unsigned long long insns_ptr;
		} bpf_loaded_prog;

		struct {
			int fd;
			struct bpf_fd_data fd_data;
		} close_data;

		struct {
			int fd;
			unsigned long req;
			int data;
			int ret;

			unsigned int bpf_prog_id;
		} ioctl_data;
	};
};

#endif /* __SYSCALL_TRACER_H */
