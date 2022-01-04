/* Generated automatically from machmode.def and config/aarch64/aarch64-modes.def
   by genmodes.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "real.h"

const char *const mode_name[NUM_MACHINE_MODES] =
{
  "VOID",
  "BLK",
  "CC",
  "CCFP",
  "CCFPE",
  "CC_SWP",
  "CC_NZ",
  "CC_Z",
  "CC_C",
  "CC_ADC",
  "CC_V",
  "BI",
  "QI",
  "HI",
  "SI",
  "DI",
  "TI",
  "OI",
  "CI",
  "XI",
  "QQ",
  "HQ",
  "SQ",
  "DQ",
  "TQ",
  "UQQ",
  "UHQ",
  "USQ",
  "UDQ",
  "UTQ",
  "HA",
  "SA",
  "DA",
  "TA",
  "UHA",
  "USA",
  "UDA",
  "UTA",
  "HF",
  "SF",
  "DF",
  "TF",
  "SD",
  "DD",
  "TD",
  "CQI",
  "CHI",
  "CSI",
  "CDI",
  "CTI",
  "COI",
  "CCI",
  "CXI",
  "HC",
  "SC",
  "DC",
  "TC",
  "VNx16BI",
  "VNx8BI",
  "VNx4BI",
  "VNx2BI",
  "V8QI",
  "V4HI",
  "V2SI",
  "V16QI",
  "VNx16QI",
  "V8HI",
  "VNx8HI",
  "V4SI",
  "VNx4SI",
  "V2DI",
  "VNx2DI",
  "VNx32QI",
  "VNx16HI",
  "VNx8SI",
  "VNx4DI",
  "VNx2TI",
  "VNx48QI",
  "VNx24HI",
  "VNx12SI",
  "VNx6DI",
  "VNx3TI",
  "VNx64QI",
  "VNx32HI",
  "VNx16SI",
  "VNx8DI",
  "VNx4TI",
  "VNx2OI",
  "V2HF",
  "V4HF",
  "V2SF",
  "V1DF",
  "V8HF",
  "VNx8HF",
  "V4SF",
  "VNx4SF",
  "V2DF",
  "VNx2DF",
  "VNx16HF",
  "VNx8SF",
  "VNx4DF",
  "VNx24HF",
  "VNx12SF",
  "VNx6DF",
  "VNx32HF",
  "VNx16SF",
  "VNx8DF",
};

const unsigned char mode_class[NUM_MACHINE_MODES] =
{
  MODE_RANDOM,             /* VOID */
  MODE_RANDOM,             /* BLK */
  MODE_CC,                 /* CC */
  MODE_CC,                 /* CCFP */
  MODE_CC,                 /* CCFPE */
  MODE_CC,                 /* CC_SWP */
  MODE_CC,                 /* CC_NZ */
  MODE_CC,                 /* CC_Z */
  MODE_CC,                 /* CC_C */
  MODE_CC,                 /* CC_ADC */
  MODE_CC,                 /* CC_V */
  MODE_INT,                /* BI */
  MODE_INT,                /* QI */
  MODE_INT,                /* HI */
  MODE_INT,                /* SI */
  MODE_INT,                /* DI */
  MODE_INT,                /* TI */
  MODE_INT,                /* OI */
  MODE_INT,                /* CI */
  MODE_INT,                /* XI */
  MODE_FRACT,              /* QQ */
  MODE_FRACT,              /* HQ */
  MODE_FRACT,              /* SQ */
  MODE_FRACT,              /* DQ */
  MODE_FRACT,              /* TQ */
  MODE_UFRACT,             /* UQQ */
  MODE_UFRACT,             /* UHQ */
  MODE_UFRACT,             /* USQ */
  MODE_UFRACT,             /* UDQ */
  MODE_UFRACT,             /* UTQ */
  MODE_ACCUM,              /* HA */
  MODE_ACCUM,              /* SA */
  MODE_ACCUM,              /* DA */
  MODE_ACCUM,              /* TA */
  MODE_UACCUM,             /* UHA */
  MODE_UACCUM,             /* USA */
  MODE_UACCUM,             /* UDA */
  MODE_UACCUM,             /* UTA */
  MODE_FLOAT,              /* HF */
  MODE_FLOAT,              /* SF */
  MODE_FLOAT,              /* DF */
  MODE_FLOAT,              /* TF */
  MODE_DECIMAL_FLOAT,      /* SD */
  MODE_DECIMAL_FLOAT,      /* DD */
  MODE_DECIMAL_FLOAT,      /* TD */
  MODE_COMPLEX_INT,        /* CQI */
  MODE_COMPLEX_INT,        /* CHI */
  MODE_COMPLEX_INT,        /* CSI */
  MODE_COMPLEX_INT,        /* CDI */
  MODE_COMPLEX_INT,        /* CTI */
  MODE_COMPLEX_INT,        /* COI */
  MODE_COMPLEX_INT,        /* CCI */
  MODE_COMPLEX_INT,        /* CXI */
  MODE_COMPLEX_FLOAT,      /* HC */
  MODE_COMPLEX_FLOAT,      /* SC */
  MODE_COMPLEX_FLOAT,      /* DC */
  MODE_COMPLEX_FLOAT,      /* TC */
  MODE_VECTOR_BOOL,        /* VNx16BI */
  MODE_VECTOR_BOOL,        /* VNx8BI */
  MODE_VECTOR_BOOL,        /* VNx4BI */
  MODE_VECTOR_BOOL,        /* VNx2BI */
  MODE_VECTOR_INT,         /* V8QI */
  MODE_VECTOR_INT,         /* V4HI */
  MODE_VECTOR_INT,         /* V2SI */
  MODE_VECTOR_INT,         /* V16QI */
  MODE_VECTOR_INT,         /* VNx16QI */
  MODE_VECTOR_INT,         /* V8HI */
  MODE_VECTOR_INT,         /* VNx8HI */
  MODE_VECTOR_INT,         /* V4SI */
  MODE_VECTOR_INT,         /* VNx4SI */
  MODE_VECTOR_INT,         /* V2DI */
  MODE_VECTOR_INT,         /* VNx2DI */
  MODE_VECTOR_INT,         /* VNx32QI */
  MODE_VECTOR_INT,         /* VNx16HI */
  MODE_VECTOR_INT,         /* VNx8SI */
  MODE_VECTOR_INT,         /* VNx4DI */
  MODE_VECTOR_INT,         /* VNx2TI */
  MODE_VECTOR_INT,         /* VNx48QI */
  MODE_VECTOR_INT,         /* VNx24HI */
  MODE_VECTOR_INT,         /* VNx12SI */
  MODE_VECTOR_INT,         /* VNx6DI */
  MODE_VECTOR_INT,         /* VNx3TI */
  MODE_VECTOR_INT,         /* VNx64QI */
  MODE_VECTOR_INT,         /* VNx32HI */
  MODE_VECTOR_INT,         /* VNx16SI */
  MODE_VECTOR_INT,         /* VNx8DI */
  MODE_VECTOR_INT,         /* VNx4TI */
  MODE_VECTOR_INT,         /* VNx2OI */
  MODE_VECTOR_FLOAT,       /* V2HF */
  MODE_VECTOR_FLOAT,       /* V4HF */
  MODE_VECTOR_FLOAT,       /* V2SF */
  MODE_VECTOR_FLOAT,       /* V1DF */
  MODE_VECTOR_FLOAT,       /* V8HF */
  MODE_VECTOR_FLOAT,       /* VNx8HF */
  MODE_VECTOR_FLOAT,       /* V4SF */
  MODE_VECTOR_FLOAT,       /* VNx4SF */
  MODE_VECTOR_FLOAT,       /* V2DF */
  MODE_VECTOR_FLOAT,       /* VNx2DF */
  MODE_VECTOR_FLOAT,       /* VNx16HF */
  MODE_VECTOR_FLOAT,       /* VNx8SF */
  MODE_VECTOR_FLOAT,       /* VNx4DF */
  MODE_VECTOR_FLOAT,       /* VNx24HF */
  MODE_VECTOR_FLOAT,       /* VNx12SF */
  MODE_VECTOR_FLOAT,       /* VNx6DF */
  MODE_VECTOR_FLOAT,       /* VNx32HF */
  MODE_VECTOR_FLOAT,       /* VNx16SF */
  MODE_VECTOR_FLOAT,       /* VNx8DF */
};

