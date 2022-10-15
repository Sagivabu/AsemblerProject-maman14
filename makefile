LFLAGS := -g -ansi -Wall -pedantic
CFLAGS := -c -ansi -Wall -pedantic

mmn14: build/main.o build/common.o build/struct.o build/passOne.o build/passTwo.o build/macroDeploy.o build/externalStruct.o
	gcc $(LFLAGS) build/main.o build/common.o build/struct.o build/passOne.o build/passTwo.o build/macroDeploy.o build/externalStruct.o -lm -o mmn14

build/main.o: src/main.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/main.c -o build/main.o

build/common.o: src/common.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/common.c -o build/common.o

build/struct.o: src/struct.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/struct.c -o build/struct.o

build/externalStruct.o: src/externalStruct.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/externalStruct.c -o build/externalStruct.o

build/macroDeploy.o: src/macroDeploy.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/macroDeploy.c -o build/macroDeploy.o

build/passOne.o: src/passOne.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/passOne.c -o build/passOne.o

build/passTwo.o: src/passTwo.c src/main.h src/common.h src/external_vars.h src/pass.h src/struct.h
	gcc $(CFLAGS) src/passTwo.c -o build/passTwo.o

clean: 
	rm -f build/*.o
	rm -f mmn14

all:
	make clean
	make 

# mmn14: main.o macroDeploy.o
# 	gcc $(CFLAGS) build/main.o build/macroDeploy.o -lm -o mmn14

# macroDeploy.o: macroDeploy.c
# 	gcc $(CFLAGS) src/macroDeploy.c - o build/macroDeploy.o