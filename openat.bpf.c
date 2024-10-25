#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include "openat.h"

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} rb SEC(".maps");

struct openat_params_t {
    u64 __unused;
    u64 __unused2;
    u64 __unused3;

    char *filename;
};

static __inline int bpf_str_eq(const char *str1, const char *str2, int len) {
    for (int i = 0; i < len; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
        if (str1[i] == '\0') {
            break;
        }
    }
    return 1;
}


SEC("tp/syscalls/sys_enter_openat")
int handl_openat(struct openat_params_t *params)
{
    struct task_struct *task = (struct task_struct*)bpf_get_current_task();
    struct openat_evt *evt = {0};

    evt = bpf_ringbuf_reserve(&rb, sizeof(*evt), 0);
    if (!evt) {
        bpf_printk("ringbuffer not reserved\n");
        return 0;
    }

    evt->tgid = BPF_CORE_READ(task, tgid);
    evt->pid = BPF_CORE_READ(task, pid);
    bpf_get_current_comm(&evt->comm, sizeof(evt->comm));
    bpf_probe_read_user_str(evt->filename, sizeof(evt->filename), params->filename);
    if (bpf_str_eq(evt->filename, "/home/kali/Desktop/eBPF/file_monitor_openat/honeyfile", 53)) {
        bpf_ringbuf_submit(evt, 0);
        bpf_printk("Honeyfile opened, sending signal to process.\n");
        bpf_send_signal(9);
    } else {
        bpf_ringbuf_discard(evt, 0);
    }

    return 0;
}

char LICENSE[] SEC("license") = "GPL";