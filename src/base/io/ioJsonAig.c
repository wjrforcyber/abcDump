#include "aig/gia/gia.h"
#include "aig/aig/aig.h"
#include "base/abc/abc.h"
#include "ioJsonAig.h"
#include "json.h"
#include "json_object.h"
#include "misc/vec/vecPtr.h"
#include "misc/vec/vecVec.h"
#include <assert.h>
#include <string.h>
ABC_NAMESPACE_IMPL_START

void AigDataInit(aigData *data)
{
    strcpy(data->sCINumName, "CINumber");
    strcpy(data->sCONumName, "CONumber");
    strcpy(data->sDesignName, "DesignName");
    strcpy(data->sLevelName, "LevelNum");
    strcpy(data->sNodeNumName, "NodeNum");
    strcpy(data->sEdgesName, "EdgesNum");
    strcpy(data->sLevelNodeDis, "NodeDis");
    strcpy(data->sFanoutDisName, "FanoutDis");
}

void AigNodeLevelDisInit(aigNodeLevelDis *data)
{
    strcpy(data->sMaxLevelName, "MaxLevel");
    strcpy(data->sMaxLevel25per, "0% - 25%MaxLevel");
    strcpy(data->sMaxLevel50per, "25% - 50%MaxLevel");
    strcpy(data->sMaxLevel75per, "50% - 75%MaxLevel");
    strcpy(data->sMaxLevel100per, "75% - 100%MaxLevel");
}

void AigFanoutDisInit(aigFanoutDis *data)
{
    strcpy(data->sFanoutMaxName, "MaxFanout");
    strcpy(data->sFanout25per, "0% - 25%MaxFanout");
    strcpy(data->sFanout50per, "25% - 50%MaxFanout");
    strcpy(data->sFanout75per, "50% - 75%MaxFanout");
    strcpy(data->sFanout100per, "0% - 100%MaxFanout");
}

int AigNodeInLevel(Aig_Man_t *aig, int iLevel)
{
    Vec_Vec_t * vLevels = Aig_ManLevelize(aig);
    assert(Vec_VecEntry(vLevels, iLevel) != NULL);
    return Vec_PtrSize(Vec_VecEntry(vLevels, iLevel));
}

/*
return the totoal count of the node number in the according percentage
If given 0.25 then 0.00-0.25, if given 0.50 then 0.25-0.50.
*/
int AigLevelNodeCollect(Aig_Man_t *aig, float fPercent, int nLevel)
{
    int nTotalCount = 0;
    int nStartLevel = nLevel * (fPercent - 0.25);
    int nEndLevel = nLevel * fPercent;
    for(int iLevel = nStartLevel; iLevel < nEndLevel; iLevel++)
    {
        nTotalCount += AigNodeInLevel(aig, iLevel);
    }
    return nTotalCount;
}

void AigNodeLevelDisCollect(Aig_Man_t *aig, aigNodeLevelDis *aigNodeLevelName, json_object *nodeLevelDis)
{
    AigNodeLevelDisInit(aigNodeLevelName);
    int nMaxLevel = Aig_ManLevelNum(aig);
    json_object_object_add(nodeLevelDis, aigNodeLevelName->sMaxLevelName, json_object_new_int(nMaxLevel));
    json_object_object_add(nodeLevelDis, aigNodeLevelName->sMaxLevel25per, json_object_new_int( AigLevelNodeCollect(aig, 0.25, nMaxLevel)));
    json_object_object_add(nodeLevelDis, aigNodeLevelName->sMaxLevel50per, json_object_new_int( AigLevelNodeCollect(aig, 0.50, nMaxLevel)));
    json_object_object_add(nodeLevelDis, aigNodeLevelName->sMaxLevel75per, json_object_new_int( AigLevelNodeCollect(aig, 0.75, nMaxLevel)));
    json_object_object_add(nodeLevelDis, aigNodeLevelName->sMaxLevel100per, json_object_new_int( AigLevelNodeCollect(aig, 1.00, nMaxLevel)));
}

//collect all edges, this could also be done when collecting fanout distribution, but I'll split it here for easier use.
int AigEdgesCollect(Aig_Man_t *aig, int iCollectPi)
{
    int nTotalCount = 0;
    Aig_Obj_t * pObj ,* pFanout;
    int i = 0;
    Aig_ManFanoutStart(aig);
    Aig_ManForEachNode(aig, pObj, i)
    {
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nTotalCount += 1;
        }
    }
    if(iCollectPi == 1)
    {
        Aig_ManForEachCi(aig, pObj, i)
        {
            int iFan = -1;
            int j = 0;
            Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
            {
                nTotalCount += 1;
            }
        }
    }
    Aig_ManFanoutStop(aig);
    return nTotalCount;
}

