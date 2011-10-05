.PHONY: clean
CC = gcc
CFLAGS = -std=c99 -g
OUTFILE1 = changeCaseStr
OUTFILE2 = sum
SHM = /dev/shm/sjb89_hw1-5

q1 : ${OUTFILE1}.c
	${CC} ${CFLAGS} -o ${OUTFILE1} $?
q2 : ${OUTFILE2}.c
	${CC} ${CFLAGS} -o ${OUTFILE2} $? -lrt
clean :
	-rm -f ${OUTFILE1} ${OUTFILE2} ${SHM}
cleanshm :
	-rm -f ${SHM}
