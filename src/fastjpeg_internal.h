/*--------------------------------------------------------------------------*/

#ifndef __FASTJPEG_INTERNAL_H__
#define __FASTJPEG_INTERNAL_H__

/*--------------------------------------------------------------------------*/

#include "../include/fastjpeg.h"

/*--------------------------------------------------------------------------*/
/* PANIC */
/*--------------------------------------------------------------------------*/

void fastjpeg_panic(const char *format, ...) __attribute__ ((format(printf, 1, 2), noreturn));

/*--------------------------------------------------------------------------*/
/* ENDIANNESS */
/*--------------------------------------------------------------------------*/

#if __GNUC__ >= 4
  #define endian_word(x) (		\
    (((x) >> 0) & 0xFF) << 8	\
    |							\
    (((x) >> 8) & 0xFF) << 0	\
  )

  #define endian_dword(x) (		\
    (((x) >>  0) & 0xFF) << 24	\
    |							\
    (((x) >>  8) & 0xFF) << 16	\
    |							\
    (((x) >> 16) & 0xFF) << 8	\
    |							\
    (((x) >> 24) & 0xFF) << 0	\
  )

  #define endian_qword(x) (		\
    (((x) >>  0) & 0xFF) << 56	\
    |							\
    (((x) >>  8) & 0xFF) << 48	\
    |							\
    (((x) >> 16) & 0xFF) << 40	\
    |							\
    (((x) >> 24) & 0xFF) << 32	\
    |							\
    (((x) >> 32) & 0xFF) << 24	\
    |							\
    (((x) >> 40) & 0xFF) << 16	\
    |							\
    (((x) >> 48) & 0xFF) << 8	\
    |							\
    (((x) >> 56) & 0xFF) << 0	\
  )
#else
  #error "fastjpeg requires at least gcc 4.0.0"
#endif

/*--------------------------------------------------------------------------*/
/* MEMORY */
/*--------------------------------------------------------------------------*/

DLL_PUBLIC size_t fastjpeg_memory_get_size(buff_t);
DLL_PUBLIC const char *fastjpeg_memory_get_file(buff_t);
DLL_PUBLIC int fastjpeg_memory_get_line(buff_t);

/*--------------------------------------------------------------------------*/
/* FASTJPEG */
/*--------------------------------------------------------------------------*/

typedef struct fastjpeg_marker_data_s
{
	bool soi;
	bool eoi;

	size_t size[64]
	uint8_t *data[64];

} fastjpeg_marker_data_t;

/*--------------------------------------------------------------------------*/

bool fastjpeg_read_byte(struct fastjpeg_s *fastjpeg, uint8_t *value);
bool fastjpeg_read_word(struct fastjpeg_s *fastjpeg, uint16_t *value);
bool fastjpeg_read_dword(struct fastjpeg_s *fastjpeg, uint32_t *value);
bool fastjpeg_read_qword(struct fastjpeg_s *fastjpeg, uint64_t *value);
bool fastjpeg_read_buffer(struct fastjpeg_s *fastjpeg, uint8_t *value, size_t size);

/*--------------------------------------------------------------------------*/
/* IO */
/*--------------------------------------------------------------------------*/

typedef struct fastjpeg_io_descr_s
{
	long data[4];

} fastjpeg_io_descr_t;

/*--------------------------------------------------------------------------*/

typedef enum fastjpeg_io_type_e
{
	FASTJPEG_IO_IN,
	FASTJPEG_IO_OUT,

} fastjpeg_io_type_t;

/*--------------------------------------------------------------------------*/

typedef struct fastjpeg_io_s
{
	fastjpeg_io_type_t type;

	fastjpeg_io_descr_t descr;

	void (* init) (struct fastjpeg_io_descr_s descr);
	long (* read) (struct fastjpeg_io_descr_s descr, uint8_t *buff, size_t size);
	bool (* skip) (struct fastjpeg_io_descr_s descr, size_t size);
	bool (* seek) (struct fastjpeg_io_descr_s descr, size_t size);
	void (* close) (struct fastjpeg_io_descr_s descr);

} fastjpeg_io_t;

/*--------------------------------------------------------------------------*/

fastjpeg_io_t *fastjpeg_io_new(
	fastjpeg_io_descr_t descr,
	fastjpeg_io_type_t type,
	void (* init) (struct fastjpeg_io_descr_s descr),
	long (* read) (struct fastjpeg_io_descr_s descr, uint8_t *buff, size_t size),
	bool (* skip) (struct fastjpeg_io_descr_s descr, size_t size),
	bool (* seek) (struct fastjpeg_io_descr_s descr, size_t size),
	void (* close) (struct fastjpeg_io_descr_s descr)
);

void fastjpeg_io_delete(struct fastjpeg_io_s *fastjpeg_io);

/*--------------------------------------------------------------------------*/
/* JPEG MARKERS */
/*--------------------------------------------------------------------------*/

#define FASTJPEG_MARKER_TEM			0XFF01