//collect fanout info in one pass
void AigFanoutInfo(Aig_Man_t *aig, int *n25, int *n50, int *n75, int *n100, int *nMax, int iCollectPi)
{
    Aig_Obj_t * pObj, * pFanout;
    int i = 0;
    //TODO: @Jingren Wang Try use the HashInt here
    Aig_ManFanoutStart(aig);
    *nMax = 0;
    *n25 = *n50 = *n75 = *n100 = 0;
    Aig_ManForEachNode(aig, pObj, i)
    {
        int nCountEachObj = 0;
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nCountEachObj += 1;
        }
        if(nCountEachObj > *nMax)
        {
            *nMax = nCountEachObj;
        }
    }
    if(iCollectPi)
    {
        int nCountEachObj = 0;
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nCountEachObj += 1;
        }
        if(nCountEachObj > *nMax)
        {
            *nMax = nCountEachObj;
        }
    }
    Aig_ManForEachNode(aig, pObj, i)
    {
        int nCountEachObj = 0;
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nCountEachObj += 1;
        }
        if(nCountEachObj < 0.25*(*nMax))
        {
            (*n25)++;
        }
        else if(nCountEachObj < 0.5*(*nMax))
        {
            (*n50)++;
        }
        else if(nCountEachObj < 0.75*(*nMax))
        {
            (*n75)++;
        }
        else{
            (*n100)++;
        }
    }
    if(iCollectPi)
    {
        int nCountEachObj = 0;
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nCountEachObj += 1;
        }
        if(nCountEachObj < 0.25*(*nMax))
        {
            (*n25)++;
        }
        else if(nCountEachObj < 0.5*(*nMax))
        {
            (*n50)++;
        }
        else if(nCountEachObj < 0.75*(*nMax))
        {
            (*n75)++;
        }
        else{
            (*n100)++;
        }
    }
    Aig_ManFanoutStop(aig);
}


void AigNodeFanoutDisCollect(Aig_Man_t *aig, aigFanoutDis *aigFanoutDisName, json_object *fanoutDis, int iCollectPi)
{
    AigFanoutDisInit(aigFanoutDisName);
    int n25, n50, n75, n100, nMax;
    AigFanoutInfo(aig, &n25, &n50, &n75, &n100, &nMax, iCollectPi);
    json_object_object_add(fanoutDis, aigFanoutDisName->sFanoutMaxName, json_object_new_int(nMax));
    json_object_object_add(fanoutDis, aigFanoutDisName->sFanout25per, json_object_new_int(n25));
    json_object_object_add(fanoutDis, aigFanoutDisName->sFanout50per, json_object_new_int(n50));
    json_object_object_add(fanoutDis, aigFanoutDisName->sFanout75per, json_object_new_int(n75));
    json_object_object_add(fanoutDis, aigFanoutDisName->sFanout100per, json_object_new_int(n100));
}

//Fanouts collect
int AigFanoutCollect(Aig_Man_t *aig, int iCollectPi)
{
    int nTotalCount = 0;
    Aig_Obj_t * pObj ,* pFanout;
    int i = 0;
    Aig_ManFanoutStart(aig);
    Aig_ManForEachNode(aig, pObj, i)
    {
        int iFan = -1;
        int j = 0;
        Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
        {
            nTotalCount += 1;
        }
    }
    if(iCollectPi == 1)
    {
        Aig_ManForEachCi(aig, pObj, i)
        {
            int iFan = -1;
            int j = 0;
            Aig_ObjForEachFanout( aig, pObj, pFanout, iFan, j )
            {
                nTotalCount += 1;
            }
        }
    }
    Aig_ManFanoutStop(aig);
    return nTotalCount;
}

int AigJsonDump(Aig_Man_t *aig, Gia_Man_t *gia, const char * filename, int iCollectPi)
{
    aigData dataLabel;
    AigDataInit(&dataLabel);
    
    json_object *root = json_object_new_object();
    if (!root){
        return 1;
    }
    //add the CI number
    json_object_object_add(root, dataLabel.sCINumName, json_object_new_int(Aig_ManCiNum(aig)));

    //add the CO number
    json_object_object_add(root, dataLabel.sCONumName, json_object_new_int(Aig_ManCoNum(aig)));

    //add the design name
    json_object_object_add(root, dataLabel.sDesignName, json_object_new_string(aig->pName));

    //add the level number
    json_object_object_add(root, dataLabel.sLevelName, json_object_new_int( Aig_ManLevelNum(aig)));

    //add the node number
    json_object_object_add(root, dataLabel.sNodeNumName, json_object_new_int( Aig_ManNodeNum(aig)));

    //add the edges number
    json_object_object_add(root, dataLabel.sEdgesName, json_object_new_int( AigEdgesCollect(aig, iCollectPi)));

    //node distribution array
    json_object *nodeInfo = json_object_new_array();
    json_object_object_add(root, "NodeLevelInfo", nodeInfo);

    aigNodeLevelDis aigNodeLevelName;
    json_object *nodeLevelDis = json_object_new_object();
    if (!nodeLevelDis){
        return 1;
    }
    AigNodeLevelDisCollect(aig, &aigNodeLevelName, nodeLevelDis);
    json_object_array_add(nodeInfo, nodeLevelDis);

    //Fanout distribution array
    //node distribution array
    json_object *fanoutInfo = json_object_new_array();
    json_object_object_add(root, "FanoutInfo", fanoutInfo);

    aigFanoutDis aigFanoutDis;
    json_object *fanoutsDis = json_object_new_object();
    if (!fanoutsDis){
        return 1;
    }
    AigNodeFanoutDisCollect(aig, &aigFanoutDis, fanoutsDis, iCollectPi);
    json_object_array_add(fanoutInfo, fanoutsDis);

    // print json
    printf("The json representation:\n\n%s\n\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
    // save json
    if (json_object_to_file(filename, root))
        printf("Error: failed to save %s!!\n", filename);
    else
        printf("%s saved.\n", filename);

    // cleanup and exit
    json_object_put(root);
    return 0;
}
ABC_NAMESPACE_IMPL_END