poly_uint16_pod mode_precision[NUM_MACHINE_MODES] = 
{
  { 0, 0 },                /* VOID */
  { 0, 0 },                /* BLK */
  { 4 * BITS_PER_UNIT, 0 },/* CC */
  { 4 * BITS_PER_UNIT, 0 },/* CCFP */
  { 4 * BITS_PER_UNIT, 0 },/* CCFPE */
  { 4 * BITS_PER_UNIT, 0 },/* CC_SWP */
  { 4 * BITS_PER_UNIT, 0 },/* CC_NZ */
  { 4 * BITS_PER_UNIT, 0 },/* CC_Z */
  { 4 * BITS_PER_UNIT, 0 },/* CC_C */
  { 4 * BITS_PER_UNIT, 0 },/* CC_ADC */
  { 4 * BITS_PER_UNIT, 0 },/* CC_V */
  { 1, 0 },                /* BI */
  { 1 * BITS_PER_UNIT, 0 },/* QI */
  { 2 * BITS_PER_UNIT, 0 },/* HI */
  { 4 * BITS_PER_UNIT, 0 },/* SI */
  { 8 * BITS_PER_UNIT, 0 },/* DI */
  { 16 * BITS_PER_UNIT, 0 }, /* TI */
  { 32 * BITS_PER_UNIT, 0 }, /* OI */
  { 48 * BITS_PER_UNIT, 0 }, /* CI */
  { 64 * BITS_PER_UNIT, 0 }, /* XI */
  { 1 * BITS_PER_UNIT, 0 },/* QQ */
  { 2 * BITS_PER_UNIT, 0 },/* HQ */
  { 4 * BITS_PER_UNIT, 0 },/* SQ */
  { 8 * BITS_PER_UNIT, 0 },/* DQ */
  { 16 * BITS_PER_UNIT, 0 }, /* TQ */
  { 1 * BITS_PER_UNIT, 0 },/* UQQ */
  { 2 * BITS_PER_UNIT, 0 },/* UHQ */
  { 4 * BITS_PER_UNIT, 0 },/* USQ */
  { 8 * BITS_PER_UNIT, 0 },/* UDQ */
  { 16 * BITS_PER_UNIT, 0 }, /* UTQ */
  { 2 * BITS_PER_UNIT, 0 },/* HA */
  { 4 * BITS_PER_UNIT, 0 },/* SA */
  { 8 * BITS_PER_UNIT, 0 },/* DA */
  { 16 * BITS_PER_UNIT, 0 }, /* TA */
  { 2 * BITS_PER_UNIT, 0 },/* UHA */
  { 4 * BITS_PER_UNIT, 0 },/* USA */
  { 8 * BITS_PER_UNIT, 0 },/* UDA */
  { 16 * BITS_PER_UNIT, 0 }, /* UTA */
  { 2 * BITS_PER_UNIT, 0 },/* HF */
  { 4 * BITS_PER_UNIT, 0 },/* SF */
  { 8 * BITS_PER_UNIT, 0 },/* DF */
  { 16 * BITS_PER_UNIT, 0 }, /* TF */
  { 4 * BITS_PER_UNIT, 0 },/* SD */
  { 8 * BITS_PER_UNIT, 0 },/* DD */
  { 16 * BITS_PER_UNIT, 0 }, /* TD */
  { 2 * BITS_PER_UNIT, 0 },/* CQI */
  { 4 * BITS_PER_UNIT, 0 },/* CHI */
  { 8 * BITS_PER_UNIT, 0 },/* CSI */
  { 16 * BITS_PER_UNIT, 0 }, /* CDI */
  { 32 * BITS_PER_UNIT, 0 }, /* CTI */
  { 64 * BITS_PER_UNIT, 0 }, /* COI */
  { 96 * BITS_PER_UNIT, 0 }, /* CCI */
  { 128 * BITS_PER_UNIT, 0 },  /* CXI */
  { 4 * BITS_PER_UNIT, 0 },/* HC */
  { 8 * BITS_PER_UNIT, 0 },/* SC */
  { 16 * BITS_PER_UNIT, 0 }, /* DC */
  { 32 * BITS_PER_UNIT, 0 }, /* TC */
  { 2 * BITS_PER_UNIT, 0 },/* VNx16BI */
  { 2 * BITS_PER_UNIT, 0 },/* VNx8BI */
  { 2 * BITS_PER_UNIT, 0 },/* VNx4BI */
  { 2 * BITS_PER_UNIT, 0 },/* VNx2BI */
  { 8 * BITS_PER_UNIT, 0 },/* V8QI */
  { 8 * BITS_PER_UNIT, 0 },/* V4HI */
  { 8 * BITS_PER_UNIT, 0 },/* V2SI */
  { 16 * BITS_PER_UNIT, 0 }, /* V16QI */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx16QI */
  { 16 * BITS_PER_UNIT, 0 }, /* V8HI */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx8HI */
  { 16 * BITS_PER_UNIT, 0 }, /* V4SI */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx4SI */
  { 16 * BITS_PER_UNIT, 0 }, /* V2DI */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx2DI */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx32QI */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx16HI */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx8SI */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx4DI */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx2TI */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx48QI */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx24HI */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx12SI */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx6DI */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx3TI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx64QI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx32HI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx16SI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx8DI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx4TI */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx2OI */
  { 4 * BITS_PER_UNIT, 0 },/* V2HF */
  { 8 * BITS_PER_UNIT, 0 },/* V4HF */
  { 8 * BITS_PER_UNIT, 0 },/* V2SF */
  { 8 * BITS_PER_UNIT, 0 },/* V1DF */
  { 16 * BITS_PER_UNIT, 0 }, /* V8HF */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx8HF */
  { 16 * BITS_PER_UNIT, 0 }, /* V4SF */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx4SF */
  { 16 * BITS_PER_UNIT, 0 }, /* V2DF */
  { 16 * BITS_PER_UNIT, 0 }, /* VNx2DF */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx16HF */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx8SF */
  { 32 * BITS_PER_UNIT, 0 }, /* VNx4DF */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx24HF */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx12SF */
  { 48 * BITS_PER_UNIT, 0 }, /* VNx6DF */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx32HF */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx16SF */
  { 64 * BITS_PER_UNIT, 0 }, /* VNx8DF */
};

poly_uint16_pod mode_size[NUM_MACHINE_MODES] = 
{
  { 0, 0 },                /* VOID */
  { 0, 0 },                /* BLK */
  { 4, 0 },                /* CC */
  { 4, 0 },                /* CCFP */
  { 4, 0 },                /* CCFPE */
  { 4, 0 },                /* CC_SWP */
  { 4, 0 },                /* CC_NZ */
  { 4, 0 },                /* CC_Z */
  { 4, 0 },                /* CC_C */
  { 4, 0 },                /* CC_ADC */
  { 4, 0 },                /* CC_V */
  { 1, 0 },                /* BI */
  { 1, 0 },                /* QI */
  { 2, 0 },                /* HI */
  { 4, 0 },                /* SI */
  { 8, 0 },                /* DI */
  { 16, 0 },               /* TI */
  { 32, 0 },               /* OI */
  { 48, 0 },               /* CI */
  { 64, 0 },               /* XI */
  { 1, 0 },                /* QQ */
  { 2, 0 },                /* HQ */
  { 4, 0 },                /* SQ */
  { 8, 0 },                /* DQ */
  { 16, 0 },               /* TQ */
  { 1, 0 },                /* UQQ */
  { 2, 0 },                /* UHQ */
  { 4, 0 },                /* USQ */
  { 8, 0 },                /* UDQ */
  { 16, 0 },               /* UTQ */
  { 2, 0 },                /* HA */
  { 4, 0 },                /* SA */
  { 8, 0 },                /* DA */
  { 16, 0 },               /* TA */
  { 2, 0 },                /* UHA */
  { 4, 0 },                /* USA */
  { 8, 0 },                /* UDA */
  { 16, 0 },               /* UTA */
  { 2, 0 },                /* HF */
  { 4, 0 },                /* SF */
  { 8, 0 },                /* DF */
  { 16, 0 },               /* TF */
  { 4, 0 },                /* SD */
  { 8, 0 },                /* DD */
  { 16, 0 },               /* TD */
  { 2, 0 },                /* CQI */
  { 4, 0 },                /* CHI */
  { 8, 0 },                /* CSI */
  { 16, 0 },               /* CDI */
  { 32, 0 },               /* CTI */
  { 64, 0 },               /* COI */
  { 96, 0 },               /* CCI */
  { 128, 0 },              /* CXI */
  { 4, 0 },                /* HC */
  { 8, 0 },                /* SC */
  { 16, 0 },               /* DC */
  { 32, 0 },               /* TC */
  { 2, 0 },                /* VNx16BI */
  { 2, 0 },                /* VNx8BI */
  { 2, 0 },                /* VNx4BI */
  { 2, 0 },                /* VNx2BI */
  { 8, 0 },                /* V8QI */
  { 8, 0 },                /* V4HI */
  { 8, 0 },                /* V2SI */
  { 16, 0 },               /* V16QI */
  { 16, 0 },               /* VNx16QI */
  { 16, 0 },               /* V8HI */
  { 16, 0 },               /* VNx8HI */
  { 16, 0 },               /* V4SI */
  { 16, 0 },               /* VNx4SI */
  { 16, 0 },               /* V2DI */
  { 16, 0 },               /* VNx2DI */
  { 32, 0 },               /* VNx32QI */
  { 32, 0 },               /* VNx16HI */
  { 32, 0 },               /* VNx8SI */
  { 32, 0 },               /* VNx4DI */
  { 32, 0 },               /* VNx2TI */
  { 48, 0 },               /* VNx48QI */
  { 48, 0 },               /* VNx24HI */
  { 48, 0 },               /* VNx12SI */
  { 48, 0 },               /* VNx6DI */
  { 48, 0 },               /* VNx3TI */
  { 64, 0 },               /* VNx64QI */
  { 64, 0 },               /* VNx32HI */
  { 64, 0 },               /* VNx16SI */
  { 64, 0 },               /* VNx8DI */
  { 64, 0 },               /* VNx4TI */
  { 64, 0 },               /* VNx2OI */
  { 4, 0 },                /* V2HF */
  { 8, 0 },                /* V4HF */
  { 8, 0 },                /* V2SF */
  { 8, 0 },                /* V1DF */
  { 16, 0 },               /* V8HF */
  { 16, 0 },               /* VNx8HF */
  { 16, 0 },               /* V4SF */
  { 16, 0 },               /* VNx4SF */
  { 16, 0 },               /* V2DF */
  { 16, 0 },               /* VNx2DF */
  { 32, 0 },               /* VNx16HF */
  { 32, 0 },               /* VNx8SF */
  { 32, 0 },               /* VNx4DF */
  { 48, 0 },               /* VNx24HF */
  { 48, 0 },               /* VNx12SF */
  { 48, 0 },               /* VNx6DF */
  { 64, 0 },               /* VNx32HF */
  { 64, 0 },               /* VNx16SF */
  { 64, 0 },               /* VNx8DF */
};

