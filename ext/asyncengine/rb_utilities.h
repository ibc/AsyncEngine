#ifndef _AE_RB_UTILITIES_H
#define _AE_RB_UTILITIES_H


/*
 * Macros for checking variable arguments number in Ruby C methods.
 * NOTE: These macros assume that the Ruby C method is defined as:
 *   VALUE xxx(int argc, VALUE *argv, VALUE self)
 */
#define AE_RB_CHECK_NUM_ARGS(min,max)  \
if (argc < min || argc > max)  \
  rb_raise(rb_eArgError, "wrong number of arguments")

#define AE_RB_GET_BLOCK_OR_PROC(num_arg, target_proc)  \
  if (rb_block_given_p())  \
    target_proc = rb_block_proc();  \
  else if (argc >= num_arg) {  \
    target_proc = argv[num_arg-1];  \
    if (! rb_obj_is_proc(target_proc))  \
      rb_raise(rb_eTypeError, "argument %d is not a Proc", num_arg);  \
    }  \
  else  \
    target_proc = Qnil;

#define AE_RB_ENSURE_BLOCK_OR_PROC(num_arg, save_to)  \
  AE_RB_GET_BLOCK_OR_PROC(num_arg, save_to);  \
  if (NIL_P(save_to))  \
    rb_raise(rb_eArgError, "no block or proc given");

/*
 * Macros for generating Ruby strings with specified encoding.
 */
#define AE_RB_STR_EXTERNAL_NEW(s, len) (rb_enc_str_new(s, len, rb_default_external_encoding()))
#define AE_RB_STR_TAINTED_EXTERNAL_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_default_external_encoding()))
#define AE_RB_STR_UTF8_NEW(s, len) (rb_enc_str_new(s, len, rb_utf8_encoding()))
#define AE_RB_STR_TAINTED_UTF8_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_utf8_encoding()))
#define AE_RB_STR_ASCII_NEW(s, len) (rb_enc_str_new(s, len, rb_ascii8bit_encoding()))
#define AE_RB_STR_TAINTED_ASCII_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_ascii8bit_encoding()))


typedef enum {
  string_encoding_external = 0,
  string_encoding_utf8,
  string_encoding_ascii
} enum_string_encoding;


/* Exported Ruby symbols. */

VALUE symbol_encoding_external;
VALUE symbol_encoding_utf8;
VALUE symbol_encoding_ascii;


void init_rb_utilities(void);

VALUE ae_rb_str_new(char* ptr, long len, enum_string_encoding enc, int tainted);
VALUE ae_get_rb_encoding(enum_string_encoding encoding);


#endif  /* _AE_RB_UTILITIES_H */