CC=gcc
server:main-ev.o CProjectDfn.o MemoryPool.o threadpool.o
	$(CC) -o server main-ev.o CProjectDfn.o MemoryPool.o threadpool.o -lev -lpthread

main-ev.o:main-ev.c types_define.h OBD_mq.h OBD_struct.h OBD_connect_task.h
	$(CC) -c main-ev.c
CProjectDfn.o:FULMemoryPool/CProjectDfn.c FULMemoryPool/CProjectDfn.h
	$(CC) -c FULMemoryPool/CProjectDfn.c
MemoryPool.o:FULMemoryPool/MemoryPool.c FULMemoryPool/MemoryPool.h
	$(CC) -c FULMemoryPool/MemoryPool.c
threadpool.o:threadpool/threadpool.c threadpool/threadpool.h
	$(CC) -c threadpool/threadpool.c
clean:
	rm -rf server client  *.o
