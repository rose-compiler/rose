/* jconfig.h.  Generated automatically by configure.  */
/* jconfig.cfg --- source file edited by configure script */
/* see jconfig.doc for explanations */

#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
#undef void
#undef const
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H
#endif
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS
#undef NEED_SHORT_EXTERNAL_NAMES
/* Define this if you get warnings about undefined structures. */
#undef INCOMPLETE_TYPES_BROKEN

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED
#define INLINE /**/
/* These are for configuring the JPEG memory manager. */
#undef DEFAULT_MAX_MEM
#undef NO_MKTEMP

#endif /* JPEG_INTERNALS */

#ifdef JPEG_CJPEG_DJPEG

#define BMP_SUPPORTED		/* BMP image file format */
#define GIF_SUPPORTED		/* GIF image file format */
#define PPM_SUPPORTED		/* PBMPLUS PPM/PGM image file format */
#undef RLE_SUPPORTED		/* Utah RLE image file format */
#define TARGA_SUPPORTED		/* Targa image file format */

#undef TWO_FILE_COMMANDLINE
#undef NEED_SIGNAL_CATCHER
#undef DONT_USE_B_MODE

/* Define this if you want percent-done progress reports from cjpeg/djpeg. */
#undef PROGRESS_REPORT

#endif /* JPEG_CJPEG_DJPEG */

/*
 * PDFlib GmbH: overrides define in jmemmgr.c
 * Otherwise program crashes because of not aligned pointers
 */
#if defined __ILEC400__ && !defined AS400
#define AS400
#endif
#if defined(AS400)
#ifndef ALIGN_TYPE              /* overrides define in jmemmgr.c */
#define ALIGN_TYPE  char *
#endif
#endif

/*
 * PDFlib GmbH:  we allow the use of PDFlib inside of programs using
 * another instance of libjpeg
 */
