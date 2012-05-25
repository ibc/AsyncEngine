#ifndef UTILITIES_H
#define UTILITIES_H


/*
 * define utilities for variable arguments number in Ruby C methods.
 * These defines assume that the Ruby C method is defined as:
 *   VALUE xxx(int argc, VALUE *argv, VALUE self)
 */
#define AE_RB_CHECK_NUM_ARGS(min,max)  \
if (argc < min || argc > max)  \
  rb_raise(rb_eArgError, "wrong number of arguments")

#define AE_RB_GET_BLOCK_OR_PROC(num_arg, save_to)  \
  if (rb_block_given_p())  \
    save_to = rb_block_proc();  \
  else if (argc >= num_arg) {  \
    save_to = argv[num_arg-1];  \
    if (! rb_obj_is_kind_of(save_to, rb_cProc))  \
      rb_raise(rb_eTypeError, "argument %d is not a Proc", num_arg);  \
    }  \
  else  \
    save_to = Qnil;


typedef enum {
  string_encoding_external = 0,
  string_encoding_utf8,
  string_encoding_ascii
} enum_string_encoding;


VALUE ae_rb_str_new(char* ptr, long len, enum_string_encoding enc, int tainted);


#endif  /* UTILITIES_H */