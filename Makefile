CFLAGS = -I. -g3
KERNOBJS = kernels/rgbunp.o kernels/rgb2yuv.o kernels/scale.o
LIBOBJS = avscale.o kernels.o $(KERNOBJS)

all: test

clean:
	rm -f test $(LIBOBJS)

test: test.c $(LIBOBJS)
	gcc $(CFLAGS) -o test test.c $(LIBOBJS)