poly_uint16_pod mode_nunits[NUM_MACHINE_MODES] = 
{
  { 0, 0 },                /* VOID */
  { 0, 0 },                /* BLK */
  { 1, 0 },                /* CC */
  { 1, 0 },                /* CCFP */
  { 1, 0 },                /* CCFPE */
  { 1, 0 },                /* CC_SWP */
  { 1, 0 },                /* CC_NZ */
  { 1, 0 },                /* CC_Z */
  { 1, 0 },                /* CC_C */
  { 1, 0 },                /* CC_ADC */
  { 1, 0 },                /* CC_V */
  { 1, 0 },                /* BI */
  { 1, 0 },                /* QI */
  { 1, 0 },                /* HI */
  { 1, 0 },                /* SI */
  { 1, 0 },                /* DI */
  { 1, 0 },                /* TI */
  { 1, 0 },                /* OI */
  { 1, 0 },                /* CI */
  { 1, 0 },                /* XI */
  { 1, 0 },                /* QQ */
  { 1, 0 },                /* HQ */
  { 1, 0 },                /* SQ */
  { 1, 0 },                /* DQ */
  { 1, 0 },                /* TQ */
  { 1, 0 },                /* UQQ */
  { 1, 0 },                /* UHQ */
  { 1, 0 },                /* USQ */
  { 1, 0 },                /* UDQ */
  { 1, 0 },                /* UTQ */
  { 1, 0 },                /* HA */
  { 1, 0 },                /* SA */
  { 1, 0 },                /* DA */
  { 1, 0 },                /* TA */
  { 1, 0 },                /* UHA */
  { 1, 0 },                /* USA */
  { 1, 0 },                /* UDA */
  { 1, 0 },                /* UTA */
  { 1, 0 },                /* HF */
  { 1, 0 },                /* SF */
  { 1, 0 },                /* DF */
  { 1, 0 },                /* TF */
  { 1, 0 },                /* SD */
  { 1, 0 },                /* DD */
  { 1, 0 },                /* TD */
  { 2, 0 },                /* CQI */
  { 2, 0 },                /* CHI */
  { 2, 0 },                /* CSI */
  { 2, 0 },                /* CDI */
  { 2, 0 },                /* CTI */
  { 2, 0 },                /* COI */
  { 2, 0 },                /* CCI */
  { 2, 0 },                /* CXI */
  { 2, 0 },                /* HC */
  { 2, 0 },                /* SC */
  { 2, 0 },                /* DC */
  { 2, 0 },                /* TC */
  { 16, 0 },               /* VNx16BI */
  { 8, 0 },                /* VNx8BI */
  { 4, 0 },                /* VNx4BI */
  { 2, 0 },                /* VNx2BI */
  { 8, 0 },                /* V8QI */
  { 4, 0 },                /* V4HI */
  { 2, 0 },                /* V2SI */
  { 16, 0 },               /* V16QI */
  { 16, 0 },               /* VNx16QI */
  { 8, 0 },                /* V8HI */
  { 8, 0 },                /* VNx8HI */
  { 4, 0 },                /* V4SI */
  { 4, 0 },                /* VNx4SI */
  { 2, 0 },                /* V2DI */
  { 2, 0 },                /* VNx2DI */
  { 32, 0 },               /* VNx32QI */
  { 16, 0 },               /* VNx16HI */
  { 8, 0 },                /* VNx8SI */
  { 4, 0 },                /* VNx4DI */
  { 2, 0 },                /* VNx2TI */
  { 48, 0 },               /* VNx48QI */
  { 24, 0 },               /* VNx24HI */
  { 12, 0 },               /* VNx12SI */
  { 6, 0 },                /* VNx6DI */
  { 3, 0 },                /* VNx3TI */
  { 64, 0 },               /* VNx64QI */
  { 32, 0 },               /* VNx32HI */
  { 16, 0 },               /* VNx16SI */
  { 8, 0 },                /* VNx8DI */
  { 4, 0 },                /* VNx4TI */
  { 2, 0 },                /* VNx2OI */
  { 2, 0 },                /* V2HF */
  { 4, 0 },                /* V4HF */
  { 2, 0 },                /* V2SF */
  { 1, 0 },                /* V1DF */
  { 8, 0 },                /* V8HF */
  { 8, 0 },                /* VNx8HF */
  { 4, 0 },                /* V4SF */
  { 4, 0 },                /* VNx4SF */
  { 2, 0 },                /* V2DF */
  { 2, 0 },                /* VNx2DF */
  { 16, 0 },               /* VNx16HF */
  { 8, 0 },                /* VNx8SF */
  { 4, 0 },                /* VNx4DF */
  { 24, 0 },               /* VNx24HF */
  { 12, 0 },               /* VNx12SF */
  { 6, 0 },                /* VNx6DF */
  { 32, 0 },               /* VNx32HF */
  { 16, 0 },               /* VNx16SF */
  { 8, 0 },                /* VNx8DF */
};

const unsigned char mode_wider[NUM_MACHINE_MODES] =
{
  E_VOIDmode,              /* VOID */
  E_VOIDmode,              /* BLK */
  E_VOIDmode,              /* CC */
  E_VOIDmode,              /* CCFP */
  E_VOIDmode,              /* CCFPE */
  E_VOIDmode,              /* CC_SWP */
  E_VOIDmode,              /* CC_NZ */
  E_VOIDmode,              /* CC_Z */
  E_VOIDmode,              /* CC_C */
  E_VOIDmode,              /* CC_ADC */
  E_VOIDmode,              /* CC_V */
  E_QImode,                /* BI */
  E_HImode,                /* QI */
  E_SImode,                /* HI */
  E_DImode,                /* SI */
  E_TImode,                /* DI */
  E_OImode,                /* TI */
  E_CImode,                /* OI */
  E_XImode,                /* CI */
  E_VOIDmode,              /* XI */
  E_HQmode,                /* QQ */
  E_SQmode,                /* HQ */
  E_DQmode,                /* SQ */
  E_TQmode,                /* DQ */
  E_VOIDmode,              /* TQ */
  E_UHQmode,               /* UQQ */
  E_USQmode,               /* UHQ */
  E_UDQmode,               /* USQ */
  E_UTQmode,               /* UDQ */
  E_VOIDmode,              /* UTQ */
  E_SAmode,                /* HA */
  E_DAmode,                /* SA */
  E_TAmode,                /* DA */
  E_VOIDmode,              /* TA */
  E_USAmode,               /* UHA */
  E_UDAmode,               /* USA */
  E_UTAmode,               /* UDA */
  E_VOIDmode,              /* UTA */
  E_SFmode,                /* HF */
  E_DFmode,                /* SF */
  E_TFmode,                /* DF */
  E_VOIDmode,              /* TF */
  E_DDmode,                /* SD */
  E_TDmode,                /* DD */
  E_VOIDmode,              /* TD */
  E_CHImode,               /* CQI */
  E_CSImode,               /* CHI */
  E_CDImode,               /* CSI */
  E_CTImode,               /* CDI */
  E_COImode,               /* CTI */
  E_CCImode,               /* COI */
  E_CXImode,               /* CCI */
  E_VOIDmode,              /* CXI */
  E_SCmode,                /* HC */
  E_DCmode,                /* SC */
  E_TCmode,                /* DC */
  E_VOIDmode,              /* TC */
  E_VNx8BImode,            /* VNx16BI */
  E_VNx4BImode,            /* VNx8BI */
  E_VNx2BImode,            /* VNx4BI */
  E_VOIDmode,              /* VNx2BI */
  E_V4HImode,              /* V8QI */
  E_V2SImode,              /* V4HI */
  E_V16QImode,             /* V2SI */
  E_VNx16QImode,           /* V16QI */
  E_V8HImode,              /* VNx16QI */
  E_VNx8HImode,            /* V8HI */
  E_V4SImode,              /* VNx8HI */
  E_VNx4SImode,            /* V4SI */
  E_V2DImode,              /* VNx4SI */
  E_VNx2DImode,            /* V2DI */
  E_VNx32QImode,           /* VNx2DI */
  E_VNx16HImode,           /* VNx32QI */
  E_VNx8SImode,            /* VNx16HI */
  E_VNx4DImode,            /* VNx8SI */
  E_VNx2TImode,            /* VNx4DI */
  E_VNx48QImode,           /* VNx2TI */
  E_VNx24HImode,           /* VNx48QI */
  E_VNx12SImode,           /* VNx24HI */
  E_VNx6DImode,            /* VNx12SI */
  E_VNx3TImode,            /* VNx6DI */
  E_VNx64QImode,           /* VNx3TI */
  E_VNx32HImode,           /* VNx64QI */
  E_VNx16SImode,           /* VNx32HI */
  E_VNx8DImode,            /* VNx16SI */
  E_VNx4TImode,            /* VNx8DI */
  E_VNx2OImode,            /* VNx4TI */
  E_VOIDmode,              /* VNx2OI */
  E_V4HFmode,              /* V2HF */
  E_V2SFmode,              /* V4HF */
  E_V1DFmode,              /* V2SF */
  E_V8HFmode,              /* V1DF */
  E_VNx8HFmode,            /* V8HF */
  E_V4SFmode,              /* VNx8HF */
  E_VNx4SFmode,            /* V4SF */
  E_V2DFmode,              /* VNx4SF */
  E_VNx2DFmode,            /* V2DF */
  E_VNx16HFmode,           /* VNx2DF */
  E_VNx8SFmode,            /* VNx16HF */
  E_VNx4DFmode,            /* VNx8SF */
  E_VNx24HFmode,           /* VNx4DF */
  E_VNx12SFmode,           /* VNx24HF */
  E_VNx6DFmode,            /* VNx12SF */
  E_VNx32HFmode,           /* VNx6DF */
  E_VNx16SFmode,           /* VNx32HF */
  E_VNx8DFmode,            /* VNx16SF */
  E_VOIDmode,              /* VNx8DF */
};

