CC=gcc
CFLAGS=-g
TARGET:test_nw_topo

OBJS=gluethread/glthread.o \
                  topologies.o \
                  graph.o \
				  net.o

test_nw_topo:test.o ${OBJS}
		${CC} ${CFLAGS} test.o ${OBJS} -o test_nw_topo

test.o:test.c
		${CC} ${CFLAGS} -c test.c -o test.o

gluethread/glthread.o:gluethread/glthread.c
		${CC} ${CFLAGS} -c gluethread/glthread.c -o gluethread/glthread.o

graph.o:graph.c
		${CC} ${CFLAGS} -c graph.c -o graph.o

topologies.o:topologies.c
		${CC} ${CFLAGS} -c topologies.c -o topologies.o

net.o:net.c
		${CC} ${CFLAGS} -c net.c -o net.o

clean:
		rm *.o
		rm gluethread/glthread.o
		rm test_*
