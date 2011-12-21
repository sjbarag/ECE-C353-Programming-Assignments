.PHONY: clean cleanshm
CC = gcc
CFLAGS = -std=c99
OUTDIR = out/
OUTFILE1 = threadSum
OUTFILE2 = matrixMult

default : q1 q2

q1 : ${OUTFILE1}.c
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE1} $? -pthread
q2 : ${OUTFILE2}.c
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE2} $< -pthread
clean :
	-rm -f ${OUTDIR}${OUTFILE1} ${OUTDIR}${OUTFILE2}
