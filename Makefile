CC=gcc
CFLAGS=-g
LIBS=-L ./CommandParser -lpthread -lcli
TARGET:test_udp

OBJS=gluethread/glthread.o \
                  topologies.o \
                  graph.o \
				  net.o \
				  nwcli.o \
				  comm.o \
				  test.o

test_udp:${OBJS} CommandParser/libcli.a
		${CC} ${CFLAGS} ${OBJS} -o test_udp ${LIBS}

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

nwcli.o:nwcli.c
		${CC} ${CFLAGS} -c nwcli.c -o nwcli.o

comm.o:comm.c
		${CC} ${CFLAGS} -c comm.c -o comm.o

CommandParser/libcli.a:
		(cd CommandParser; make)

clean:
		rm *.o
		rm gluethread/glthread.o
		rm test_*
		(cd CommandParser; make clean)

		