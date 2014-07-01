# gcc -o pesqmain pesqmain.c
objects = pesqmain.o pesqmod.o pesqio.o pesqdsp.o dsp.o
headers = dsp.h pesq.h pesqpar.h

CC=gcc

all : pesq

# Just to get the include file dependancy...
dsp.o : dsp.c $(headers)
	$(CC) -c dsp.c

pesqdsp.o : pesqdsp.c $(headers)
	$(CC) -c pesqdsp.c

pesqio.o : pesqio.c $(headers)
	$(CC) -c pesqio.c

pesqmod.o : pesqmod.c $(headers)
	$(CC) -c pesqmod.c

pesq : $(objects) $(headers)
	$(CC) -o pesq $(objects)

install : pesq
	cp pesq.exe "../PESQ_calc/pesq.exe"

clean :
	rm -f $(objects) pesq