#define FASTJPEG_MARKER_SOF0		0XFFC0
#define FASTJPEG_MARKER_SOF1		0XFFC1
#define FASTJPEG_MARKER_SOF2		0XFFC2
#define FASTJPEG_MARKER_SOF3		0XFFC3
#define FASTJPEG_MARKER_DHT			0XFFC4
#define FASTJPEG_MARKER_SOF5		0XFFC5
#define FASTJPEG_MARKER_SOF6		0XFFC6
#define FASTJPEG_MARKER_SOF7		0XFFC7
#define FASTJPEG_MARKER_JPG			0XFFC8
#define FASTJPEG_MARKER_SOF9		0XFFC9
#define FASTJPEG_MARKER_SOF10		0XFFCA
#define FASTJPEG_MARKER_SOF11		0XFFCB
#define FASTJPEG_MARKER_DAC			0XFFCC
#define FASTJPEG_MARKER_SOF13		0XFFCD
#define FASTJPEG_MARKER_SOF14		0XFFCE
#define FASTJPEG_MARKER_SOF15		0XFFCF

#define FASTJPEG_MARKER_RST0		0XFFD0
#define FASTJPEG_MARKER_RST1		0XFFD1
#define FASTJPEG_MARKER_RST2		0XFFD2
#define FASTJPEG_MARKER_RST3		0XFFD3
#define FASTJPEG_MARKER_RST4		0XFFD4
#define FASTJPEG_MARKER_RST5		0XFFD5
#define FASTJPEG_MARKER_RST6		0XFFD6
#define FASTJPEG_MARKER_RST7		0XFFD7
#define FASTJPEG_MARKER_SOI			0XFFD8
#define FASTJPEG_MARKER_EOI			0XFFD9
#define FASTJPEG_MARKER_SOS			0XFFDA
#define FASTJPEG_MARKER_DQT			0XFFDB
#define FASTJPEG_MARKER_DNL			0XFFDC
#define FASTJPEG_MARKER_DRI			0XFFDD
#define FASTJPEG_MARKER_DHP			0XFFDE
#define FASTJPEG_MARKER_EXP			0XFFDF

#define FASTJPEG_MARKER_APP0		0XFFE0
#define FASTJPEG_MARKER_APP1		0XFFE1
#define FASTJPEG_MARKER_APP2		0XFFE2
#define FASTJPEG_MARKER_APP3		0XFFE3
#define FASTJPEG_MARKER_APP4		0XFFE4
#define FASTJPEG_MARKER_APP5		0XFFE5
#define FASTJPEG_MARKER_APP6		0XFFE6
#define FASTJPEG_MARKER_APP7		0XFFE7
#define FASTJPEG_MARKER_APP8		0XFFE8
#define FASTJPEG_MARKER_APP9		0XFFE9
#define FASTJPEG_MARKER_APP10		0XFFEA
#define FASTJPEG_MARKER_APP11		0XFFEB
#define FASTJPEG_MARKER_APP12		0XFFEC
#define FASTJPEG_MARKER_APP13		0XFFED
#define FASTJPEG_MARKER_APP14		0XFFEE
#define FASTJPEG_MARKER_APP15		0XFFEF

#define FASTJPEG_MARKER_JPG0		0XFFF0
#define FASTJPEG_MARKER_JPG1		0XFFF1
#define FASTJPEG_MARKER_JPG2		0XFFF2
#define FASTJPEG_MARKER_JPG3		0XFFF3
#define FASTJPEG_MARKER_JPG4		0XFFF4
#define FASTJPEG_MARKER_JPG5		0XFFF5
#define FASTJPEG_MARKER_JPG6		0XFFF6
#define FASTJPEG_MARKER_JPG7		0XFFF7
#define FASTJPEG_MARKER_JPG8		0XFFF8
#define FASTJPEG_MARKER_JPG9		0XFFF9
#define FASTJPEG_MARKER_JPG10		0XFFFA
#define FASTJPEG_MARKER_JPG11		0XFFFB
#define FASTJPEG_MARKER_JPG12		0XFFFC
#define FASTJPEG_MARKER_JPG13		0XFFFD
#define FASTJPEG_MARKER_COM			0XFFFE

/*--------------------------------------------------------------------------*/
/* JFIF HEADER */
/*--------------------------------------------------------------------------*/

typedef struct fastjpeg_jfif_header_s
{
	uint8_t id[5];

	uint8_t version_major;
	uint8_t version_minor;

	uint8_t units;

	uint16_t x_density;
	uint16_t y_density;

	uint8_t x_thumbnail;
	uint8_t y_thumbnail;
	uint8_t *thumbnail;

} __attribute__ ((packed)) fastjpeg_jfif_header_t;

/*--------------------------------------------------------------------------*/

struct fastjpeg_jfif_header_s *fastjpeg_jfif_header_new(void);
struct fastjpeg_jfif_header_s *fastjpeg_jfif_header_extract(uint8_t *buff, size_t size);

void fastjpeg_jfif_header_delete(struct fastjpeg_jfif_header_s *jfif_header);

/*--------------------------------------------------------------------------*/

#endif /* __FASTJPEG_INTERNAL_H__ */

/*--------------------------------------------------------------------------*/

