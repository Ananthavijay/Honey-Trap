APP=openat

BPFTOOL=bpftool
LIBBPF_PATH=/home/kali/Desktop/libbpf/src/build/libbpf.a
LIBBPF_INCLUDE=/home/kali/Desktop/libbpf/src/root/usr/include/


.PHONY: $(APP)
$(APP): skel
	clang openat.c -Wall $(LIBBPF_PATH) -lelf -lz -I$(LIBBPF_INCLUDE) -o $(APP)

.PHONY: vmlinux
vmlinux:
	$(BPFTOOL) btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

.PHONY: bpf
bpf: vmlinux
	clang -g -O3 -target bpf -D__TARGET_ARCH_x86_64 -I$(LIBBPF_INCLUDE) -c openat.bpf.c -o openat.bpf.o

.PHONY: skel
skel: bpf
	$(BPFTOOL) gen skeleton openat.bpf.o name openat > openat.skel.h

.PHONY: run
run: $(APP)
	sudo ./$(APP)

.PHONY: clean
clean:
	-rm -rf *.o *.skel.h vmlinux.h $(APP)

.PHONY: gdb
gdb: bpf
	sudo gdb --args $(BPFTOOL) gen skeleton openat.bpf.o name openat
