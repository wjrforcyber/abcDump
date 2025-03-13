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
typedef struct aigNodeLevelDis_ aigNodeLevelDis;
typedef struct aigFanoutDis_ aigFanoutDis;

struct aigData_ {
    char sCINumName[20];
    char sCONumName[20];
    char sDesignName[500];
    char sLevelName[20];
    char sNodeNumName[20];
    char sEdgesName[20];
    char sFanoutDisName[20];
    char sLevelNodeDis[20];
};

/*
We are using percentage of the max level to collect node number in each part instead of the exact level number, since the whole level can be huge.
*/
struct aigNodeLevelDis_ {
    char sMaxLevelName[50];
    char sMaxLevel25per[50];
    char sMaxLevel50per[50];
    char sMaxLevel75per[50];
    char sMaxLevel100per[50];
};

struct aigFanoutDis_ {
    char sFanoutMaxName[50];
    char sFanout25per[50];
    char sFanout50per[50];
    char sFanout75per[50];
    char sFanout100per[50];
};
ABC_NAMESPACE_HEADER_END

#endif