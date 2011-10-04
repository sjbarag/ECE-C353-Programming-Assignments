.PHONY: clean
CC = gcc
CFLAGS = -std=c99
OUTFILE = changeCaseStr

${OUTFILE} : ${OUTFILE}.c
	${CC} ${CFLAGS} -o $@ $?
clean :
	-rm -f ${OUTFILE}