const unsigned char mode_2xwider[NUM_MACHINE_MODES] =
{
  E_VOIDmode,              /* VOID */
  E_BLKmode,               /* BLK */
  E_VOIDmode,              /* CC */
  E_VOIDmode,              /* CCFP */
  E_VOIDmode,              /* CCFPE */
  E_VOIDmode,              /* CC_SWP */
  E_VOIDmode,              /* CC_NZ */
  E_VOIDmode,              /* CC_Z */
  E_VOIDmode,              /* CC_C */
  E_VOIDmode,              /* CC_ADC */
  E_VOIDmode,              /* CC_V */
  E_VOIDmode,              /* BI */
  E_HImode,                /* QI */
  E_SImode,                /* HI */
  E_DImode,                /* SI */
  E_TImode,                /* DI */
  E_OImode,                /* TI */
  E_XImode,                /* OI */
  E_VOIDmode,              /* CI */
  E_VOIDmode,              /* XI */
  E_HQmode,                /* QQ */
  E_SQmode,                /* HQ */
  E_DQmode,                /* SQ */
  E_TQmode,                /* DQ */
  E_VOIDmode,              /* TQ */
  E_UHQmode,               /* UQQ */
  E_USQmode,               /* UHQ */
  E_UDQmode,               /* USQ */
  E_UTQmode,               /* UDQ */
  E_VOIDmode,              /* UTQ */
  E_SAmode,                /* HA */
  E_DAmode,                /* SA */
  E_TAmode,                /* DA */
  E_VOIDmode,              /* TA */
  E_USAmode,               /* UHA */
  E_UDAmode,               /* USA */
  E_UTAmode,               /* UDA */
  E_VOIDmode,              /* UTA */
  E_SFmode,                /* HF */
  E_DFmode,                /* SF */
  E_TFmode,                /* DF */
  E_VOIDmode,              /* TF */
  E_DDmode,                /* SD */
  E_TDmode,                /* DD */
  E_VOIDmode,              /* TD */
  E_CHImode,               /* CQI */
  E_CSImode,               /* CHI */
  E_CDImode,               /* CSI */
  E_CTImode,               /* CDI */
  E_COImode,               /* CTI */
  E_CXImode,               /* COI */
  E_VOIDmode,              /* CCI */
  E_VOIDmode,              /* CXI */
  E_SCmode,                /* HC */
  E_DCmode,                /* SC */
  E_TCmode,                /* DC */
  E_VOIDmode,              /* TC */
  E_VOIDmode,              /* VNx16BI */
  E_VOIDmode,              /* VNx8BI */
  E_VOIDmode,              /* VNx4BI */
  E_VOIDmode,              /* VNx2BI */
  E_V16QImode,             /* V8QI */
  E_V8HImode,              /* V4HI */
  E_V4SImode,              /* V2SI */
  E_VNx32QImode,           /* V16QI */
  E_VNx32QImode,           /* VNx16QI */
  E_VNx16HImode,           /* V8HI */
  E_VNx16HImode,           /* VNx8HI */
  E_VNx8SImode,            /* V4SI */
  E_VNx8SImode,            /* VNx4SI */
  E_VNx4DImode,            /* V2DI */
  E_VNx4DImode,            /* VNx2DI */
  E_VNx64QImode,           /* VNx32QI */
  E_VNx32HImode,           /* VNx16HI */
  E_VNx16SImode,           /* VNx8SI */
  E_VNx8DImode,            /* VNx4DI */
  E_VNx4TImode,            /* VNx2TI */
  E_VOIDmode,              /* VNx48QI */
  E_VOIDmode,              /* VNx24HI */
  E_VOIDmode,              /* VNx12SI */
  E_VOIDmode,              /* VNx6DI */
  E_VOIDmode,              /* VNx3TI */
  E_VOIDmode,              /* VNx64QI */
  E_VOIDmode,              /* VNx32HI */
  E_VOIDmode,              /* VNx16SI */
  E_VOIDmode,              /* VNx8DI */
  E_VOIDmode,              /* VNx4TI */
  E_VOIDmode,              /* VNx2OI */
  E_V4HFmode,              /* V2HF */
  E_V8HFmode,              /* V4HF */
  E_V4SFmode,              /* V2SF */
  E_V2DFmode,              /* V1DF */
  E_VNx16HFmode,           /* V8HF */
  E_VNx16HFmode,           /* VNx8HF */
  E_VNx8SFmode,            /* V4SF */
  E_VNx8SFmode,            /* VNx4SF */
  E_VNx4DFmode,            /* V2DF */
  E_VNx4DFmode,            /* VNx2DF */
  E_VNx32HFmode,           /* VNx16HF */
  E_VNx16SFmode,           /* VNx8SF */
  E_VNx8DFmode,            /* VNx4DF */
  E_VOIDmode,              /* VNx24HF */
  E_VOIDmode,              /* VNx12SF */
  E_VOIDmode,              /* VNx6DF */
  E_VOIDmode,              /* VNx32HF */
  E_VOIDmode,              /* VNx16SF */
  E_VOIDmode,              /* VNx8DF */
};

const unsigned char mode_complex[NUM_MACHINE_MODES] =
{
  E_VOIDmode,              /* VOID */
  E_VOIDmode,              /* BLK */
  E_VOIDmode,              /* CC */
  E_VOIDmode,              /* CCFP */
  E_VOIDmode,              /* CCFPE */
  E_VOIDmode,              /* CC_SWP */
  E_VOIDmode,              /* CC_NZ */
  E_VOIDmode,              /* CC_Z */
  E_VOIDmode,              /* CC_C */
  E_VOIDmode,              /* CC_ADC */
  E_VOIDmode,              /* CC_V */
  E_VOIDmode,              /* BI */
  E_CQImode,               /* QI */
  E_CHImode,               /* HI */
  E_CSImode,               /* SI */
  E_CDImode,               /* DI */
  E_CTImode,               /* TI */
  E_COImode,               /* OI */
  E_CCImode,               /* CI */
  E_CXImode,               /* XI */
  E_VOIDmode,              /* QQ */
  E_VOIDmode,              /* HQ */
  E_VOIDmode,              /* SQ */
  E_VOIDmode,              /* DQ */
  E_VOIDmode,              /* TQ */
  E_VOIDmode,              /* UQQ */
  E_VOIDmode,              /* UHQ */
  E_VOIDmode,              /* USQ */
  E_VOIDmode,              /* UDQ */
  E_VOIDmode,              /* UTQ */
  E_VOIDmode,              /* HA */
  E_VOIDmode,              /* SA */
  E_VOIDmode,              /* DA */
  E_VOIDmode,              /* TA */
  E_VOIDmode,              /* UHA */
  E_VOIDmode,              /* USA */
  E_VOIDmode,              /* UDA */
  E_VOIDmode,              /* UTA */
  E_HCmode,                /* HF */
  E_SCmode,                /* SF */
  E_DCmode,                /* DF */
  E_TCmode,                /* TF */
  E_VOIDmode,              /* SD */
  E_VOIDmode,              /* DD */
  E_VOIDmode,              /* TD */
  E_VOIDmode,              /* CQI */
  E_VOIDmode,              /* CHI */
  E_VOIDmode,              /* CSI */
  E_VOIDmode,              /* CDI */
  E_VOIDmode,              /* CTI */
  E_VOIDmode,              /* COI */
  E_VOIDmode,              /* CCI */
  E_VOIDmode,              /* CXI */
  E_VOIDmode,              /* HC */
  E_VOIDmode,              /* SC */
  E_VOIDmode,              /* DC */
  E_VOIDmode,              /* TC */
  E_VOIDmode,              /* VNx16BI */
  E_VOIDmode,              /* VNx8BI */
  E_VOIDmode,              /* VNx4BI */
  E_VOIDmode,              /* VNx2BI */
  E_VOIDmode,              /* V8QI */
  E_VOIDmode,              /* V4HI */
  E_VOIDmode,              /* V2SI */
  E_VOIDmode,              /* V16QI */
  E_VOIDmode,              /* VNx16QI */
  E_VOIDmode,              /* V8HI */
  E_VOIDmode,              /* VNx8HI */
  E_VOIDmode,              /* V4SI */
  E_VOIDmode,              /* VNx4SI */
  E_VOIDmode,              /* V2DI */
  E_VOIDmode,              /* VNx2DI */
  E_VOIDmode,              /* VNx32QI */
  E_VOIDmode,              /* VNx16HI */
  E_VOIDmode,              /* VNx8SI */
  E_VOIDmode,              /* VNx4DI */
  E_VOIDmode,              /* VNx2TI */
  E_VOIDmode,              /* VNx48QI */
  E_VOIDmode,              /* VNx24HI */
  E_VOIDmode,              /* VNx12SI */
  E_VOIDmode,              /* VNx6DI */
  E_VOIDmode,              /* VNx3TI */
  E_VOIDmode,              /* VNx64QI */
  E_VOIDmode,              /* VNx32HI */
  E_VOIDmode,              /* VNx16SI */
  E_VOIDmode,              /* VNx8DI */
  E_VOIDmode,              /* VNx4TI */
  E_VOIDmode,              /* VNx2OI */
  E_VOIDmode,              /* V2HF */
  E_VOIDmode,              /* V4HF */
  E_VOIDmode,              /* V2SF */
  E_VOIDmode,              /* V1DF */
  E_VOIDmode,              /* V8HF */
  E_VOIDmode,              /* VNx8HF */
  E_VOIDmode,              /* V4SF */
  E_VOIDmode,              /* VNx4SF */
  E_VOIDmode,              /* V2DF */
  E_VOIDmode,              /* VNx2DF */
  E_VOIDmode,              /* VNx16HF */
  E_VOIDmode,              /* VNx8SF */
  E_VOIDmode,              /* VNx4DF */
  E_VOIDmode,              /* VNx24HF */
  E_VOIDmode,              /* VNx12SF */
  E_VOIDmode,              /* VNx6DF */
  E_VOIDmode,              /* VNx32HF */
  E_VOIDmode,              /* VNx16SF */
  E_VOIDmode,              /* VNx8DF */
};

