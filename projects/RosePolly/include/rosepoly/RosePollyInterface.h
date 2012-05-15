
/*
 *  This file contains the declarations of all the
 *	API functions that would be directly available
 *	to the users. In other words, the ROSE polyhedral
 *	framework is defined by this interface only !
 *
 *	Athor : Athanasios Konstantinidis
 *	Date : 12-Jan-2012
 *	Lawrence Livermore National Laboratory
 */


#ifndef SG_POLLY_INTERFACE_H
#define SG_POLLY_INTERFACE_H

#include <rosepoly/RosePollyModel.h>
#include <rosepoly/RosePollyCustom.h>
#include <rosepoly/RosePluto.h>
#include <rosepoly/RoseCloog.h>
#include <rosepoly/Kernel.h>

typedef enum {
		ANNOTATED = 0,
		AUTO      = 1
}	search_type;

const string annotName = "rosePolly";

vector<RosePollyModel*> RosePollyBuildModel( SgNode * root, search_type t=ANNOTATED );

vector<RosePollyModel*> RosePollyBuildModel( SgNode * root, RosePollyCustom * c, search_type t=ANNOTATED );

RosePollyModel * RosePollyBuildModel( FlowGraph * graph );

RosePollyModel * RosePollyBuildModel( FlowGraph * graph, RosePollyCustom * c );

RosePluto * RosePollyBuildPluto( RosePollyModel * model );

RoseCloog * RosePollyBuildCloog( RosePollyModel * model );

void RosePollyTerminate();


#endif
