#undef _FORTIFY_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include "strformat.h"

typedef   signed char  s8;
typedef unsigned char  u8;
typedef   signed short s16;
typedef unsigned short u16;
typedef   signed int   s32;
typedef unsigned int   u32;

u16 sw16(u16 data)
{
  return (data << 8) | (data >> 8);
}

u32 sw32(u32 data)
{
  return ((data & 0xff) << 24) | ((data & 0xff00) << 8) | ((data & 0xff0000) >> 8) | ((data & 0xff000000) >> 24);
}

s8 r8s(const u8 *data)
{
  return *data;
}

u8 r8(const u8 *data)
{
  return *data;
}

s16 r16s(const u8 *data)
{
  return sw16(*(const u16 *)data);
}

u16 r16(const u8 *data)
{
  return sw16(*(const u16 *)data);
}

s32 r32s(const u8 *data)
{
  return sw32(*(const u32 *)data);
}

u32 r32(const u8 *data)
{
  return sw32(*(const u32 *)data);
}

void file_load(const char *fname, u8 **data, u32 *size)
{
  char msg[4096];
  sprintf(msg, "Open %s", fname);
  int fd = open(fname, O_RDONLY);
  if(fd<0) {
    perror(msg);
    exit(2);
  }

  *size = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  *data = (u8 *)malloc(*size);
  read(fd, *data, *size);
  close(fd);
}

int main(int argc, char **argv)
{
  if(argc != 2) {
    fprintf(stderr, "Usage:\n%s <file.bin>\n", argv[0]);
    exit(1);
  }

  u8 *data;
  u32 size;
  file_load(argv[1], &data, &size);

  u32 adr = 0x1f004;
  while(adr < 0x58eba) {
    u32 oadr = adr;
    u8 opcode = r8(data + adr++);
    std::string opc = "?";

    switch(opcode) {
    case 0x06: adr+=3; break;
    case 0x09: {
      u32 a1 = (r8(data + adr) << 16) | r16(data + adr + 1);
      adr += 3;
      opc = util::string_format("jsr.far %06x", a1);
      break;
    }
    case 0x21: {
      u32 a1 = (adr & 0xffff0000) | r16(data + adr);
      adr += 2;
      opc = util::string_format("jsr %06x", a1);
      break;
    }
    case 0x22: {
      u32 a1 = adr + r16(data + adr + 1) - 1;
      adr += 3;
      opc = util::string_format("branch.cond(?) %04x", a1);
      break;
    }
    case 0x23: {
      u32 a1 = adr + r16(data + adr + 1) - 1;
      adr += 3;
      opc = util::string_format("branch.cond(?) %04x", a1);
      break;
    }
    case 0x42: adr+=2; break;
    case 0x44: adr+=2; break;
    case 0x50: adr+=2; break;
    case 0x52: adr+=2; break;
    case 0x56: adr+=2; break;
    case 0x58: adr+=2; break;
    case 0x5a: adr+=2; break;
    case 0x5c: adr+=2; break;
    case 0x5e: adr+=2; break;
    case 0x60: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r0, %02x", value);
      break;
    }
    case 0x61: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r1, %02x", value);
      break;
    }
    case 0x62: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r2, %02x", value);
      break;
    }
    case 0x63: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r3, %02x", value);
      break;
    }
    case 0x64: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r4, %02x", value);
      break;
    }
    case 0x65: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r5, %02x", value);
      break;
    }
    case 0x66: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r6, %02x", value);
      break;
    }
    case 0x67: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r7, %02x", value);
      break;
    }
    case 0x68: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r8, %02x", value);
      break;
    }
    case 0x69: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r9, %02x", value);
      break;
    }
    case 0x6a: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r10, %02x", value);
      break;
    }
    case 0x6b: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r11, %02x", value);
      break;
    }
    case 0x6c: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r12, %02x", value);
      break;
    }
    case 0x6d: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r13, %02x", value);
      break;
    }
    case 0x6e: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r14, %02x", value);
      break;
    }
    case 0x6f: {
      u8 value = r8(data + adr++);
      opc = util::string_format("mov r15, %02x", value);
      break;
    }
    case 0x78: case 0x79: {
      u16 reg = ((opcode << 8) | r8(data + adr++)) & 0x1ff;
      opc = util::string_format("restore %03x", reg);
      break;
    }
    case 0x7e: case 0x7f: {
      u16 reg = ((opcode << 8) | r8(data + adr++)) & 0x1ff;
      opc = util::string_format("save %03x", reg);
      break;
    }
    case 0xa2: adr++; break;
    case 0xa3: adr++; break;
    case 0xe0: opc = "rts2"; break;
    case 0xe1: opc = "rts"; break;
    }
    printf("%06x:", oadr);
    for(u32 a = oadr; a < adr; a++)
      printf(" %02x", r8(data+a));
    for(u32 a = adr; a < oadr + 4; a++)
      printf("   ");
    printf("  %s\n", opc.c_str());
  }
  return 0;
}

// 61a4 -> 619c
// 7e00 -> 7800
// 7e44 -> 7844
// 7e88 -> 7888
// 7ecc -> 78cc
// 7f10 -> 7910
// 7f
