SRC_LIBFREETYPE = phantom/libfreetype/sfnt.c \
									phantom/libfreetype/ttgload.c \
									phantom/libfreetype/ftutil.c \
									phantom/libfreetype/ttcmap.c \
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
									phantom/libfreetype/afdummy.c \
									phantom/libfreetype/ftobjs.c \
									phantom/libfreetype/ftgloadr.c \
									phantom/libfreetype/ftbase.c \
									phantom/libfreetype/afglobal.c \
									phantom/libfreetype/afhints.c \
									phantom/libfreetype/ftdbgmem.c \
									phantom/libfreetype/sfobjs.c \
									phantom/libfreetype/sfdriver.c \
									phantom/libfreetype/afloader.c \
									phantom/libfreetype/autofit.c \
									phantom/libfreetype/fttrigon.c \
									phantom/libfreetype/ftnames.c \
									phantom/libfreetype/ttkern.c \
									phantom/libfreetype/ttobjs.c \
									phantom/libfreetype/afmodule.c \
									phantom/libfreetype/ttload.c \
									phantom/libfreetype/afangles.c \
									phantom/libfreetype/aflatin.c \
									phantom/libfreetype/ftcalc.c \
									phantom/libfreetype/ftgrays.c \
									phantom/libfreetype/ttdriver.c \
									phantom/libfreetype/ftglyph.c \
									phantom/libfreetype/ttpload.c


#include "ttload.c"
#include "ttcmap.c"
#include "ttkern.c"
#include "sfobjs.c"
#include "sfdriver.c"

SRC_LIBFREETYPE = phantom/libfreetype/sfnt.c

SRC_C += $(SRC_LIBFREETYPE)