#ifndef _COPYTLIB_H
#define _COPYTLIB_H

#include <glide.h>

#define NO_TABLE ((GrTexTable_t)(~0))

// Copy from 3dfx tlib
typedef FxU32 TlPalette[256];

typedef struct {
  FxU8  yRGB[16];
  FxI16 iRGB[4][3];
  FxI16 qRGB[4][3];
  FxU32 packed_data[12];
} TlNCCTable;

typedef union {
    TlPalette  palette;
    TlNCCTable nccTable;
} TlTextureTable;

typedef struct {
    GrTexInfo      info;
    GrTexTable_t   tableType;
    TlTextureTable tableData;
} TlTexture;

#endif // _COPYTLIB_H
