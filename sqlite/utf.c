/*
** 2004 April 13
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains routines used to translate between UTF-8, 
** UTF-16, UTF-16BE, and UTF-16LE.
**
** $Id: utf.c,v 1.32 2005/01/28 01:29:08 drh Exp $
**
** Notes on UTF-8:
**
**   Byte-0    Byte-1    Byte-2    Byte-3    Value
**  0xxxxxxx                                 00000000 00000000 0xxxxxxx
**  110yyyyy  10xxxxxx                       00000000 00000yyy yyxxxxxx
**  1110zzzz  10yyyyyy  10xxxxxx             00000000 zzzzyyyy yyxxxxxx
**  11110uuu  10uuzzzz  10yyyyyy  10xxxxxx   000uuuuu zzzzyyyy yyxxxxxx
**
**
** Notes on UTF-16:  (with wwww+1==uuuuu)
**
**      Word-0               Word-1          Value
**  110110ww wwzzzzyy   110111yy yyxxxxxx    000uuuuu zzzzyyyy yyxxxxxx
**  zzzzyyyy yyxxxxxx                        00000000 zzzzyyyy yyxxxxxx
**
**
** BOM or Byte Order Mark:
**     0xff 0xfe   little-endian utf-16 follows
**     0xfe 0xff   big-endian utf-16 follows
**
**
** Handling of malformed strings:
**
** SQLite accepts and processes malformed strings without an error wherever
** possible. However this is not possible when converting between UTF-8 and
** UTF-16.
**
** When converting malformed UTF-8 strings to UTF-16, one instance of the
** replacement character U+FFFD for each byte that cannot be interpeted as
** part of a valid unicode character.
**
** When converting malformed UTF-16 strings to UTF-8, one instance of the
** replacement character U+FFFD for each pair of bytes that cannot be
** interpeted as part of a valid unicode character.
**
** This file contains the following public routines:
**
** sqlite3VdbeMemTranslate() - Translate the encoding used by a Mem* string.
** sqlite3VdbeMemHandleBom() - Handle byte-order-marks in UTF16 Mem* strings.
** sqlite3utf16ByteLen()     - Calculate byte-length of a void* UTF16 string.
** sqlite3utf8CharLen()      - Calculate char-length of a char* UTF8 string.
** sqlite3utf8LikeCompare()  - Do a LIKE match given two UTF8 char* strings.
**
*/
#include "sqliteInt.h"
#include <assert.h>
#include "vdbeInt.h"

/*
** This table maps from the first byte of a UTF-8 character to the number
** of trailing bytes expected. A value '255' indicates that the table key
** is not a legal first byte for a UTF-8 character.
*/
static const u8 xtra_utf8_bytes[256]  = {
/* 0xxxxxxx */
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0,

/* 10wwwwww */
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

/* 110yyyyy */
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,     1, 1, 1, 1, 1, 1, 1, 1,

/* 1110zzzz */
2, 2, 2, 2, 2, 2, 2, 2,     2, 2, 2, 2, 2, 2, 2, 2,

/* 11110yyy */
3, 3, 3, 3, 3, 3, 3, 3,     255, 255, 255, 255, 255, 255, 255, 255,
};

/*
** This table maps from the number of trailing bytes in a UTF-8 character
** to an integer constant that is effectively calculated for each character
** read by a naive implementation of a UTF-8 character reader. The code
** in the READ_UTF8 macro explains things best.
*/
static const int xtra_utf8_bits[4] =  {
0,
12416,          /* (0xC0 << 6) + (0x80) */
925824,         /* (0xE0 << 12) + (0x80 << 6) + (0x80) */
63447168        /* (0xF0 << 18) + (0x80 << 12) + (0x80 << 6) + 0x80 */
};

#define READ_UTF8(zIn, c) { \
  int xtra;                                            \
  c = *(zIn)++;                                        \
  xtra = xtra_utf8_bytes[c];                           \
  switch( xtra ){                                      \
    case 255: c = (int)0xFFFD; break;                  \
    case 3: c = (c<<6) + *(zIn)++;                     \
    case 2: c = (c<<6) + *(zIn)++;                     \
    case 1: c = (c<<6) + *(zIn)++;                     \
    c -= xtra_utf8_bits[xtra];                         \
  }                                                    \
}
int sqlite3ReadUtf8(const unsigned char *z){
  int c;
  READ_UTF8(z, c);
  return c;
}

#define SKIP_UTF8(zIn) {                               \
  zIn += (xtra_utf8_bytes[*(u8 *)zIn] + 1);            \
}

