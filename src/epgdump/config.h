#ifndef CONFIG_H
#define CONFIG_H

/* #undef ICONV_REQUIRES_CONST */

#ifdef ICONV_REQUIRES_CONST
#define ICONV_CONST const
#else
#define ICONV_CONST
#endif
#endif

#define VERSION "0.2.1"
