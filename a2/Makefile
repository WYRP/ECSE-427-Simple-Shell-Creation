CC=gcc
CFLAGS=

# Add macros for frame store and variable store sizes
CFLAGS += -Dframesize=$(framesize) -Dvarmemsize=$(varmemsize)

myshell: shell.c interpreter.c shellmemory.c kernel.c pcb.c ready_queue.c
	$(CC) $(CFLAGS) -c -g shell.c interpreter.c shellmemory.c kernel.c pcb.c ready_queue.c
	$(CC) $(CFLAGS) -o myshell shell.o interpreter.o shellmemory.o kernel.o pcb.o ready_queue.o

clean: 
	rm -f myshell *.o