.PHONY: clean cleanshm
CC = gcc
CFLAGS  = -std=c99
OUTDIR  = out/
SRC_1   = src/work_crew.c
SRC_2   = src/queue_utils.c
HDR     = src/queue.h
OUTFILE = out/work_crew

default : q1

q1 : ${SRC_1} ${SRC_2} ${HDR} ${OUTDIR}
	${CC} ${CFLAGS} -o ${OUTFILE} ${SRC_1} ${SRC_2}

${OUTDIR}:
	mkdir out/

clean :
	-rm -f ${OUTFILE}
