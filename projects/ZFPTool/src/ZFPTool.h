#ifndef _ZFPTOOL_H
#define _ZFPTOOL_H
#include "rose.h"
#include "rose_config.h" // obtain macros defining backend compiler names, etc.
#include "SgNodeHelper.h"
#include "AstMatching.h"
#include "AstTerm.h"

enum replace_type {nosub, onesub, twosub};

void replaceFuncArgument(SgExpressionPtrList expList, int idx);

std::vector<std::string> getFortranTargetnameList(SgNode* root);
std::vector<std::string> getTargetnameList(SgNode* root);
std::map<std::string, enum replace_type> APIInfo = 
{
{"stream_open",nosub},
{"stream_close",nosub},
{"zfp_field_alloc",nosub},
{"zfp_field_1d",nosub},
{"zfp_field_2d",nosub},
{"zfp_field_3d",nosub},
{"zfp_field_4d",nosub},
{"stream_read_bits",onesub},
{"zfp_stream_open",onesub},
{"zfp_stream_close",onesub},
{"zfp_stream_bit_stream",onesub},
{"zfp_stream_compression_mode",onesub},
{"zfp_stream_mode",onesub},
{"zfp_stream_params",onesub},
{"zfp_stream_compressed_size",onesub},
{"zfp_stream_set_rate",onesub},
{"zfp_stream_set_precision",onesub},
{"zfp_stream_set_accuracy",onesub},
{"zfp_stream_set_mode",onesub},
{"zfp_stream_set_params",onesub},
{"zfp_stream_execution",onesub},
{"zfp_stream_omp_threads",onesub},
{"zfp_stream_omp_chunk_size",onesub},
{"zfp_stream_set_execution",onesub},
{"zfp_stream_set_omp_threads",onesub},
{"zfp_stream_set_omp_chunk_size",onesub},
{"zfp_stream_flush",onesub},
{"zfp_stream_align",onesub},
{"zfp_stream_rewind",onesub},
{"zfp_field_free",onesub},
{"zfp_field_pointer",onesub},
{"zfp_field_type",onesub},
{"zfp_field_precision",onesub},
{"zfp_field_dimensionality",onesub},
{"zfp_field_size",onesub},
{"zfp_field_stride",onesub},
{"zfp_field_metadata",onesub},
{"zfp_field_set_pointer",onesub},
{"zfp_field_set_type",onesub},
{"zfp_field_set_size_1d",onesub},
{"zfp_field_set_size_2d",onesub},
{"zfp_field_set_size_3d",onesub},
{"zfp_field_set_size_4d",onesub},
{"zfp_field_set_stride_1d",onesub},
{"zfp_field_set_stride_2d",onesub},
{"zfp_field_set_stride_3d",onesub},
{"zfp_field_set_stride_4d",onesub},
{"zfp_field_set_metadata",onesub},
{"zfp_stream_set_bit_stream",twosub},
{"zfp_stream_maximum_size",twosub},
{"zfp_compress",twosub},
{"zfp_decompress",twosub},
{"zfp_write_header",twosub},
{"zfp_read_header",twosub}
};

#endif  //_ZFPTOOL_H
