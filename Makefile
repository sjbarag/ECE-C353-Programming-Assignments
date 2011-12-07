.PHONY: clean
CC = gcc
CFLAGS  = -std=c99 -lpthread
OUTDIR  = out/
SRC_1   = src/work_crew.c
SRC_2   = src/queue_utils.c
HDR_1   = src/work_crew.h
HDR_2   = src/queue.h
OUTFILE = out/work_crew
DIR     = /home/sean/school
STRING  = double

default : q1

q1 : ${SRC_1} ${SRC_2} ${HDR_1} ${HDR_2} ${OUTDIR}
	${CC} ${CFLAGS} -o ${OUTFILE} ${SRC_1} ${SRC_2}

run : ${OUTFILE}
	./${OUTFILE} ${STRING} ${DIR}

${OUTDIR}:
	mkdir out/

clean :
	-rm -f ${OUTFILE}
