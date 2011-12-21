.PHONY: clean cleanshm
CC = gcc
CFLAGS = -std=c99
OUTDIR = out/
OUTFILE1 = changeCaseStr
OUTFILE2 = sum
SHM = /dev/shm/sjb89_hw1-5

default : q1 q2

q1 : ${OUTFILE1}.c
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE1} $?
q2 : ${OUTFILE2}.c
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE2} $< -lrt
clean :
	-rm -f ${OUTDIR}${OUTFILE1} ${OUTDIR}${OUTFILE2} ${SHM}
cleanshm :
	-rm -f ${SHM}
