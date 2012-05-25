#ifndef UTILITIES_H
#define UTILITIES_H


typedef enum {
  string_encoding_external = 0,
  string_encoding_utf8,
  string_encoding_ascii
} enum_string_encoding;


VALUE ae_rb_str_new(char* ptr, long len, enum_string_encoding enc, int tainted);


#endif  /* UTILITIES_H */