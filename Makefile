CC=gcc
LIBS=-lcap
RM=rm
SET_PRIVILEGE=setcap cap_setfcap+eip

defalut: all set_privilege

all: userspace.c
	$(CC) *.c $(LIBS) -o myioctl


set_privilege:
	$(SET_PRIVILEGE) ./myioctl

clean:
	$(RM) myioctl
	