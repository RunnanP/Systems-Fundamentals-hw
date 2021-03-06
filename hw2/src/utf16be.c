#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <unistd.h>
#define UNUSED(x) (void)x   //change



//extern char *const STR_UTF16BE;
//extern char *const STR_UTF16LE;
//extern char *const STR_UTF8;
int
from_utf16be_to_utf16le(int infile, int outfile)
{
  int bom;
  utf16_glyph_t buf;
  ssize_t bytes_read;
  ssize_t bytes_to_write;
  int ret = 0;

  bom = UTF16LE;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(&bom, 2);
#endif
  write_to_bigendian(outfile, &bom, 2);




  while ((bytes_read = read_to_bigendian(infile, &(buf.upper_bytes), 2)) > 0) {
    bytes_to_write = 2; /* utf-32 future compatibility */
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse_bytes(&buf.upper_bytes, 2);
    #endif

    if(is_lower_surrogate_pair(buf)) {
      if((bytes_read = read_to_bigendian(infile, &buf.lower_bytes, 2) < 0)) {
        break;
      }



     #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      reverse_bytes(&(buf.lower_bytes), 2);
      #endif
      bytes_to_write += 2;
    }



    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
     reverse_bytes(&buf, bytes_to_write);
    #endif
    write_to_bigendian(outfile, &buf, bytes_to_write);

  }
  ret = bytes_read;
  return ret;
}



int
from_utf16be_to_utf8(int infile, int outfile)


{
  int ret = 0;
  int bom;
  utf16_glyph_t utf16_buf;
  ssize_t bytes_read;
  //ssize_t bytes_to_write;

  size_t size_of_glyph;
  code_point_t code_point;
  utf8_glyph_t utf8_buf;

  bom = UTF8;

 // int o=0;



 #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  reverse_bytes(&bom, 3);
  #endif



  write_to_bigendian(outfile, &bom, 3);

while ((bytes_read = read_to_bigendian(infile, &(utf16_buf.upper_bytes), 2)) > 0) {


 //  printf("1-%d-\n",is_upper_surrogate_pair(utf16_buf));
 //  printf("2-%d-\n",is_lower_surrogate_pair(utf16_buf));
 //   printf("3*%X*\n",utf16_buf.upper_bytes);
 //  printf("4*%X*\n",utf16_buf.lower_bytes);
    if(is_upper_surrogate_pair(utf16_buf) || (is_lower_surrogate_pair(utf16_buf))) {


       if((bytes_read = read_to_bigendian(infile, &(utf16_buf.lower_bytes), 2)> 0)) {


                code_point = utf16_glyph_to_code_point(&utf16_buf);

                utf8_buf = code_point_to_utf8_glyph(code_point, &size_of_glyph);

                 write_to_bigendian(outfile, &utf8_buf, size_of_glyph);
                 memset(&utf16_buf, 0, sizeof utf16_buf);

           //   printf("%dq\n",o);
            //  o++;



       }else{
                  code_point = utf16_glyph_to_code_point(&utf16_buf);

                utf8_buf = code_point_to_utf8_glyph(code_point, &size_of_glyph);

                write_to_bigendian(outfile, &utf8_buf, size_of_glyph);
                  memset(&utf16_buf, 0, sizeof utf16_buf);
               //   printf("%dw\n",o);
                 //  o++;


        break;
       }


     }else{

                code_point = utf16_glyph_to_code_point(&utf16_buf);

                utf8_buf = code_point_to_utf8_glyph(code_point, &size_of_glyph);

                write_to_bigendian(outfile, &utf8_buf, size_of_glyph);
                  memset(&utf16_buf, 0, sizeof utf16_buf);
              //    printf("%de\n",o);
                //   o++;


       }


    }

      ret = bytes_read;
  return ret;
}
















utf16_glyph_t
code_point_to_utf16be_glyph(code_point_t code_point, size_t *size_of_glyph)
{
  utf16_glyph_t ret;

  memset(&ret, 0, sizeof ret);
  if(is_code_point_surrogate(code_point)) {
    code_point -= 0x10000;
    ret.upper_bytes = (code_point >> 10) + 0xD800;  //+11011
    ret.lower_bytes = (code_point & 0x3FF) + 0xDC00;   //110111....
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
    reverse_bytes(&ret.lower_bytes, 2);
  #endif
    *size_of_glyph = 4;
  }
  else {
    ret.upper_bytes |= code_point;
  #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    reverse_bytes(&ret.upper_bytes, 2);
  #endif
    *size_of_glyph = 2;
  }
  return ret;
}



