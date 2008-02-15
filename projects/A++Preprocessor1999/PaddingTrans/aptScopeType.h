//-*-Mode: C++;-*-
#ifndef _aptScopeType_h_
#define _aptScopeType_h_


// CW: types of scopes
// arrayReferencs should only be added for
// loop scopes
enum aptScopeType
{
	aptGlobalScopeTag,
	aptLoopScopeTag,
	aptNonLoopScopeTag
};

#endif
