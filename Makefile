.PHONY: clean cleanshm
CC = gcc
CFLAGS = -std=c99 -g
OUTDIR = out/
OUTFILE1 = changeCaseStr
OUTFILE2 = sum
SHM = /dev/shm/sjb89_hw1-5

q1 : ${OUTFILE1}.c
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE1} $?
q2 : ${OUTFILE2}.c cleanshm
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE2} $< -lrt
clean :
	-rm -f ${OUTDIR}${OUTFILE1} ${OUTDIR}${OUTFILE2} ${SHM} core
cleanshm :
	-rm -f ${SHM}
