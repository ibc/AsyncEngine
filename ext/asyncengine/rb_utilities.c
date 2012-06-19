#include <ruby.h>
#include <ruby/encoding.h>
#include "debug.h"
#include "rb_utilities.h"


void init_rb_utilities(void)
{
  AE_TRACE();

  symbol_encoding_external = ID2SYM(rb_intern("external"));
  symbol_encoding_utf8 = ID2SYM(rb_intern("utf8"));
  symbol_encoding_ascii = ID2SYM(rb_intern("ascii"));
}


VALUE ae_rb_str_new(char* s, long len, enum_string_encoding enc, int tainted)
{
  AE_TRACE();

  switch(enc) {
    case string_encoding_external:
      return ( tainted ? AE_RB_STR_TAINTED_EXTERNAL_NEW(s, len) : AE_RB_STR_EXTERNAL_NEW(s, len) );
      break;
    case string_encoding_utf8:
      return ( tainted ? AE_RB_STR_TAINTED_UTF8_NEW(s, len) : AE_RB_STR_UTF8_NEW(s, len) );
      break;
    case string_encoding_ascii:
      return ( tainted ? AE_RB_STR_TAINTED_ASCII_NEW(s, len) : AE_RB_STR_ASCII_NEW(s, len) );
      break;
    default:
      AE_ABORT("invalid enum_string_encoding %d", enc);
  }
}


VALUE ae_encoding_to_rb_symbol(enum_string_encoding enc)
{
  AE_TRACE();

  switch(enc) {
    case string_encoding_external:
      return symbol_encoding_external;
      break;
    case string_encoding_utf8:
      return symbol_encoding_utf8;
      break;
    case string_encoding_ascii:
      return symbol_encoding_ascii;
      break;
    default:
      AE_ABORT("invalid enum_string_encoding %d", enc);
  }
}
