.PHONY: clean cleanshm
CC = gcc
CFLAGS = -std=c99
LFLAGS = -lpthread -lm
OUTDIR = out/
OUTFILE1 = sleeping_barber
OUTFILE2 = histogram

default : q1 q2

q1 : ${OUTFILE1}.c ${OUTDIR}
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE1} $< ${LFLAGS} -lrt
q2 : ${OUTFILE2}.c ${OUTDIR}
	${CC} ${CFLAGS} -o ${OUTDIR}${OUTFILE2} $< ${LFLAGS}
${OUTDIR}:
	mkdir out/
clean :
	-rm -f ${OUTDIR}${OUTFILE1} ${OUTDIR}${OUTFILE2}
