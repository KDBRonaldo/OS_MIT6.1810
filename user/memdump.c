#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  while(*fmt != '\0'){
    switch(*fmt){
    case 'i':
      printf("%d\n", (int)((uint)(uchar)data[0] | (uint)(uchar)data[1] << 8 |
                           (uint)(uchar)data[2] << 16 | (uint)(uchar)data[3] << 24));
      data += 4;
      break;
    case 'p':
      printf("%lx\n", (uint64)(uchar)data[0] | (uint64)(uchar)data[1] << 8 |
                       (uint64)(uchar)data[2] << 16 | (uint64)(uchar)data[3] << 24 |
                       (uint64)(uchar)data[4] << 32 | (uint64)(uchar)data[5] << 40 |
                       (uint64)(uchar)data[6] << 48 | (uint64)(uchar)data[7] << 56);
      data += 8;
      break;
    case 'h':
      printf("%d\n", (int)((ushort)(uchar)data[0] | (ushort)(uchar)data[1] << 8));
      data += 2;
      break;
    case 'c':
      printf("%c\n", *data);
      data += 1;
      break;
    case 's':
      printf("%s\n", (char *)((uint64)(uchar)data[0] | (uint64)(uchar)data[1] << 8 |
                              (uint64)(uchar)data[2] << 16 | (uint64)(uchar)data[3] << 24 |
                              (uint64)(uchar)data[4] << 32 | (uint64)(uchar)data[5] << 40 |
                              (uint64)(uchar)data[6] << 48 | (uint64)(uchar)data[7] << 56));
      data += 8;
      break;
    case 'S':
      printf("%s\n", data);
      while(*data != '\0')
        data++;
      data++;
      break;
    }
    fmt++;
  }
}