const unsigned HOST_WIDE_INT mode_mask_array[NUM_MACHINE_MODES] =
{
#define MODE_MASK(m)                          \
  ((m) >= HOST_BITS_PER_WIDE_INT)             \
   ? HOST_WIDE_INT_M1U                        \
   : (HOST_WIDE_INT_1U << (m)) - 1

  MODE_MASK (0),           /* VOID */
  MODE_MASK (0),           /* BLK */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC */
  MODE_MASK (4*BITS_PER_UNIT),   /* CCFP */
  MODE_MASK (4*BITS_PER_UNIT),   /* CCFPE */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_SWP */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_NZ */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_Z */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_C */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_ADC */
  MODE_MASK (4*BITS_PER_UNIT),   /* CC_V */
  MODE_MASK (1),           /* BI */
  MODE_MASK (1*BITS_PER_UNIT),   /* QI */
  MODE_MASK (2*BITS_PER_UNIT),   /* HI */
  MODE_MASK (4*BITS_PER_UNIT),   /* SI */
  MODE_MASK (8*BITS_PER_UNIT),   /* DI */
  MODE_MASK (16*BITS_PER_UNIT),    /* TI */
  MODE_MASK (32*BITS_PER_UNIT),    /* OI */
  MODE_MASK (48*BITS_PER_UNIT),    /* CI */
  MODE_MASK (64*BITS_PER_UNIT),    /* XI */
  MODE_MASK (1*BITS_PER_UNIT),   /* QQ */
  MODE_MASK (2*BITS_PER_UNIT),   /* HQ */
  MODE_MASK (4*BITS_PER_UNIT),   /* SQ */
  MODE_MASK (8*BITS_PER_UNIT),   /* DQ */
  MODE_MASK (16*BITS_PER_UNIT),    /* TQ */
  MODE_MASK (1*BITS_PER_UNIT),   /* UQQ */
  MODE_MASK (2*BITS_PER_UNIT),   /* UHQ */
  MODE_MASK (4*BITS_PER_UNIT),   /* USQ */
  MODE_MASK (8*BITS_PER_UNIT),   /* UDQ */
  MODE_MASK (16*BITS_PER_UNIT),    /* UTQ */
  MODE_MASK (2*BITS_PER_UNIT),   /* HA */
  MODE_MASK (4*BITS_PER_UNIT),   /* SA */
  MODE_MASK (8*BITS_PER_UNIT),   /* DA */
  MODE_MASK (16*BITS_PER_UNIT),    /* TA */
  MODE_MASK (2*BITS_PER_UNIT),   /* UHA */
  MODE_MASK (4*BITS_PER_UNIT),   /* USA */
  MODE_MASK (8*BITS_PER_UNIT),   /* UDA */
  MODE_MASK (16*BITS_PER_UNIT),    /* UTA */
  MODE_MASK (2*BITS_PER_UNIT),   /* HF */
  MODE_MASK (4*BITS_PER_UNIT),   /* SF */
  MODE_MASK (8*BITS_PER_UNIT),   /* DF */
  MODE_MASK (16*BITS_PER_UNIT),    /* TF */
  MODE_MASK (4*BITS_PER_UNIT),   /* SD */
  MODE_MASK (8*BITS_PER_UNIT),   /* DD */
  MODE_MASK (16*BITS_PER_UNIT),    /* TD */
  MODE_MASK (2*BITS_PER_UNIT),   /* CQI */
  MODE_MASK (4*BITS_PER_UNIT),   /* CHI */
  MODE_MASK (8*BITS_PER_UNIT),   /* CSI */
  MODE_MASK (16*BITS_PER_UNIT),    /* CDI */
  MODE_MASK (32*BITS_PER_UNIT),    /* CTI */
  MODE_MASK (64*BITS_PER_UNIT),    /* COI */
  MODE_MASK (96*BITS_PER_UNIT),    /* CCI */
  MODE_MASK (128*BITS_PER_UNIT),     /* CXI */
  MODE_MASK (4*BITS_PER_UNIT),   /* HC */
  MODE_MASK (8*BITS_PER_UNIT),   /* SC */
  MODE_MASK (16*BITS_PER_UNIT),    /* DC */
  MODE_MASK (32*BITS_PER_UNIT),    /* TC */
  MODE_MASK (2*BITS_PER_UNIT),   /* VNx16BI */
  MODE_MASK (2*BITS_PER_UNIT),   /* VNx8BI */
  MODE_MASK (2*BITS_PER_UNIT),   /* VNx4BI */
  MODE_MASK (2*BITS_PER_UNIT),   /* VNx2BI */
  MODE_MASK (8*BITS_PER_UNIT),   /* V8QI */
  MODE_MASK (8*BITS_PER_UNIT),   /* V4HI */
  MODE_MASK (8*BITS_PER_UNIT),   /* V2SI */
  MODE_MASK (16*BITS_PER_UNIT),    /* V16QI */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx16QI */
  MODE_MASK (16*BITS_PER_UNIT),    /* V8HI */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx8HI */
  MODE_MASK (16*BITS_PER_UNIT),    /* V4SI */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx4SI */
  MODE_MASK (16*BITS_PER_UNIT),    /* V2DI */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx2DI */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx32QI */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx16HI */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx8SI */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx4DI */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx2TI */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx48QI */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx24HI */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx12SI */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx6DI */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx3TI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx64QI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx32HI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx16SI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx8DI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx4TI */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx2OI */
  MODE_MASK (4*BITS_PER_UNIT),   /* V2HF */
  MODE_MASK (8*BITS_PER_UNIT),   /* V4HF */
  MODE_MASK (8*BITS_PER_UNIT),   /* V2SF */
  MODE_MASK (8*BITS_PER_UNIT),   /* V1DF */
  MODE_MASK (16*BITS_PER_UNIT),    /* V8HF */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx8HF */
  MODE_MASK (16*BITS_PER_UNIT),    /* V4SF */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx4SF */
  MODE_MASK (16*BITS_PER_UNIT),    /* V2DF */
  MODE_MASK (16*BITS_PER_UNIT),    /* VNx2DF */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx16HF */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx8SF */
  MODE_MASK (32*BITS_PER_UNIT),    /* VNx4DF */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx24HF */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx12SF */
  MODE_MASK (48*BITS_PER_UNIT),    /* VNx6DF */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx32HF */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx16SF */
  MODE_MASK (64*BITS_PER_UNIT),    /* VNx8DF */
#undef MODE_MASK
};

const unsigned char mode_inner[NUM_MACHINE_MODES] =
{
  E_VOIDmode,              /* VOID */
  E_BLKmode,               /* BLK */
  E_CCmode,                /* CC */
  E_CCFPmode,              /* CCFP */
  E_CCFPEmode,             /* CCFPE */
  E_CC_SWPmode,            /* CC_SWP */
  E_CC_NZmode,             /* CC_NZ */
  E_CC_Zmode,              /* CC_Z */
  E_CC_Cmode,              /* CC_C */
  E_CC_ADCmode,            /* CC_ADC */
  E_CC_Vmode,              /* CC_V */
  E_BImode,                /* BI */
  E_QImode,                /* QI */
  E_HImode,                /* HI */
  E_SImode,                /* SI */
  E_DImode,                /* DI */
  E_TImode,                /* TI */
  E_OImode,                /* OI */
  E_CImode,                /* CI */
  E_XImode,                /* XI */
  E_QQmode,                /* QQ */
  E_HQmode,                /* HQ */
  E_SQmode,                /* SQ */
  E_DQmode,                /* DQ */
  E_TQmode,                /* TQ */
  E_UQQmode,               /* UQQ */
  E_UHQmode,               /* UHQ */
  E_USQmode,               /* USQ */
  E_UDQmode,               /* UDQ */
  E_UTQmode,               /* UTQ */
  E_HAmode,                /* HA */
  E_SAmode,                /* SA */
  E_DAmode,                /* DA */
  E_TAmode,                /* TA */
  E_UHAmode,               /* UHA */
  E_USAmode,               /* USA */
  E_UDAmode,               /* UDA */
  E_UTAmode,               /* UTA */
  E_HFmode,                /* HF */
  E_SFmode,                /* SF */
  E_DFmode,                /* DF */
  E_TFmode,                /* TF */
  E_SDmode,                /* SD */
  E_DDmode,                /* DD */
  E_TDmode,                /* TD */
  E_QImode,                /* CQI */
  E_HImode,                /* CHI */
  E_SImode,                /* CSI */
  E_DImode,                /* CDI */
  E_TImode,                /* CTI */
  E_OImode,                /* COI */
  E_CImode,                /* CCI */
  E_XImode,                /* CXI */
  E_HFmode,                /* HC */
  E_SFmode,                /* SC */
  E_DFmode,                /* DC */
  E_TFmode,                /* TC */
  E_BImode,                /* VNx16BI */
  E_BImode,                /* VNx8BI */
  E_BImode,                /* VNx4BI */
  E_BImode,                /* VNx2BI */
  E_QImode,                /* V8QI */
  E_HImode,                /* V4HI */
  E_SImode,                /* V2SI */
  E_QImode,                /* V16QI */
  E_QImode,                /* VNx16QI */
  E_HImode,                /* V8HI */
  E_HImode,                /* VNx8HI */
  E_SImode,                /* V4SI */
  E_SImode,                /* VNx4SI */
  E_DImode,                /* V2DI */
  E_DImode,                /* VNx2DI */
  E_QImode,                /* VNx32QI */
  E_HImode,                /* VNx16HI */
  E_SImode,                /* VNx8SI */
  E_DImode,                /* VNx4DI */
  E_TImode,                /* VNx2TI */
  E_QImode,                /* VNx48QI */
  E_HImode,                /* VNx24HI */
  E_SImode,                /* VNx12SI */
  E_DImode,                /* VNx6DI */
  E_TImode,                /* VNx3TI */
  E_QImode,                /* VNx64QI */
  E_HImode,                /* VNx32HI */
  E_SImode,                /* VNx16SI */
  E_DImode,                /* VNx8DI */
  E_TImode,                /* VNx4TI */
  E_OImode,                /* VNx2OI */
  E_HFmode,                /* V2HF */
  E_HFmode,                /* V4HF */
  E_SFmode,                /* V2SF */
  E_DFmode,                /* V1DF */
  E_HFmode,                /* V8HF */
  E_HFmode,                /* VNx8HF */
  E_SFmode,                /* V4SF */
  E_SFmode,                /* VNx4SF */
  E_DFmode,                /* V2DF */
  E_DFmode,                /* VNx2DF */
  E_HFmode,                /* VNx16HF */
  E_SFmode,                /* VNx8SF */
  E_DFmode,                /* VNx4DF */
  E_HFmode,                /* VNx24HF */
  E_SFmode,                /* VNx12SF */
  E_DFmode,                /* VNx6DF */
  E_HFmode,                /* VNx32HF */
  E_SFmode,                /* VNx16SF */
  E_DFmode,                /* VNx8DF */
};

const unsigned char mode_unit_size[NUM_MACHINE_MODES] = 
{
  0,                       /* VOID */
  0,                       /* BLK */
  4,                       /* CC */
  4,                       /* CCFP */
  4,                       /* CCFPE */
  4,                       /* CC_SWP */
  4,                       /* CC_NZ */
  4,                       /* CC_Z */
  4,                       /* CC_C */
  4,                       /* CC_ADC */
  4,                       /* CC_V */
  1,                       /* BI */
  1,                       /* QI */
  2,                       /* HI */
  4,                       /* SI */
  8,                       /* DI */
  16,                      /* TI */
  32,                      /* OI */
  48,                      /* CI */
  64,                      /* XI */
  1,                       /* QQ */
  2,                       /* HQ */
  4,                       /* SQ */
  8,                       /* DQ */
  16,                      /* TQ */
  1,                       /* UQQ */
  2,                       /* UHQ */
  4,                       /* USQ */
  8,                       /* UDQ */
  16,                      /* UTQ */
  2,                       /* HA */
  4,                       /* SA */
  8,                       /* DA */
  16,                      /* TA */
  2,                       /* UHA */
  4,                       /* USA */
  8,                       /* UDA */
  16,                      /* UTA */
  2,                       /* HF */
  4,                       /* SF */
  8,                       /* DF */
  16,                      /* TF */
  4,                       /* SD */
  8,                       /* DD */
  16,                      /* TD */
  1,                       /* CQI */
  2,                       /* CHI */
  4,                       /* CSI */
  8,                       /* CDI */
  16,                      /* CTI */
  32,                      /* COI */
  48,                      /* CCI */
  64,                      /* CXI */
  2,                       /* HC */
  4,                       /* SC */
  8,                       /* DC */
  16,                      /* TC */
  1,                       /* VNx16BI */
  1,                       /* VNx8BI */
  1,                       /* VNx4BI */
  1,                       /* VNx2BI */
  1,                       /* V8QI */
  2,                       /* V4HI */
  4,                       /* V2SI */
  1,                       /* V16QI */
  1,                       /* VNx16QI */
  2,                       /* V8HI */
  2,                       /* VNx8HI */
  4,                       /* V4SI */
  4,                       /* VNx4SI */
  8,                       /* V2DI */
  8,                       /* VNx2DI */
  1,                       /* VNx32QI */
  2,                       /* VNx16HI */
  4,                       /* VNx8SI */
  8,                       /* VNx4DI */
  16,                      /* VNx2TI */
  1,                       /* VNx48QI */
  2,                       /* VNx24HI */
  4,                       /* VNx12SI */
  8,                       /* VNx6DI */
  16,                      /* VNx3TI */
  1,                       /* VNx64QI */
  2,                       /* VNx32HI */
  4,                       /* VNx16SI */
  8,                       /* VNx8DI */
  16,                      /* VNx4TI */
  32,                      /* VNx2OI */
  2,                       /* V2HF */
  2,                       /* V4HF */
  4,                       /* V2SF */
  8,                       /* V1DF */
  2,                       /* V8HF */
  2,                       /* VNx8HF */
  4,                       /* V4SF */
  4,                       /* VNx4SF */
  8,                       /* V2DF */
  8,                       /* VNx2DF */
  2,                       /* VNx16HF */
  4,                       /* VNx8SF */
  8,                       /* VNx4DF */
  2,                       /* VNx24HF */
  4,                       /* VNx12SF */
  8,                       /* VNx6DF */
  2,                       /* VNx32HF */
  4,                       /* VNx16SF */
  8,                       /* VNx8DF */
};