#define WRITE_UTF8(zOut, c) {                          \
  if( c<0x00080 ){                                     \
    *zOut++ = (c&0xFF);                                \
  }                                                    \
  else if( c<0x00800 ){                                \
    *zOut++ = 0xC0 + ((c>>6)&0x1F);                    \
    *zOut++ = 0x80 + (c & 0x3F);                       \
  }                                                    \
  else if( c<0x10000 ){                                \
    *zOut++ = 0xE0 + ((c>>12)&0x0F);                   \
    *zOut++ = 0x80 + ((c>>6) & 0x3F);                  \
    *zOut++ = 0x80 + (c & 0x3F);                       \
  }else{                                               \
    *zOut++ = 0xF0 + ((c>>18) & 0x07);                 \
    *zOut++ = 0x80 + ((c>>12) & 0x3F);                 \
    *zOut++ = 0x80 + ((c>>6) & 0x3F);                  \
    *zOut++ = 0x80 + (c & 0x3F);                       \
  }                                                    \
}

#define WRITE_UTF16LE(zOut, c) {                                \
  if( c<=0xFFFF ){                                              \
    *zOut++ = (c&0x00FF);                                       \
    *zOut++ = ((c>>8)&0x00FF);                                  \
  }else{                                                        \
    *zOut++ = (((c>>10)&0x003F) + (((c-0x10000)>>10)&0x00C0));  \
    *zOut++ = (0x00D8 + (((c-0x10000)>>18)&0x03));              \
    *zOut++ = (c&0x00FF);                                       \
    *zOut++ = (0x00DC + ((c>>8)&0x03));                         \
  }                                                             \
}

#define WRITE_UTF16BE(zOut, c) {                                \
  if( c<=0xFFFF ){                                              \
    *zOut++ = ((c>>8)&0x00FF);                                  \
    *zOut++ = (c&0x00FF);                                       \
  }else{                                                        \
    *zOut++ = (0x00D8 + (((c-0x10000)>>18)&0x03));              \
    *zOut++ = (((c>>10)&0x003F) + (((c-0x10000)>>10)&0x00C0));  \
    *zOut++ = (0x00DC + ((c>>8)&0x03));                         \
    *zOut++ = (c&0x00FF);                                       \
  }                                                             \
}

#define READ_UTF16LE(zIn, c){                                         \
  c = (*zIn++);                                                       \
  c += ((*zIn++)<<8);                                                 \
  if( c>=0xD800 && c<=0xE000 ){                                       \
    int c2 = (*zIn++);                                                \
    c2 += ((*zIn++)<<8);                                              \
    c = (c2&0x03FF) + ((c&0x003F)<<10) + (((c&0x03C0)+0x0040)<<10);   \
  }                                                                   \
}

#define READ_UTF16BE(zIn, c){                                         \
  c = ((*zIn++)<<8);                                                  \
  c += (*zIn++);                                                      \
  if( c>=0xD800 && c<=0xE000 ){                                       \
    int c2 = ((*zIn++)<<8);                                           \
    c2 += (*zIn++);                                                   \
    c = (c2&0x03FF) + ((c&0x003F)<<10) + (((c&0x03C0)+0x0040)<<10);   \
  }                                                                   \
}

#define SKIP_UTF16BE(zIn){                                            \
  if( *zIn>=0xD8 && (*zIn<0xE0 || (*zIn==0xE0 && *(zIn+1)==0x00)) ){  \
    zIn += 4;                                                         \
  }else{                                                              \
    zIn += 2;                                                         \
  }                                                                   \
}
#define SKIP_UTF16LE(zIn){                                            \
  zIn++;                                                              \
  if( *zIn>=0xD8 && (*zIn<0xE0 || (*zIn==0xE0 && *(zIn-1)==0x00)) ){  \
    zIn += 3;                                                         \
  }else{                                                              \
    zIn += 1;                                                         \
  }                                                                   \
}

#define RSKIP_UTF16LE(zIn){                                            \
  if( *zIn>=0xD8 && (*zIn<0xE0 || (*zIn==0xE0 && *(zIn-1)==0x00)) ){  \
    zIn -= 4;                                                         \
  }else{                                                              \
    zIn -= 2;                                                         \
  }                                                                   \
}
#define RSKIP_UTF16BE(zIn){                                            \
  zIn--;                                                              \
  if( *zIn>=0xD8 && (*zIn<0xE0 || (*zIn==0xE0 && *(zIn+1)==0x00)) ){  \
    zIn -= 3;                                                         \
  }else{                                                              \
    zIn -= 1;                                                         \
  }                                                                   \
}

/*
** If the TRANSLATE_TRACE macro is defined, the value of each Mem is
** printed on stderr on the way into and out of sqlite3VdbeMemTranslate().
*/ 
/* #define TRANSLATE_TRACE 1 */

/*
** pZ is a UTF-8 encoded unicode string. If nByte is less than zero,
** return the number of unicode characters in pZ up to (but not including)
** the first 0x00 byte. If nByte is not less than zero, return the
** number of unicode characters in the first nByte of pZ (or up to 
** the first 0x00, whichever comes first).
*/
int sqlite3utf8CharLen(const char *z, int nByte){
  int r = 0;
  const char *zTerm;
  if( nByte>=0 ){
    zTerm = &z[nByte];
  }else{
    zTerm = (const char *)(-1);
  }
  assert( z<=zTerm );
  while( *z!=0 && z<zTerm ){
    SKIP_UTF8(z);
    r++;
  }
  return r;
}

