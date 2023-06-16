# gen makefile
# target: libsm4_avx
# suffix: .a or .so
# type: static or shared
# source files: sm4_core.c sm4-aesni-avx-asm_64.S

TARGET=libsm4_avx
STATIC_SUFFIX=.a
SHARED_SUFFIX=.so

CC=gcc
CFLAGS=-g
SHARED_CFLAGS=-g -fPIC

all: $(TARGET)$(STATIC_SUFFIX) $(TARGET)$(SHARED_SUFFIX) sm4_test


$(TARGET)$(STATIC_SUFFIX): 
	$(CC) $(CFLAGS) -c sm4_core.c sm4-aesni-avx-asm_64.S
	ar rcs $(TARGET)$(STATIC_SUFFIX) sm4_core.o sm4-aesni-avx-asm_64.o

$(TARGET)$(SHARED_SUFFIX): 
	$(CC) $(SHARED_CFLAGS) -c sm4_core.c sm4-aesni-avx-asm_64.S
	$(CC) -shared -o $(TARGET)$(SHARED_SUFFIX) $(SHARED_CFLAGS) sm4_core.o sm4-aesni-avx-asm_64.o

sm4_test:
	$(CC) $(CFLAGS) -o sm4_test test.c $(TARGET)$(STATIC_SUFFIX)

.PHONY: clean
clean:
	rm -rf *.o *.a *.so sm4_test