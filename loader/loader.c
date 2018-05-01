/*
 * Loader Implementation
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
#include <sys/stat.h>

#include "linked_list.h"
#include "exec_parser.h"

static so_exec_t *exec;
static char * executable_file;
static struct sigaction old_action;

static so_seg_t *get_segment(void *page_start_pointer)
{
	uintptr_t page_start = (uintptr_t) page_start_pointer;
	for (int i = 0; i < exec->segments_no; i++) {
		fprintf(stderr, "Segment %i: %p mem_size: %d file_size: %d\n",
				i,
				(void *) exec->segments[i].vaddr,
				(int) exec->segments[i].mem_size,
				(int) exec->segments[i].file_size);
		if (exec->segments[i].vaddr <= page_start
			&& exec->segments[i].vaddr + exec->segments[i].mem_size
				>= page_start) {
			return &exec->segments[i];
		}
	}
	return NULL;
}

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	void *addr;
	if (signum != SIGSEGV) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	addr = info->si_addr;
	if (addr == NULL) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}
	int pageSize = getpagesize();
	void *page_start = (void *) ((int) addr & ~(pageSize - 1));

	so_seg_t *segment = get_segment(page_start);

	if (segment == NULL) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	LINKED_LIST *segment_used_pages = (LINKED_LIST *) segment->data;
	if (!contains(&segment_used_pages, (uintptr_t) page_start)) {
		append(&segment_used_pages, (uintptr_t) page_start);
		printList(segment_used_pages);
		segment->data = (void *)segment_used_pages;
	} else {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	void *result = mmap(page_start,
						pageSize,
						segment->perm,
						MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,
						-1,
						0);
	if (result == MAP_FAILED) {
		perror("MMAP FAILED");
		exit(0);
	}
	else {
		fprintf(stderr, "Asked for %p received %p\n", page_start, result);
	}

//	if((uintptr_t)page_start >= segment->vaddr &&
//		(uintptr_t)page_start + pageSize < segment->vaddr + segment->file_size) {
//		// TODO: copy pagesize bytes from file
//	} else if ((uintptr_t)page_start >= segment->vaddr &&
//		(uintptr_t)page_start + pageSize > segment->vaddr + segment->file_size &&
//		(uintptr_t)page_start + pageSize < segment->vaddr + segment->mem_size
//		) {
//
//
//	} else if ((uintptr_t)page_start >= segment->vaddr + segment->file_size &&
//		(uintptr_t)page_start + pageSize < segment->vaddr + segment->mem_size) {
//
//	} else {
//		old_action.sa_sigaction(signum, info, context);
//		return;
//	}



}

int so_init_loader(void)
{
	/* TODO: initialize on-demand loader */
	struct sigaction action;

	int rc;

	action.sa_sigaction = segv_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, &old_action);
	if (rc == -1) {
		perror("sigaction");
	}
	return 0;
}


void get_executable(char *path) {
	/** First we map a file */
	int fd = open("Makefile", O_RDWR);
	if (fd == -1)
		perror("open");

	struct stat st;
	fstat(fd, &st);

	executable_file = mmap(NULL, st.st_size,
			 PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (executable_file == MAP_FAILED)
		perror("mmap");
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
