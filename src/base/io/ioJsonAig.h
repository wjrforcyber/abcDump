/**CFile****************************************************************

  FileName    [jsonaig.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Collect json format data from aig.]

  Synopsis    [For the purpose of easing data analysis.]

  Author      [Jingren Wang]
  
  Affiliation []

  Date        []

  Revision    []

***********************************************************************/
#ifndef ABC__base__io__ioJsonAig_h
#define ABC__base__io__ioJsonAig_h
#include "misc/util/abc_global.h"

ABC_NAMESPACE_HEADER_START
typedef struct aigData_ aigData;

struct aigData_ {
    char sDesignName[500];
    char sLevelName[20];
    char sNodeNumName[20];
};
ABC_NAMESPACE_HEADER_END

#endif