const unsigned short mode_unit_precision[NUM_MACHINE_MODES] =
{
  0,                       /* VOID */
  0,                       /* BLK */
  4*BITS_PER_UNIT,         /* CC */
  4*BITS_PER_UNIT,         /* CCFP */
  4*BITS_PER_UNIT,         /* CCFPE */
  4*BITS_PER_UNIT,         /* CC_SWP */
  4*BITS_PER_UNIT,         /* CC_NZ */
  4*BITS_PER_UNIT,         /* CC_Z */
  4*BITS_PER_UNIT,         /* CC_C */
  4*BITS_PER_UNIT,         /* CC_ADC */
  4*BITS_PER_UNIT,         /* CC_V */
  1,                       /* BI */
  1*BITS_PER_UNIT,         /* QI */
  2*BITS_PER_UNIT,         /* HI */
  4*BITS_PER_UNIT,         /* SI */
  8*BITS_PER_UNIT,         /* DI */
  16*BITS_PER_UNIT,        /* TI */
  32*BITS_PER_UNIT,        /* OI */
  48*BITS_PER_UNIT,        /* CI */
  64*BITS_PER_UNIT,        /* XI */
  1*BITS_PER_UNIT,         /* QQ */
  2*BITS_PER_UNIT,         /* HQ */
  4*BITS_PER_UNIT,         /* SQ */
  8*BITS_PER_UNIT,         /* DQ */
  16*BITS_PER_UNIT,        /* TQ */
  1*BITS_PER_UNIT,         /* UQQ */
  2*BITS_PER_UNIT,         /* UHQ */
  4*BITS_PER_UNIT,         /* USQ */
  8*BITS_PER_UNIT,         /* UDQ */
  16*BITS_PER_UNIT,        /* UTQ */
  2*BITS_PER_UNIT,         /* HA */
  4*BITS_PER_UNIT,         /* SA */
  8*BITS_PER_UNIT,         /* DA */
  16*BITS_PER_UNIT,        /* TA */
  2*BITS_PER_UNIT,         /* UHA */
  4*BITS_PER_UNIT,         /* USA */
  8*BITS_PER_UNIT,         /* UDA */
  16*BITS_PER_UNIT,        /* UTA */
  2*BITS_PER_UNIT,         /* HF */
  4*BITS_PER_UNIT,         /* SF */
  8*BITS_PER_UNIT,         /* DF */
  16*BITS_PER_UNIT,        /* TF */
  4*BITS_PER_UNIT,         /* SD */
  8*BITS_PER_UNIT,         /* DD */
  16*BITS_PER_UNIT,        /* TD */
  1*BITS_PER_UNIT,         /* CQI */
  2*BITS_PER_UNIT,         /* CHI */
  4*BITS_PER_UNIT,         /* CSI */
  8*BITS_PER_UNIT,         /* CDI */
  16*BITS_PER_UNIT,        /* CTI */
  32*BITS_PER_UNIT,        /* COI */
  48*BITS_PER_UNIT,        /* CCI */
  64*BITS_PER_UNIT,        /* CXI */
  2*BITS_PER_UNIT,         /* HC */
  4*BITS_PER_UNIT,         /* SC */
  8*BITS_PER_UNIT,         /* DC */
  16*BITS_PER_UNIT,        /* TC */
  1,                       /* VNx16BI */
  1,                       /* VNx8BI */
  1,                       /* VNx4BI */
  1,                       /* VNx2BI */
  1*BITS_PER_UNIT,         /* V8QI */
  2*BITS_PER_UNIT,         /* V4HI */
  4*BITS_PER_UNIT,         /* V2SI */
  1*BITS_PER_UNIT,         /* V16QI */
  1*BITS_PER_UNIT,         /* VNx16QI */
  2*BITS_PER_UNIT,         /* V8HI */
  2*BITS_PER_UNIT,         /* VNx8HI */
  4*BITS_PER_UNIT,         /* V4SI */
  4*BITS_PER_UNIT,         /* VNx4SI */
  8*BITS_PER_UNIT,         /* V2DI */
  8*BITS_PER_UNIT,         /* VNx2DI */
  1*BITS_PER_UNIT,         /* VNx32QI */
  2*BITS_PER_UNIT,         /* VNx16HI */
  4*BITS_PER_UNIT,         /* VNx8SI */
  8*BITS_PER_UNIT,         /* VNx4DI */
  16*BITS_PER_UNIT,        /* VNx2TI */
  1*BITS_PER_UNIT,         /* VNx48QI */
  2*BITS_PER_UNIT,         /* VNx24HI */
  4*BITS_PER_UNIT,         /* VNx12SI */
  8*BITS_PER_UNIT,         /* VNx6DI */
  16*BITS_PER_UNIT,        /* VNx3TI */
  1*BITS_PER_UNIT,         /* VNx64QI */
  2*BITS_PER_UNIT,         /* VNx32HI */
  4*BITS_PER_UNIT,         /* VNx16SI */
  8*BITS_PER_UNIT,         /* VNx8DI */
  16*BITS_PER_UNIT,        /* VNx4TI */
  32*BITS_PER_UNIT,        /* VNx2OI */
  2*BITS_PER_UNIT,         /* V2HF */
  2*BITS_PER_UNIT,         /* V4HF */
  4*BITS_PER_UNIT,         /* V2SF */
  8*BITS_PER_UNIT,         /* V1DF */
  2*BITS_PER_UNIT,         /* V8HF */
  2*BITS_PER_UNIT,         /* VNx8HF */
  4*BITS_PER_UNIT,         /* V4SF */
  4*BITS_PER_UNIT,         /* VNx4SF */
  8*BITS_PER_UNIT,         /* V2DF */
  8*BITS_PER_UNIT,         /* VNx2DF */
  2*BITS_PER_UNIT,         /* VNx16HF */
  4*BITS_PER_UNIT,         /* VNx8SF */
  8*BITS_PER_UNIT,         /* VNx4DF */
  2*BITS_PER_UNIT,         /* VNx24HF */
  4*BITS_PER_UNIT,         /* VNx12SF */
  8*BITS_PER_UNIT,         /* VNx6DF */
  2*BITS_PER_UNIT,         /* VNx32HF */
  4*BITS_PER_UNIT,         /* VNx16SF */
  8*BITS_PER_UNIT,         /* VNx8DF */
};

unsigned short mode_base_align[NUM_MACHINE_MODES] = 
{
  0,                       /* VOID */
  0,                       /* BLK */
  4,                       /* CC */
  4,                       /* CCFP */
  4,                       /* CCFPE */
  4,                       /* CC_SWP */
  4,                       /* CC_NZ */
  4,                       /* CC_Z */
  4,                       /* CC_C */
  4,                       /* CC_ADC */
  4,                       /* CC_V */
  1,                       /* BI */
  1,                       /* QI */
  2,                       /* HI */
  4,                       /* SI */
  8,                       /* DI */
  16,                      /* TI */
  32,                      /* OI */
  16,                      /* CI */
  64,                      /* XI */
  1,                       /* QQ */
  2,                       /* HQ */
  4,                       /* SQ */
  8,                       /* DQ */
  16,                      /* TQ */
  1,                       /* UQQ */
  2,                       /* UHQ */
  4,                       /* USQ */
  8,                       /* UDQ */
  16,                      /* UTQ */
  2,                       /* HA */
  4,                       /* SA */
  8,                       /* DA */
  16,                      /* TA */
  2,                       /* UHA */
  4,                       /* USA */
  8,                       /* UDA */
  16,                      /* UTA */
  2,                       /* HF */
  4,                       /* SF */
  8,                       /* DF */
  16,                      /* TF */
  4,                       /* SD */
  8,                       /* DD */
  16,                      /* TD */
  1,                       /* CQI */
  2,                       /* CHI */
  4,                       /* CSI */
  8,                       /* CDI */
  16,                      /* CTI */
  32,                      /* COI */
  16,                      /* CCI */
  64,                      /* CXI */
  2,                       /* HC */
  4,                       /* SC */
  8,                       /* DC */
  16,                      /* TC */
  2,                       /* VNx16BI */
  2,                       /* VNx8BI */
  2,                       /* VNx4BI */
  2,                       /* VNx2BI */
  8,                       /* V8QI */
  8,                       /* V4HI */
  8,                       /* V2SI */
  16,                      /* V16QI */
  16,                      /* VNx16QI */
  16,                      /* V8HI */
  16,                      /* VNx8HI */
  16,                      /* V4SI */
  16,                      /* VNx4SI */
  16,                      /* V2DI */
  16,                      /* VNx2DI */
  32,                      /* VNx32QI */
  32,                      /* VNx16HI */
  32,                      /* VNx8SI */
  32,                      /* VNx4DI */
  32,                      /* VNx2TI */
  16,                      /* VNx48QI */
  16,                      /* VNx24HI */
  16,                      /* VNx12SI */
  16,                      /* VNx6DI */
  16,                      /* VNx3TI */
  64,                      /* VNx64QI */
  64,                      /* VNx32HI */
  64,                      /* VNx16SI */
  64,                      /* VNx8DI */
  64,                      /* VNx4TI */
  64,                      /* VNx2OI */
  4,                       /* V2HF */
  8,                       /* V4HF */
  8,                       /* V2SF */
  8,                       /* V1DF */
  16,                      /* V8HF */
  16,                      /* VNx8HF */
  16,                      /* V4SF */
  16,                      /* VNx4SF */
  16,                      /* V2DF */
  16,                      /* VNx2DF */
  32,                      /* VNx16HF */
  32,                      /* VNx8SF */
  32,                      /* VNx4DF */
  16,                      /* VNx24HF */
  16,                      /* VNx12SF */
  16,                      /* VNx6DF */
  64,                      /* VNx32HF */
  64,                      /* VNx16SF */
  64,                      /* VNx8DF */
};

