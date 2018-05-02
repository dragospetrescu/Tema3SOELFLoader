/*
 * Loader Implementation
 *
 * Dragos Petrescu 332CC
 *
 * 2018, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "already_mapped_pages.h"
#include "exec_parser.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static so_exec_t *exec;

static int exec_fd;

static struct sigaction old_action;

static int PAGE_SIZE;

static int get_segment(void *page_start_pointer)
{
	uintptr_t page_start = (uintptr_t) page_start_pointer;
	int i;

	for (i = 0; i < exec->segments_no; i++) {
		if (exec->segments[i].vaddr <= page_start
			&& exec->segments[i].vaddr + exec->segments[i].mem_size
				>= page_start)
			return i;
	}
	return -1;
}

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	void *addr;
	void *page_start;
	int seg_no;
	int rc;
	size_t lungime_date = 0;
	size_t lungime_zero = 0;
	struct so_seg *segment;

	if (signum != SIGSEGV) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	addr = info->si_addr;
	if (addr == NULL) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	page_start = (void *) ((int) addr & ~(PAGE_SIZE - 1));

	seg_no = get_segment(page_start);
	if (seg_no < 0) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}
	segment = &exec->segments[seg_no];

	if (!contains((uintptr_t) page_start))
		append((uintptr_t) page_start);
	else {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	int page_no = (-(int) segment->vaddr + (int) page_start) / PAGE_SIZE;

	void *result = mmap(page_start,
						PAGE_SIZE,
						PROT_WRITE |
							PROT_READ |
							PROT_EXEC,
						MAP_ANONYMOUS |
							MAP_PRIVATE |
							MAP_FIXED,
						0,
						0);
	if (result == MAP_FAILED) {
		perror("MMAP FAILED");
		exit(-1);
	}

	if ((uintptr_t) page_start < segment->vaddr + segment->file_size) {
		lungime_date = MIN(PAGE_SIZE,
						   -(int) page_start
							   + segment->vaddr
							   + segment->file_size
		);

		char buf[lungime_date];
		int offset = segment->offset + PAGE_SIZE * page_no;

		pread(exec_fd, buf, lungime_date, offset);
		memcpy(result, buf, lungime_date);
	}

	if ((uintptr_t) page_start + PAGE_SIZE
		> segment->vaddr + segment->file_size) {
		lungime_zero = MIN(segment->mem_size - segment->file_size,
						   (uintptr_t) page_start
							   + PAGE_SIZE
							   - segment->vaddr
							   + segment->mem_size);
		if (lungime_zero > 0)
			memset(result + lungime_date, '\0', lungime_zero);
	}

	rc = mprotect(page_start, PAGE_SIZE, segment->perm);
	if (rc < 0)
		perror("mprotect: ");
}

int so_init_loader(void)
{
	struct sigaction action;
	int rc;

	action.sa_sigaction = segv_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, &old_action);
	if (rc == -1)
		perror("sigaction");

	PAGE_SIZE = getpagesize();
	return 0;
}

void get_executable(char *path)
{
	int fd = open(path, O_RDWR);

	if (fd == -1)
		perror("open");
	exec_fd = fd;
}

int so_execute(char *path, char *argv[])
{
	get_executable(path);
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}
