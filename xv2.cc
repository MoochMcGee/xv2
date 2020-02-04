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

u32 r24(const u8 *data)
{
  return (data[0] << 16) | (data[1] << 8) | data[2];
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

  static const char *regs[8] = { "r0", "r1", "r2", "r3", "r4", "r5", "sp", "lnk" };

  u32 adr = 0;
  while(adr < 0x58eba) {
    u32 oadr = adr;
    u8 opcode = r8(data + adr++);
    std::string opc = "?";

    switch(opcode) {
    case 0x06: case 0x07: {
      u32 value = r24(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 22);
      value &= 0x3fffff;
      adr += 3;
      opc = util::string_format("%s = %06x", regs[reg], value);
      break;
    }
    case 0x08: {
      u32 value = r24(data + adr);
      adr += 3;
      opc = util::string_format("jmp.far %06x", value);
      break;
    }
    case 0x09: {
      u32 a1 = r24(data + adr);
      adr += 3;
      opc = util::string_format("jsr.far %06x", a1);
      break;
    }
    case 0x0a: case 0xb: {
      u32 value = r24(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 22);
      value &= 0x3fffff;
      adr += 3;
      opc = util::string_format("%s &= %06x", regs[reg], value);
      break;
    }
    case 0x0c: case 0xd: {
      u32 value = r24(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 22);
      value &= 0x3fffff;
      adr += 3;
      opc = util::string_format("%s |= %06x", regs[reg], value);
      break;
    }
    case 0x20: {
      u32 a1 = (adr & 0xffff0000) | r16(data + adr);
      adr += 2;
      opc = util::string_format("jmp %06x", a1);
      break;
    }
    case 0x21: {
      u32 a1 = (adr & 0xffff0000) | r16(data + adr);
      adr += 2;
      opc = util::string_format("jsr %06x", a1);
      break;
    }
    case 0x22: case 0x23: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x0000", regs[reg], value << 2);
      break;
    }
    case 0x26: case 0x27: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("cmp %s, %04x", regs[reg], value);
      break;
    }
    case 0x2a: case 0x2b: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      u8 reg2 = (value >> 11) & 7;
      value &= 0x7ff;
      adr += 2;
      opc = util::string_format("%s = %s & %04x", regs[reg], regs[reg2], value);
      break;
    }
    case 0x2c: case 0x2d: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      u8 reg2 = (value >> 11) & 7;
      value &= 0x7ff;
      adr += 2;
      opc = util::string_format("%s = %s | %04x", regs[reg], regs[reg2], value);
      break;
    }

    case 0x42: adr+=2; break;
    case 0x44: adr+=2; break;
    case 0x50: case 0x51: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x.?", regs[reg], value);
      break;
    }
    case 0x52: case 0x53: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x.b", regs[reg], value);
      break;
    }
    case 0x54: case 0x55: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x.w", regs[reg], value);
      break;
    }
    case 0x56: case 0x57: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x.l", regs[reg], value);
      break;
    }
    case 0x58: case 0x59: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%s = %04x.l?", regs[reg], value);
      break;
    }
    case 0x5a: case 0x5b: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%04x.b = %s", value, regs[reg]);
      break;
    }
    case 0x5c: case 0x5d: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%04x.w = %s", value, regs[reg]);
      break;
    }
    case 0x5e: case 0x5f: {
      u16 value = r16(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 14);
      value &= 0x3fff;
      adr += 2;
      opc = util::string_format("%04x.l = %s", value, regs[reg]);
      break;
    }
    case 0x60: case 0x61: {
      u8 value = r8(data + adr++);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      value &= 0x3f;

      if(value & 0x20)
	opc = util::string_format("%s -= %02x", regs[reg], 0x40 - value);
      else
	opc = util::string_format("%s += %02x", regs[reg], value);
      break;
    }
    case 0x62: case 0x63: {
      u8 value = r8(data + adr++);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      value &= 0x3f;

      if(value & 0x20)
	opc = util::string_format("%s = -%02x", regs[reg], 0x40 - value);
      else
	opc = util::string_format("%s = %02x", regs[reg], value);
      break;
    }

    case 0x66: case 0x67: {
      u8 value = r8(data + adr++);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      value &= 0x3f;

      if(value & 0x20)
	opc = util::string_format("cmp %s, -%02x", regs[reg], 0x40 - value);
      else
	opc = util::string_format("cmp %s, %02x", regs[reg], value);
      break;
    }

    case 0x6e: adr++; break;
    case 0x78: case 0x79: {
      u8 value = r8(data + adr++);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      value &= 0x3f;

      if(value & 0x20)
	opc = util::string_format("%s = -%02x[sp].l", regs[reg], 0x40 - value);
      else
	opc = util::string_format("%s = %02x[sp].l", regs[reg], value);
      break;
    }
    case 0x7e: case 0x7f: {
      u8 value = r8(data + adr++);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      value &= 0x3f;

      if(value & 0x20)
	opc = util::string_format("-%02x[sp].l = %s", 0x40 - value, regs[reg]);
      else
	opc = util::string_format("%02x[sp].l = %s", value, regs[reg]);
      break;
    }
    case 0x90: case 0x91: {
      u16 value = r8(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      u8 reg2 = (value >> 3) & 7;
      adr++;
      opc = util::string_format("%s = (%s).b", regs[reg], regs[reg2]);
      break;
    }

    case 0x98: case 0x99: {
      u16 value = r8(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      u8 reg2 = (value >> 3) & 7;
      adr++;
      opc = util::string_format("%s = (%s).l", regs[reg], regs[reg2]);
      break;
    }

    case 0x9e: case 0x9f: {
      u16 value = r8(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      u8 reg2 = (value >> 3) & 7;
      adr++;
      opc = util::string_format("(%s).%x = l", regs[reg2], regs[reg]);
      break;
    }

    case 0xa2: case 0xa3: {
      u16 value = r8(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      u8 reg2 = (value >> 3) & 7;
      adr++;
      opc = util::string_format("%s = %s", regs[reg], regs[reg2]);
      break;
    }

    case 0xa6: case 0xa7: {
      u16 value = r8(data + adr);
      u8 reg = ((opcode & 1) << 2) | (value >> 6);
      u8 reg2 = (value >> 3) & 7;
      adr++;
      opc = util::string_format("cmp %s, %s", regs[reg], regs[reg2]);
      break;
    }

    case 0xc8: adr++; break;
    case 0xca: adr++; break;
    case 0xd2: {
      s8 value = r8(data + adr++);
      opc = util::string_format("b?? %06x", adr - 2 + value);
      break;
    }
    case 0xd5: {
      s8 value = r8(data + adr++);
      opc = util::string_format("b?? %06x", adr - 2 + value);
      break;
    }
    case 0xd7: {
      s8 value = r8(data + adr++);
      opc = util::string_format("b?? %06x", adr - 2 + value);
      break;
    }
    case 0xda: {
      s8 value = r8(data + adr++);
      opc = util::string_format("bne %06x", adr - 2 + value);
      break;
    }

    case 0xe0: opc = "jmp (lnk)"; break;
    case 0xe1: opc = "jmp (a)"; break;
    case 0xe3: opc = "rti"; break;
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
