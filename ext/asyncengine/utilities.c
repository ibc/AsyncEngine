#include <ruby.h>
#include <ruby/encoding.h>
#include "utilities.h"
#include "debug.h"


#define AE_RB_STR_EXTERNAL_NEW(s, len) (rb_enc_str_new(s, len, rb_default_external_encoding()))
#define AE_RB_STR_TAINTED_EXTERNAL_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_default_external_encoding()))
#define AE_RB_STR_UTF8_NEW(s, len) (rb_enc_str_new(s, len, rb_utf8_encoding()))
#define AE_RB_STR_TAINTED_UTF8_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_utf8_encoding()))
#define AE_RB_STR_ASCII_NEW(s, len) (rb_enc_str_new(s, len, rb_ascii8bit_encoding()))
#define AE_RB_STR_TAINTED_ASCII_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_ascii8bit_encoding()))


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