const unsigned char class_narrowest_mode[MAX_MODE_CLASS] =
{
  MIN_MODE_RANDOM,         /* VOID */
  MIN_MODE_CC,             /* CC */
  MIN_MODE_INT,            /* QI */
  MIN_MODE_PARTIAL_INT,    /* VOID */
  MIN_MODE_FRACT,          /* QQ */
  MIN_MODE_UFRACT,         /* UQQ */
  MIN_MODE_ACCUM,          /* HA */
  MIN_MODE_UACCUM,         /* UHA */
  MIN_MODE_FLOAT,          /* HF */
  MIN_MODE_DECIMAL_FLOAT,  /* SD */
  MIN_MODE_COMPLEX_INT,    /* CQI */
  MIN_MODE_COMPLEX_FLOAT,  /* HC */
  MIN_MODE_VECTOR_BOOL,    /* VNx16BI */
  MIN_MODE_VECTOR_INT,     /* V8QI */
  MIN_MODE_VECTOR_FRACT,   /* VOID */
  MIN_MODE_VECTOR_UFRACT,  /* VOID */
  MIN_MODE_VECTOR_ACCUM,   /* VOID */
  MIN_MODE_VECTOR_UACCUM,  /* VOID */
  MIN_MODE_VECTOR_FLOAT,   /* V2HF */
};

const struct real_format *
 real_format_for_mode[MAX_MODE_FLOAT - MIN_MODE_FLOAT + 1 + MAX_MODE_DECIMAL_FLOAT - MIN_MODE_DECIMAL_FLOAT + 1] =
{
  0,                       /* HF */
  &ieee_single_format,     /* SF */
  &ieee_double_format,     /* DF */
  &ieee_quad_format,       /* TF */
  &decimal_single_format,  /* SD */
  &decimal_double_format,  /* DD */
  &decimal_quad_format,    /* TD */
};

