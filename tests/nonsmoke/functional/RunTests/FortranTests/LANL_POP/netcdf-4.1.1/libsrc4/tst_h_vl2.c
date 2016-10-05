/* This is part of the netCDF package.  Copyright 2005 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use.

   This program excersizes HDF5 variable length array code.

   $Id: tst_h_vl2.c,v 1.3 2008/12/01 15:53:06 russ Exp $
*/
#include <nc_tests.h>

#define FILE_NAME "tst_vl.nc"
int
main()
{
   printf("\n*** Checking HDF5 VLEN types even more.\n");
#ifdef EXTRA_TESTS
   printf("*** Reading reference file...");
   {
      hid_t fileid, grpid, fapl_id, hdf_typeid, base_hdf_typeid, attid = 0;
      hid_t file_typeid, spaceid, native_typeid;
      H5O_info_t obj_info;
      char obj_name[NC_MAX_HDF5_NAME + 1];
      hsize_t num_obj, i;
      int obj_class;
      H5T_class_t class;
      size_t size, type_size;
      hssize_t att_npoints;
      int att_ndims;
      hsize_t dims[1]; 
      void *vldata;

      /* Open the file and read the vlen data. */
      if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0) ERR;
      if (H5Pset_fclose_degree(fapl_id, H5F_CLOSE_SEMI)) ERR;
      if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0) ERR;

      if ((fileid = H5Fopen(FILE_NAME, H5F_ACC_RDONLY, fapl_id)) < 0) ERR;
      if ((grpid = H5Gopen(fileid, "/")) < 0) ERR;

      if (H5Gget_num_objs(grpid, &num_obj) < 0) ERR;
      for (i = 0; i < num_obj; i++)
      {
	 if (H5Oget_info_by_idx(grpid, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, 
				i, &obj_info, H5P_DEFAULT) < 0) ERR;
	 obj_class = obj_info.type;
	 if ((size = H5Lget_name_by_idx(grpid, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, i,
					NULL, 0, H5P_DEFAULT)) < 0) ERR;
	 if (size > NC_MAX_NAME) ERR;
	 if (H5Lget_name_by_idx(grpid, ".", H5_INDEX_CRT_ORDER, H5_ITER_INC, i,
				obj_name, size+1, H5P_DEFAULT) < 0) ERR;
	 printf("nc4_rec_read_metadata: encountered HDF5 object obj_class %d obj_name %s.\n", 
		obj_class, obj_name);
	 /* Deal with groups and datasets. */
	 switch (obj_class)
	 {
	    case H5O_TYPE_GROUP:
	       printf("group\n");
	       break;
	    case H5O_TYPE_DATASET:
	       printf("dataset\n");
	       break;
	    case H5O_TYPE_NAMED_DATATYPE:
	       printf("type\n");
	       if ((hdf_typeid = H5Topen2(grpid, obj_name, H5P_DEFAULT)) < 0) ERR;
	       if ((class = H5Tget_class(hdf_typeid)) < 0) ERR;
	       switch (class)
	       {
		  case H5T_VLEN:
		     /* Find the base type of this vlen (i.e. what is this a * vlen of?) and its size. */
		     if (!(base_hdf_typeid = H5Tget_super(hdf_typeid))) ERR;
		     if (!(type_size = H5Tget_size(base_hdf_typeid))) ERR;
		     break;
		  default:
		     LOG((0, "unknown datatype class"));
		     return NC_EBADCLASS;
	       }

	       break;
	    case H5G_LINK:
	       break;
	    default:
	       LOG((0, "Unknown object class %d in nc4_rec_read_metadata!",
		    obj_class));
	 } 
      }

      num_obj = H5Aget_num_attrs(grpid);
      for (i = 0; i < num_obj; i++)
      {
	 if (attid > 0) 
	    H5Aclose(attid);
	 if ((attid = H5Aopen_idx(grpid, (unsigned int)i)) < 0) ERR;
	 if (H5Aget_name(attid, NC_MAX_NAME + 1, obj_name) < 0) ERR;
	 LOG((4, "reading attribute of _netCDF group, named %s", obj_name));
	 if ((attid && H5Aclose(attid))) ERR;      
      }

      /* Open the HDF5 attribute. */
      if ((attid = H5Aopen_name(grpid, obj_name)) < 0) ERR;

      /* Get type of attribute in file. */
      if ((file_typeid = H5Aget_type(attid)) < 0) ERR;
      if ((class = H5Tget_class(file_typeid)) < 0) ERR;
      if (class != H5T_VLEN) ERR;

      /* Get len. */
      if ((spaceid = H5Aget_space(attid)) < 0) ERR;
      if ((att_ndims = H5Sget_simple_extent_ndims(spaceid)) < 0) ERR;
      if (att_ndims != 1) ERR;
      if ((att_npoints = H5Sget_simple_extent_npoints(spaceid)) < 0) ERR;
      if (H5Sget_simple_extent_dims(spaceid, dims, NULL) < 0) ERR;
      if (dims[0] != 3) ERR;
      
      if ((native_typeid = H5Tget_native_type(file_typeid, H5T_DIR_DEFAULT)) < 0) ERR;
      if (!(vldata = malloc((unsigned int)(dims[0] * sizeof(hvl_t))))) ERR;
      if (H5Aread(attid, native_typeid, vldata) < 0) ERR;
      if (H5Tclose(native_typeid) < 0) ERR;
      if (H5Tclose(file_typeid) < 0) ERR;

      if (H5Aclose(attid) ||
	  H5Tclose(hdf_typeid) < 0 || 
	  H5Tclose(base_hdf_typeid) < 0 || 
	  H5Gclose(grpid) < 0 || 
	  H5Fclose(fileid) < 0) ERR;
   }

   SUMMARIZE_ERR;
#endif

   FINAL_RESULTS;
}
