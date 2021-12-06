SRC_LIBFREETYPE = phantom/libfreetype/sfnt.c \
									phantom/libfreetype/ftutil.c \
									phantom/libfreetype/ftoutln.c \
									phantom/libfreetype/ttsbit.c \
									phantom/libfreetype/ftbitmap.c \
									phantom/libfreetype/ttpost.c \
									phantom/libfreetype/ftstream.c \
									phantom/libfreetype/ftsystem.c \
									phantom/libfreetype/smooth.c \
									phantom/libfreetype/ttgxvar.c \
									phantom/libfreetype/ftinit.c \
									phantom/libfreetype/ftrfork.c \
									phantom/libfreetype/truetype.c \
									phantom/libfreetype/ftdebug.c \
									phantom/libfreetype/ftsmooth.c \
									phantom/libfreetype/ftobjs.c \
									phantom/libfreetype/ftgloadr.c \
									phantom/libfreetype/ftbase.c \
									phantom/libfreetype/ftdbgmem.c \
									phantom/libfreetype/sfobjs.c \
									phantom/libfreetype/autofit.c \
									phantom/libfreetype/fttrigon.c \
									phantom/libfreetype/ftnames.c \
									phantom/libfreetype/ttkern.c \
									phantom/libfreetype/ttload.c \
									phantom/libfreetype/ftcalc.c \
									phantom/libfreetype/ftgrays.c \
									phantom/libfreetype/ftglyph.c


									# phantom/libfreetype/sfdriver.c \


# phantom/libfreetype/ttdriver.c \
# phantom/libfreetype/ttpload.c \
# phantom/libfreetype/ttgload.c \
# phantom/libfreetype/ttobjs.c \


# Fixing undefined references

# SRC_LIBFREETYPE += phantom/libfreetype/ttcmap.c

# phantom/libfreetype/ttcmap.c \ # Mul Def from sfnt.c
# phantom/libfreetype/afangles.c \
# phantom/libfreetype/afglobal.c \
# phantom/libfreetype/afhints.c \
# phantom/libfreetype/afdummy.c \
# phantom/libfreetype/aflatin.c \
# phantom/libfreetype/afloader.c \
# phantom/libfreetype/afmodule.c \
									
# sfnt.c includes
#include "ttload.c"
#include "ttcmap.c"
#include "ttkern.c"
#include "sfobjs.c"
#include "sfdriver.c"

# autofit.c includes
#include "afangles.c"
#include "afglobal.c"
#include "afhints.c"
#include "afdummy.c"
#include "aflatin.c"
#include "afloader.c"
#include "afmodule.c"

# truetype.c includes
#include <ft2build.h>
#include "ttdriver.c"   /* driver interface    */
#include "ttpload.c"    /* tables loader       */
#include "ttgload.c"    /* glyph loader        */
#include "ttobjs.c"     /* object manager      */

# SRC_LIBFREETYPE = phantom/libfreetype/sfnt.c

SRC_C += $(SRC_LIBFREETYPE)