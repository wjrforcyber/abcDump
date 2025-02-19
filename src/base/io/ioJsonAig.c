#include "aig/gia/gia.h"
#include "aig/aig/aig.h"
#include "ioJsonAig.h"
#include "json.h"
ABC_NAMESPACE_IMPL_START

void AigDataInit(aigData *data)
{
    strcpy(data->sDesignName, "DesignName");
    strcpy(data->sLevelName, "LevelNum");
    strcpy(data->sNodeNumName, "NodeNum");
}

int AigJsonDump(Aig_Man_t *aig, Gia_Man_t *gia, const char * filename)
{
    aigData dataLabel;
    AigDataInit(&dataLabel);
    
    json_object *root = json_object_new_object();
    if (!root){
        return 1;
    }
    
    //add the design name
    json_object_object_add(root, dataLabel.sDesignName, json_object_new_string(aig->pName));
    
    //add the level number
    json_object_object_add(root, dataLabel.sLevelName, json_object_new_int( Aig_ManLevelNum(aig)));
    
    //add the node number
    json_object_object_add(root, dataLabel.sNodeNumName, json_object_new_int( Aig_ManNodeNum(aig)));

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