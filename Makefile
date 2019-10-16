CC=gcc
CFLAGS=-g
TARGET:test_topo

OBJS=gluethread/glthread.o \
                  topologies.o \
                  graph.o

test_topo:test.o ${OBJS}
		${CC} ${CFLAGS} test.o ${OBJS} -o test_topo

test.o:test.c
		${CC} ${CFLAGS} -c test.c -o test.o

gluethread/glthread.o:gluethread/glthread.c
		${CC} ${CFLAGS} -c gluethread/glthread.c -o gluethread/glthread.o

graph.o:graph.c
		${CC} ${CFLAGS} -c graph.c -o graph.o

topologies.o:topologies.c
		${CC} ${CFLAGS} -c topologies.c -o topologies.o

clean:
		rm *.o
		rm gluethread/glthread.o
		rm test_*
