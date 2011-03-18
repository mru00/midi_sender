mod = usbled

-include config-host.mak

# 2.6 build

all: $(mod).ko

$(mod).ko: TAGS
	rmmod $(mod).ko | tee
	make -C $(KERNEL_PATH) M=`pwd` modules

clean:
	rm -f $(mod).mod.o *$(mod).ko*
	rm -f TAGS
	rm -f .usbled.ko.cmd .usbled.mod.o.cmd .usbled.o.cmd
	rm -f usbled.o usbled.mod.c
	rm -fr .tmp_versions

insmod: all
	cp $(mod).ko $(mod).ins.ko
	insmod $(mod).ins.ko
	sleep 1
	chmod 0777 /dev/usbled0
	dmesg | tail

rmmod: 
	rmmod $(mod).ko 
	rm $(mod).ins.ko

TAGS: *.c
	etags --members *.c


VERSION=1.3.0pre9
FILE=kqemu-$(VERSION)

obj-m:= $(mod).o
kqemu-objs:= $(mod).o

