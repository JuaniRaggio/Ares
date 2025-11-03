include Makefile.inc

SAMPLE_DATA=0001-userDataModule.bin

all: userCodeModule userDataModule

userCodeModule:
	cd UserCodeModule; make

userDataModule:
	printf "This is sample data." >> $(SAMPLE_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SAMPLE_DATA)

clean:
	cd UserCodeModule; make clean
	rm -rf *.bin


.PHONY: userCodeModule all clean
