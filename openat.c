#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "openat.skel.h"
#include "openat.h"

static void bump_memlock_rlimit(void)
{
	struct rlimit rlim_new = {
		.rlim_cur	= RLIM_INFINITY,
		.rlim_max	= RLIM_INFINITY,
	};

	if (setrlimit(RLIMIT_MEMLOCK, &rlim_new)) {
		fprintf(stderr, "Failed to increase RLIMIT_MEMLOCK limit!\n");
		exit(1);
	}
}

static int handle_evt(void *ctx, void *data, size_t sz)
{
    const struct openat_evt *evt = data;

    fprintf(stdout, "tgid: %d <> pid: %d -- comm: %s <> file: %s\n", evt->tgid, evt->pid, evt->comm, evt->filename);

    return 0;
}

int main(void)
{
    bump_memlock_rlimit();

    struct openat *skel = openat__open();
    openat__load(skel);
    openat__attach(skel);

    struct ring_buffer *rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_evt, NULL, NULL);

    for(;;) {
        ring_buffer__poll(rb, 1000);
    }
    return 0;
}