
all:
	make -C basic all
	make -C src all

clean:
	make -C basic clean
	make -C src clean