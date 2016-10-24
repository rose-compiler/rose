/* This is part of the netCDF package.
   Copyright 2005 University Corporation for Atmospheric Research/Unidata
   See COPYRIGHT file for conditions of use.

   Test internal netcdf-4 list code. 
   $Id: tst_lists.c,v 1.20 2009/09/14 19:42:51 ed Exp $
*/

#include "netcdf.h"
#include <../libsrc/nc.h>
#include "nc4internal.h"
#include <nc_tests.h>

extern NC_FILE_INFO_T *nc_file;

#define NAME "lalala"

int
main(int argc, char **argv)
{
   printf("\n*** Testing netcdf-4 internal list code.\n");
   printf("*** testing group list code...");
   {
      NC_GRP_INFO_T *g = NULL, *g1;
      NC_FILE_INFO_T *nc = NULL;

      /*nc_set_log_level(6);*/

      /* Add a file, then delete it. */
      if (nc4_file_list_add(0x00010000)) ERR;
      if (!(nc = nc4_find_nc_file(0x00010000))) ERR;
      nc4_file_list_del(nc);

      /* Add netcdf-4 structs, including the root group, then delete
       * them all. */
      if (nc4_file_list_add(0x00010000)) ERR;
      if (!(nc = nc4_find_nc_file(0x00010000))) ERR;
      if (nc4_nc4f_list_add(nc, NAME, NC_NETCDF4)) ERR;
      if (!nc->nc4_info) ERR;
      if (strcmp(NAME, nc->nc4_info->path)) ERR;
      nc4_rec_grp_del(&nc->nc4_info->root_grp, nc->nc4_info->root_grp);
      free(nc->nc4_info);
      nc4_file_list_del(nc);

      /* Add a child group to the root group. */
      if (nc4_file_list_add(0x00010000)) ERR;
      if (!(nc = nc4_find_nc_file(0x00010000))) ERR;
      if (nc4_nc4f_list_add(nc, NAME, NC_NETCDF4)) ERR;
      if (nc4_grp_list_add(&nc->nc4_info->root_grp->children, 1, nc->nc4_info->root_grp, 
			   nc, "hohoho", &g1)) ERR;
      if (!(g = nc4_rec_find_grp(nc->nc4_info->root_grp, 1))) ERR;
      if (g->nc_grpid != 1) ERR;
      nc4_rec_grp_del(&nc->nc4_info->root_grp, nc->nc4_info->root_grp);
      free(nc->nc4_info);
      nc4_file_list_del(nc);
   }

   SUMMARIZE_ERR;
   printf("*** testing file list code...");

   {
      NC_FILE_INFO_T *nc;

      if (nc4_find_nc_file(0)) ERR; 
      if (nc4_file_list_add(0x00000000)) ERR;
      if (nc4_file_list_add(0x00010000)) ERR;
      if (nc4_file_list_add(0x00020000)) ERR;
      if (!(nc = nc4_find_nc_file(0x00010000))) ERR;
      nc4_file_list_del(nc);
      if (nc4_find_nc_file(0x00010000)) ERR;
      if (!(nc = nc4_find_nc_file(0x00020000))) ERR;
      nc4_file_list_del(nc);
      if (nc4_find_nc_file(0x00020000)) ERR;
      if (!(nc = nc4_find_nc_file(0))) ERR;
      if (nc4_nc4f_list_add(nc, NAME, NC_NETCDF4)) ERR;
      if (!nc->nc4_info) ERR;
      if (strncmp(NAME, nc->nc4_info->path, NC_MAX_NAME)) ERR;
      if (!nc->nc4_info->root_grp) ERR;
      if (nc->nc4_info->root_grp->children) ERR;
      if (nc->nc4_info->root_grp->next) ERR;
      if (nc->nc4_info->root_grp->parent) ERR;
      if (!(nc = nc4_find_nc_file(0x00000000))) ERR;
      nc4_rec_grp_del(&nc->nc4_info->root_grp, nc->nc4_info->root_grp);
      free(nc->nc4_info);
      nc4_file_list_del(nc);
   }

   SUMMARIZE_ERR;
   printf("*** testing attribute list code...");

#define TYPE_NAME1 "crazy"
#define TYPE_NAME2 "silly"   

   {
      NC_ATT_INFO_T *att = NULL, **attlist = &att;

      if (nc4_att_list_add(attlist)) ERR;
      nc4_att_list_del(attlist, *attlist);
   }

   SUMMARIZE_ERR;
   printf("*** testing type list code...");

#define TYPE_NAME1 "crazy"
#define TYPE_NAME2 "silly"   

   {
/*      NC_TYPE_INFO_T *typelist = NULL, *type;*/

/*      if (nc4_type_list_add(&typelist, 1, 4, TYPE_NAME1)) ERR;
      if (nc4_find_type(typelist, 1, &type)) ERR;
      if (strcmp(type->name, TYPE_NAME1) || type->nc_typeid != 1) ERR;
      type_list_del(&typelist, type);
      if (nc4_type_list_add(&typelist, 2, 4, TYPE_NAME2)) ERR;
      if (nc4_find_type(typelist, 2, &type)) ERR;
      if (strcmp(type->name, TYPE_NAME2) || type->nc_typeid != 2) ERR;
      type_list_del(&typelist, type);*/
   }

   SUMMARIZE_ERR;

   /* Print out our number of errors, if any, and exit badly. */
   if (total_err)
   {
      printf("%d errors detected! Sorry!\n", total_err);
      return 2;
   }
   
   printf("*** Tests successful!\n");
   return 0;
}
