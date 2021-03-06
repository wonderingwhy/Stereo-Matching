/* libtiff/tif_config.h.  Generated from tif_config.h.in by configure.  */
/* libtiff/tif_config.h.in.  Generated from configure.ac by autoheader.  */

#include "tiffconf.h"

/* enable partial strip reading for large strips (experimental) */
#undef CHUNKY_STRIP_READ_SUPPORT

/* enable deferred strip/tile offset/size loading (experimental) */
#undef DEFER_STRILE_LOAD

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define to 1 if you have the `jbg_newlen' function. */
/* #undef HAVE_JBG_NEWLEN */

/* Define to 1 if you have the `mmap' function. */
/* #undef HAVE_MMAP */

/* Define to 1 if you have the <search.h> header file. */
/* #undef HAVE_SEARCH_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef HAVE_UNISTD_H
/* #undef HAVE_UNISTD_H */
#endif

/* 8/12 bit libjpeg dual mode enabled */
/* #undef JPEG_DUAL_MODE_8_12 */

/* 12bit libjpeg primary include file with path */
/* #undef LIBJPEG_12_PATH */

/* Support LZMA2 compression */
/* #undef LZMA_SUPPORT */

/* Support Macintosh PackBits algorithm */
#define PACKBITS_SUPPORT 1

/* Support Pixar log-format algorithm (requires Zlib) */
#define PIXARLOG_SUPPORT 1

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* Support strip chopping (whether or not to convert single-strip uncompressed
   images to mutiple strips of specified size to reduce memory usage) */
#define STRIPCHOP_DEFAULT TIFF_STRIPCHOP

/* Default size of the strip in bytes (when strip chopping enabled) */
//#define STRIP_SIZE_DEFAULT 8192

/* Signed 64-bit type formatter */
/* Unsigned 64-bit type formatter */
#if defined _MSC_VER || defined __MINGW__ || defined __MINGW32__
# define TIFF_UINT64_FORMAT "%I64u"
# define TIFF_SSIZE_FORMAT "%Iu"
#else
# define TIFF_UINT64_FORMAT "%llu"
# define TIFF_SSIZE_FORMAT "%zd"
#endif

/* define to use win32 IO system */
/* #undef USE_WIN32_FILEIO */

#if defined _MSC_VER
/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
# ifndef inline
#  define inline __inline
# endif
#endif
//#define snprintf _snprintf
//#define lfind _lfind
#endif
