#include <ruby.h>
#include <ruby/encoding.h>
#include "debug.h"
#include "rb_utilities.h"



#define AE_RB_STR_EXTERNAL_NEW(s, len) (rb_enc_str_new(s, len, rb_default_external_encoding()))
#define AE_RB_STR_TAINTED_EXTERNAL_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_default_external_encoding()))
#define AE_RB_STR_UTF8_NEW(s, len) (rb_enc_str_new(s, len, rb_utf8_encoding()))
#define AE_RB_STR_TAINTED_UTF8_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_utf8_encoding()))
#define AE_RB_STR_ASCII_NEW(s, len) (rb_enc_str_new(s, len, rb_ascii8bit_encoding()))
#define AE_RB_STR_TAINTED_ASCII_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_ascii8bit_encoding()))


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
      AE_ASSERT("invalid enum_string_encoding");
  }
}


VALUE ae_get_rb_encoding(enum_string_encoding encoding)
{
  AE_TRACE();

  switch(encoding) {
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
      AE_ASSERT("invalid enum_string_encoding");
      return Qfalse;
  }
}