#define jpeg_abort_compress	 pdf_jpeg_abort_compress
#define jpeg_CreateCompress	 pdf_jpeg_CreateCompress
#define jpeg_destroy_compress	 pdf_jpeg_destroy_compress
#define jpeg_finish_compress	 pdf_jpeg_finish_compress
#define jpeg_suppress_tables	 pdf_jpeg_suppress_tables
#define jpeg_write_marker	 pdf_jpeg_write_marker
#define jpeg_write_m_byte	 pdf_jpeg_write_m_byte
#define jpeg_write_m_header	 pdf_jpeg_write_m_header
#define jpeg_write_tables	 pdf_jpeg_write_tables
#define jpeg_start_compress	 pdf_jpeg_start_compress
#define jpeg_write_raw_data	 pdf_jpeg_write_raw_data
#define jpeg_write_scanlines	 pdf_jpeg_write_scanlines
#define jinit_c_coef_controller	 pdf_jinit_c_coef_controller
#define jinit_color_converter	 pdf_jinit_color_converter
#define jinit_forward_dct	 pdf_jinit_forward_dct
#define jinit_huff_encoder	 pdf_jinit_huff_encoder
#define jpeg_gen_optimal_table	 pdf_jpeg_gen_optimal_table
#define jpeg_make_c_derived_tbl	 pdf_jpeg_make_c_derived_tbl
#define jinit_compress_master	 pdf_jinit_compress_master
#define jinit_c_main_controller	 pdf_jinit_c_main_controller
#define jinit_marker_writer	 pdf_jinit_marker_writer
#define jinit_c_master_control	 pdf_jinit_c_master_control
#define jpeg_abort	 pdf_jpeg_abort
#define jpeg_alloc_huff_table	 pdf_jpeg_alloc_huff_table
#define jpeg_alloc_quant_table	 pdf_jpeg_alloc_quant_table
#define jpeg_destroy	 pdf_jpeg_destroy
#define jpeg_add_quant_table	 pdf_jpeg_add_quant_table
#define jpeg_default_colorspace	 pdf_jpeg_default_colorspace
#define jpeg_quality_scaling	 pdf_jpeg_quality_scaling
#define jpeg_set_colorspace	 pdf_jpeg_set_colorspace
#define jpeg_set_defaults	 pdf_jpeg_set_defaults
#define jpeg_set_linear_quality	 pdf_jpeg_set_linear_quality
#define jpeg_set_quality	 pdf_jpeg_set_quality
#define jpeg_simple_progression	 pdf_jpeg_simple_progression
#define jinit_phuff_encoder	 pdf_jinit_phuff_encoder
#define jinit_c_prep_controller	 pdf_jinit_c_prep_controller
#define jinit_downsampler	 pdf_jinit_downsampler
#define jpeg_copy_critical_parameters	 pdf_jpeg_copy_critical_param
#define jpeg_write_coefficients	 pdf_jpeg_write_coefficients
#define jpeg_abort_decompress	 pdf_jpeg_abort_decompress
#define jpeg_consume_input	 pdf_jpeg_consume_input
#define jpeg_CreateDecompress	 pdf_jpeg_CreateDecompress
#define jpeg_destroy_decompress	 pdf_jpeg_destroy_decompress
#define jpeg_finish_decompress	 pdf_jpeg_finish_decompress
#define jpeg_has_multiple_scans	 pdf_jpeg_has_multiple_scans
#define jpeg_input_complete	 pdf_jpeg_input_complete
#define jpeg_read_header	 pdf_jpeg_read_header
#define jpeg_finish_output	 pdf_jpeg_finish_output
#define jpeg_read_raw_data	 pdf_jpeg_read_raw_data
#define jpeg_read_scanlines	 pdf_jpeg_read_scanlines
#define jpeg_start_decompress	 pdf_jpeg_start_decompress
#define jpeg_start_output	 pdf_jpeg_start_output
#define jpeg_stdio_dest	 pdf_jpeg_stdio_dest
#define jpeg_stdio_src	 pdf_jpeg_stdio_src
#define jinit_d_coef_controller	 pdf_jinit_d_coef_controller
#define jinit_color_deconverter	 pdf_jinit_color_deconverter
#define jinit_inverse_dct	 pdf_jinit_inverse_dct
#define jinit_huff_decoder	 pdf_jinit_huff_decoder
#define jpeg_fill_bit_buffer	 pdf_jpeg_fill_bit_buffer
#define jpeg_huff_decode	 pdf_jpeg_huff_decode
#define jpeg_make_d_derived_tbl	 pdf_jpeg_make_d_derived_tbl
#define jpeg_reset_huff_decode	 pdf_jpeg_reset_huff_decode
#define jinit_input_controller	 pdf_jinit_input_controller
#define jinit_d_main_controller	 pdf_jinit_d_main_controller
#define jinit_marker_reader	 pdf_jinit_marker_reader
#define jpeg_resync_to_restart	 pdf_jpeg_resync_to_restart
#define jpeg_save_markers	 pdf_jpeg_save_markers
#define jpeg_set_marker_processor	 pdf_jpeg_set_marker_processor
#define jinit_master_decompress	 pdf_jinit_master_decompress
#define jpeg_calc_output_dimensions	 pdf_jpeg_calc_output_dimensions
#define jpeg_new_colormap	 pdf_jpeg_new_colormap
#define jinit_merged_upsampler	 pdf_jinit_merged_upsampler
#define jinit_phuff_decoder	 pdf_jinit_phuff_decoder
#define jinit_d_post_controller	 pdf_jinit_d_post_controller
#define jinit_upsampler	 pdf_jinit_upsampler
#define jpeg_read_coefficients	 pdf_jpeg_read_coefficients
#define jpeg_std_error	 pdf_jpeg_std_error
#define jpeg_std_message_table	 pdf_jpeg_std_message_table
#define jpeg_fdct_float	 pdf_jpeg_fdct_float
#define jpeg_fdct_ifast	 pdf_jpeg_fdct_ifast
#define jpeg_fdct_islow	 pdf_jpeg_fdct_islow
#define jpeg_idct_float	 pdf_jpeg_idct_float
#define jpeg_idct_ifast	 pdf_jpeg_idct_ifast
#define jpeg_idct_islow	 pdf_jpeg_idct_islow
#define jpeg_idct_1x1	 pdf_jpeg_idct_1x1
#define jpeg_idct_2x2	 pdf_jpeg_idct_2x2
#define jpeg_idct_4x4	 pdf_jpeg_idct_4x4
#define jinit_memory_mgr	 pdf_jinit_memory_mgr
#define jpeg_free_large	 pdf_jpeg_free_large
#define jpeg_free_small	 pdf_jpeg_free_small
#define jpeg_get_large	 pdf_jpeg_get_large
#define jpeg_get_small	 pdf_jpeg_get_small
#define jpeg_mem_available	 pdf_jpeg_mem_available
#define jpeg_mem_init	 pdf_jpeg_mem_init
#define jpeg_mem_term	 pdf_jpeg_mem_term
#define jpeg_open_backing_store	 pdf_jpeg_open_backing_store
#define jinit_1pass_quantizer	 pdf_jinit_1pass_quantizer
#define jinit_2pass_quantizer	 pdf_jinit_2pass_quantizer
#define jcopy_block_row	 pdf_jcopy_block_row
#define jcopy_sample_rows	 pdf_jcopy_sample_rows
#define jdiv_round_up	 pdf_jdiv_round_up
#define jpeg_natural_order	 pdf_jpeg_natural_order
#define jround_up	 pdf_jround_up
#define jzero_far	 pdf_jzero_far
