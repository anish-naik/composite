#!/bin/sh

cp omp_simple.o llboot.o
./cos_linker "llboot.o, :" ./gen_client_stub

#cp llboot_comp.o llboot.o
#cp omp_hello.o boot.o
#cp test_boot.o dummy1.o
#cp test_boot.o dummy2.o
#./cos_linker "llboot.o, ;dummy1.o, ;capmgr.o, ;dummy2.o, ;*boot.o, :boot.o-capmgr.o" ./gen_client_stub