void
init_adjust_machine_modes (void)
{
  poly_uint16 ps ATTRIBUTE_UNUSED;
  size_t s ATTRIBUTE_UNUSED;

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx8DFmode], mode_nunits[E_VNx8DFmode]);
    mode_precision[E_VNx8DFmode] = ps * old_factor;
    mode_size[E_VNx8DFmode] = exact_div (mode_precision[E_VNx8DFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx8DFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx16SFmode], mode_nunits[E_VNx16SFmode]);
    mode_precision[E_VNx16SFmode] = ps * old_factor;
    mode_size[E_VNx16SFmode] = exact_div (mode_precision[E_VNx16SFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx16SFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx32HFmode], mode_nunits[E_VNx32HFmode]);
    mode_precision[E_VNx32HFmode] = ps * old_factor;
    mode_size[E_VNx32HFmode] = exact_div (mode_precision[E_VNx32HFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx32HFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx8DImode], mode_nunits[E_VNx8DImode]);
    mode_precision[E_VNx8DImode] = ps * old_factor;
    mode_size[E_VNx8DImode] = exact_div (mode_precision[E_VNx8DImode], BITS_PER_UNIT);
    mode_nunits[E_VNx8DImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx16SImode], mode_nunits[E_VNx16SImode]);
    mode_precision[E_VNx16SImode] = ps * old_factor;
    mode_size[E_VNx16SImode] = exact_div (mode_precision[E_VNx16SImode], BITS_PER_UNIT);
    mode_nunits[E_VNx16SImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx32HImode], mode_nunits[E_VNx32HImode]);
    mode_precision[E_VNx32HImode] = ps * old_factor;
    mode_size[E_VNx32HImode] = exact_div (mode_precision[E_VNx32HImode], BITS_PER_UNIT);
    mode_nunits[E_VNx32HImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:107 */
  ps = aarch64_sve_vg * 4 * 8;
    int old_factor = vector_element_size (mode_precision[E_VNx64QImode], mode_nunits[E_VNx64QImode]);
    mode_precision[E_VNx64QImode] = ps * old_factor;
    mode_size[E_VNx64QImode] = exact_div (mode_precision[E_VNx64QImode], BITS_PER_UNIT);
    mode_nunits[E_VNx64QImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3;
    int old_factor = vector_element_size (mode_precision[E_VNx6DFmode], mode_nunits[E_VNx6DFmode]);
    mode_precision[E_VNx6DFmode] = ps * old_factor;
    mode_size[E_VNx6DFmode] = exact_div (mode_precision[E_VNx6DFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx6DFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx12SFmode], mode_nunits[E_VNx12SFmode]);
    mode_precision[E_VNx12SFmode] = ps * old_factor;
    mode_size[E_VNx12SFmode] = exact_div (mode_precision[E_VNx12SFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx12SFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx24HFmode], mode_nunits[E_VNx24HFmode]);
    mode_precision[E_VNx24HFmode] = ps * old_factor;
    mode_size[E_VNx24HFmode] = exact_div (mode_precision[E_VNx24HFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx24HFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3;
    int old_factor = vector_element_size (mode_precision[E_VNx6DImode], mode_nunits[E_VNx6DImode]);
    mode_precision[E_VNx6DImode] = ps * old_factor;
    mode_size[E_VNx6DImode] = exact_div (mode_precision[E_VNx6DImode], BITS_PER_UNIT);
    mode_nunits[E_VNx6DImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx12SImode], mode_nunits[E_VNx12SImode]);
    mode_precision[E_VNx12SImode] = ps * old_factor;
    mode_size[E_VNx12SImode] = exact_div (mode_precision[E_VNx12SImode], BITS_PER_UNIT);
    mode_nunits[E_VNx12SImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx24HImode], mode_nunits[E_VNx24HImode]);
    mode_precision[E_VNx24HImode] = ps * old_factor;
    mode_size[E_VNx24HImode] = exact_div (mode_precision[E_VNx24HImode], BITS_PER_UNIT);
    mode_nunits[E_VNx24HImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:106 */
  ps = aarch64_sve_vg * 3 * 8;
    int old_factor = vector_element_size (mode_precision[E_VNx48QImode], mode_nunits[E_VNx48QImode]);
    mode_precision[E_VNx48QImode] = ps * old_factor;
    mode_size[E_VNx48QImode] = exact_div (mode_precision[E_VNx48QImode], BITS_PER_UNIT);
    mode_nunits[E_VNx48QImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx4DFmode], mode_nunits[E_VNx4DFmode]);
    mode_precision[E_VNx4DFmode] = ps * old_factor;
    mode_size[E_VNx4DFmode] = exact_div (mode_precision[E_VNx4DFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx4DFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx8SFmode], mode_nunits[E_VNx8SFmode]);
    mode_precision[E_VNx8SFmode] = ps * old_factor;
    mode_size[E_VNx8SFmode] = exact_div (mode_precision[E_VNx8SFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx8SFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx16HFmode], mode_nunits[E_VNx16HFmode]);
    mode_precision[E_VNx16HFmode] = ps * old_factor;
    mode_size[E_VNx16HFmode] = exact_div (mode_precision[E_VNx16HFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx16HFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx4DImode], mode_nunits[E_VNx4DImode]);
    mode_precision[E_VNx4DImode] = ps * old_factor;
    mode_size[E_VNx4DImode] = exact_div (mode_precision[E_VNx4DImode], BITS_PER_UNIT);
    mode_nunits[E_VNx4DImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx8SImode], mode_nunits[E_VNx8SImode]);
    mode_precision[E_VNx8SImode] = ps * old_factor;
    mode_size[E_VNx8SImode] = exact_div (mode_precision[E_VNx8SImode], BITS_PER_UNIT);
    mode_nunits[E_VNx8SImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx16HImode], mode_nunits[E_VNx16HImode]);
    mode_precision[E_VNx16HImode] = ps * old_factor;
    mode_size[E_VNx16HImode] = exact_div (mode_precision[E_VNx16HImode], BITS_PER_UNIT);
    mode_nunits[E_VNx16HImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:105 */
  ps = aarch64_sve_vg * 2 * 8;
    int old_factor = vector_element_size (mode_precision[E_VNx32QImode], mode_nunits[E_VNx32QImode]);
    mode_precision[E_VNx32QImode] = ps * old_factor;
    mode_size[E_VNx32QImode] = exact_div (mode_precision[E_VNx32QImode], BITS_PER_UNIT);
    mode_nunits[E_VNx32QImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1;
    int old_factor = vector_element_size (mode_precision[E_VNx2DFmode], mode_nunits[E_VNx2DFmode]);
    mode_precision[E_VNx2DFmode] = ps * old_factor;
    mode_size[E_VNx2DFmode] = exact_div (mode_precision[E_VNx2DFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx2DFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx4SFmode], mode_nunits[E_VNx4SFmode]);
    mode_precision[E_VNx4SFmode] = ps * old_factor;
    mode_size[E_VNx4SFmode] = exact_div (mode_precision[E_VNx4SFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx4SFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx8HFmode], mode_nunits[E_VNx8HFmode]);
    mode_precision[E_VNx8HFmode] = ps * old_factor;
    mode_size[E_VNx8HFmode] = exact_div (mode_precision[E_VNx8HFmode], BITS_PER_UNIT);
    mode_nunits[E_VNx8HFmode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1;
    int old_factor = vector_element_size (mode_precision[E_VNx2DImode], mode_nunits[E_VNx2DImode]);
    mode_precision[E_VNx2DImode] = ps * old_factor;
    mode_size[E_VNx2DImode] = exact_div (mode_precision[E_VNx2DImode], BITS_PER_UNIT);
    mode_nunits[E_VNx2DImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1 * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx4SImode], mode_nunits[E_VNx4SImode]);
    mode_precision[E_VNx4SImode] = ps * old_factor;
    mode_size[E_VNx4SImode] = exact_div (mode_precision[E_VNx4SImode], BITS_PER_UNIT);
    mode_nunits[E_VNx4SImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1 * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx8HImode], mode_nunits[E_VNx8HImode]);
    mode_precision[E_VNx8HImode] = ps * old_factor;
    mode_size[E_VNx8HImode] = exact_div (mode_precision[E_VNx8HImode], BITS_PER_UNIT);
    mode_nunits[E_VNx8HImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:104 */
  ps = aarch64_sve_vg * 1 * 8;
    int old_factor = vector_element_size (mode_precision[E_VNx16QImode], mode_nunits[E_VNx16QImode]);
    mode_precision[E_VNx16QImode] = ps * old_factor;
    mode_size[E_VNx16QImode] = exact_div (mode_precision[E_VNx16QImode], BITS_PER_UNIT);
    mode_nunits[E_VNx16QImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:56 */
  ps = aarch64_sve_vg;
    int old_factor = vector_element_size (mode_precision[E_VNx2BImode], mode_nunits[E_VNx2BImode]);
    mode_precision[E_VNx2BImode] = ps * old_factor;
    mode_size[E_VNx2BImode] = exact_div (mode_precision[E_VNx2BImode], BITS_PER_UNIT);
    mode_nunits[E_VNx2BImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:55 */
  ps = aarch64_sve_vg * 2;
    int old_factor = vector_element_size (mode_precision[E_VNx4BImode], mode_nunits[E_VNx4BImode]);
    mode_precision[E_VNx4BImode] = ps * old_factor;
    mode_size[E_VNx4BImode] = exact_div (mode_precision[E_VNx4BImode], BITS_PER_UNIT);
    mode_nunits[E_VNx4BImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:54 */
  ps = aarch64_sve_vg * 4;
    int old_factor = vector_element_size (mode_precision[E_VNx8BImode], mode_nunits[E_VNx8BImode]);
    mode_precision[E_VNx8BImode] = ps * old_factor;
    mode_size[E_VNx8BImode] = exact_div (mode_precision[E_VNx8BImode], BITS_PER_UNIT);
    mode_nunits[E_VNx8BImode] = ps;
  }

  {
    /* config/aarch64/aarch64-modes.def:53 */
  ps = aarch64_sve_vg * 8;
    int old_factor = vector_element_size (mode_precision[E_VNx16BImode], mode_nunits[E_VNx16BImode]);
    mode_precision[E_VNx16BImode] = ps * old_factor;
    mode_size[E_VNx16BImode] = exact_div (mode_precision[E_VNx16BImode], BITS_PER_UNIT);
    mode_nunits[E_VNx16BImode] = ps;
  }

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx8DFmode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx16SFmode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx32HFmode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx8DImode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx16SImode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx32HImode] = s;

  /* config/aarch64/aarch64-modes.def:107 */
  s = 16;
  mode_base_align[E_VNx64QImode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx6DFmode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx12SFmode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx24HFmode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx6DImode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx12SImode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx24HImode] = s;

  /* config/aarch64/aarch64-modes.def:106 */
  s = 16;
  mode_base_align[E_VNx48QImode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx4DFmode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx8SFmode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx16HFmode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx4DImode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx8SImode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx16HImode] = s;

  /* config/aarch64/aarch64-modes.def:105 */
  s = 16;
  mode_base_align[E_VNx32QImode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx2DFmode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx4SFmode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx8HFmode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx2DImode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx4SImode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx8HImode] = s;

  /* config/aarch64/aarch64-modes.def:104 */
  s = 16;
  mode_base_align[E_VNx16QImode] = s;

  /* config/aarch64/aarch64-modes.def:61 */
  s = 2;
  mode_base_align[E_VNx2BImode] = s;

  /* config/aarch64/aarch64-modes.def:60 */
  s = 2;
  mode_base_align[E_VNx4BImode] = s;

  /* config/aarch64/aarch64-modes.def:59 */
  s = 2;
  mode_base_align[E_VNx8BImode] = s;

  /* config/aarch64/aarch64-modes.def:58 */
  s = 2;
  mode_base_align[E_VNx16BImode] = s;

  /* config/aarch64/aarch64-modes.def:44 */
  REAL_MODE_FORMAT (E_HFmode) = &ieee_half_format;
}

const unsigned char mode_ibit[NUM_MACHINE_MODES] = 
{
  0,                       /* VOID */
  0,                       /* BLK */
  0,                       /* CC */
  0,                       /* CCFP */
  0,                       /* CCFPE */
  0,                       /* CC_SWP */
  0,                       /* CC_NZ */
  0,                       /* CC_Z */
  0,                       /* CC_C */
  0,                       /* CC_ADC */
  0,                       /* CC_V */
  0,                       /* BI */
  0,                       /* QI */
  0,                       /* HI */
  0,                       /* SI */
  0,                       /* DI */
  0,                       /* TI */
  0,                       /* OI */
  0,                       /* CI */
  0,                       /* XI */
  0,                       /* QQ */
  0,                       /* HQ */
  0,                       /* SQ */
  0,                       /* DQ */
  0,                       /* TQ */
  0,                       /* UQQ */
  0,                       /* UHQ */
  0,                       /* USQ */
  0,                       /* UDQ */
  0,                       /* UTQ */
  8,                       /* HA */
  16,                      /* SA */
  32,                      /* DA */
  64,                      /* TA */
  8,                       /* UHA */
  16,                      /* USA */
  32,                      /* UDA */
  64,                      /* UTA */
  0,                       /* HF */
  0,                       /* SF */
  0,                       /* DF */
  0,                       /* TF */
  0,                       /* SD */
  0,                       /* DD */
  0,                       /* TD */
  0,                       /* CQI */
  0,                       /* CHI */
  0,                       /* CSI */
  0,                       /* CDI */
  0,                       /* CTI */
  0,                       /* COI */
  0,                       /* CCI */
  0,                       /* CXI */
  0,                       /* HC */
  0,                       /* SC */
  0,                       /* DC */
  0,                       /* TC */
  0,                       /* VNx16BI */
  0,                       /* VNx8BI */
  0,                       /* VNx4BI */
  0,                       /* VNx2BI */
  0,                       /* V8QI */
  0,                       /* V4HI */
  0,                       /* V2SI */
  0,                       /* V16QI */
  0,                       /* VNx16QI */
  0,                       /* V8HI */
  0,                       /* VNx8HI */
  0,                       /* V4SI */
  0,                       /* VNx4SI */
  0,                       /* V2DI */
  0,                       /* VNx2DI */
  0,                       /* VNx32QI */
  0,                       /* VNx16HI */
  0,                       /* VNx8SI */
  0,                       /* VNx4DI */
  0,                       /* VNx2TI */
  0,                       /* VNx48QI */
  0,                       /* VNx24HI */
  0,                       /* VNx12SI */
  0,                       /* VNx6DI */
  0,                       /* VNx3TI */
  0,                       /* VNx64QI */
  0,                       /* VNx32HI */
  0,                       /* VNx16SI */
  0,                       /* VNx8DI */
  0,                       /* VNx4TI */
  0,                       /* VNx2OI */
  0,                       /* V2HF */
  0,                       /* V4HF */
  0,                       /* V2SF */
  0,                       /* V1DF */
  0,                       /* V8HF */
  0,                       /* VNx8HF */
  0,                       /* V4SF */
  0,                       /* VNx4SF */
  0,                       /* V2DF */
  0,                       /* VNx2DF */
  0,                       /* VNx16HF */
  0,                       /* VNx8SF */
  0,                       /* VNx4DF */
  0,                       /* VNx24HF */
  0,                       /* VNx12SF */
  0,                       /* VNx6DF */
  0,                       /* VNx32HF */
  0,                       /* VNx16SF */
  0,                       /* VNx8DF */
};

const unsigned char mode_fbit[NUM_MACHINE_MODES] = 
{
  0,                       /* VOID */
  0,                       /* BLK */
  0,                       /* CC */
  0,                       /* CCFP */
  0,                       /* CCFPE */
  0,                       /* CC_SWP */
  0,                       /* CC_NZ */
  0,                       /* CC_Z */
  0,                       /* CC_C */
  0,                       /* CC_ADC */
  0,                       /* CC_V */
  0,                       /* BI */
  0,                       /* QI */
  0,                       /* HI */
  0,                       /* SI */
  0,                       /* DI */
  0,                       /* TI */
  0,                       /* OI */
  0,                       /* CI */
  0,                       /* XI */
  7,                       /* QQ */
  15,                      /* HQ */
  31,                      /* SQ */
  63,                      /* DQ */
  127,                     /* TQ */
  8,                       /* UQQ */
  16,                      /* UHQ */
  32,                      /* USQ */
  64,                      /* UDQ */
  128,                     /* UTQ */
  7,                       /* HA */
  15,                      /* SA */
  31,                      /* DA */
  63,                      /* TA */
  8,                       /* UHA */
  16,                      /* USA */
  32,                      /* UDA */
  64,                      /* UTA */
  0,                       /* HF */
  0,                       /* SF */
  0,                       /* DF */
  0,                       /* TF */
  0,                       /* SD */
  0,                       /* DD */
  0,                       /* TD */
  0,                       /* CQI */
  0,                       /* CHI */
  0,                       /* CSI */
  0,                       /* CDI */
  0,                       /* CTI */
  0,                       /* COI */
  0,                       /* CCI */
  0,                       /* CXI */
  0,                       /* HC */
  0,                       /* SC */
  0,                       /* DC */
  0,                       /* TC */
  0,                       /* VNx16BI */
  0,                       /* VNx8BI */
  0,                       /* VNx4BI */
  0,                       /* VNx2BI */
  0,                       /* V8QI */
  0,                       /* V4HI */
  0,                       /* V2SI */
  0,                       /* V16QI */
  0,                       /* VNx16QI */
  0,                       /* V8HI */
  0,                       /* VNx8HI */
  0,                       /* V4SI */
  0,                       /* VNx4SI */
  0,                       /* V2DI */
  0,                       /* VNx2DI */
  0,                       /* VNx32QI */
  0,                       /* VNx16HI */
  0,                       /* VNx8SI */
  0,                       /* VNx4DI */
  0,                       /* VNx2TI */
  0,                       /* VNx48QI */
  0,                       /* VNx24HI */
  0,                       /* VNx12SI */
  0,                       /* VNx6DI */
  0,                       /* VNx3TI */
  0,                       /* VNx64QI */
  0,                       /* VNx32HI */
  0,                       /* VNx16SI */
  0,                       /* VNx8DI */
  0,                       /* VNx4TI */
  0,                       /* VNx2OI */
  0,                       /* V2HF */
  0,                       /* V4HF */
  0,                       /* V2SF */
  0,                       /* V1DF */
  0,                       /* V8HF */
  0,                       /* VNx8HF */
  0,                       /* V4SF */
  0,                       /* VNx4SF */
  0,                       /* V2DF */
  0,                       /* VNx2DF */
  0,                       /* VNx16HF */
  0,                       /* VNx8SF */
  0,                       /* VNx4DF */
  0,                       /* VNx24HF */
  0,                       /* VNx12SF */
  0,                       /* VNx6DF */
  0,                       /* VNx32HF */
  0,                       /* VNx16SF */
  0,                       /* VNx8DF */
};

const int_n_data_t int_n_data[] =
{
 {
  128,                     /* TI */
{ E_TImode }, },
};
