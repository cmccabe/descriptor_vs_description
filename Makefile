CFLAGS=-Wall -O2

all:		descriptor_vs_description

descriptor_vs_description:	descriptor_vs_description.o

clean:
	rm -rf descriptor_vs_description *.o
