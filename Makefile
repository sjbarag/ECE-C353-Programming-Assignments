.PHONY: clean
CC = gcc
CFLAGS = -std=c99
OUTFILE1 = changeCaseStr
OUTFILE2 = sum

q1 : ${OUTFILE1}.c
	${CC} ${CFLAGS} -o ${OUTFILE1} $?
q2 : ${OUTFILE2}.c
	${CC} ${CFLAGS} -o ${OUTFILE2} $? -lrt
clean :
	-rm -f ${OUTFILE1} ${OUTFILE2}
