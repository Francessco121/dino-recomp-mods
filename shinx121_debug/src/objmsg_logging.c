#include "modding.h"

#include "game/objects/object.h"
#include "recomputils.h"

RECOMP_HOOK("objRecvMesg") void objRecvMesg_logging_hook(Object *obj, u32 *outMesgID, Object **outSender, void **outMesgArg) {
    if (obj == NULL) {
        return;
    }
    
    ObjectMesgQueue* mesgQueue = obj->mesgQueue;
    if (mesgQueue != NULL && mesgQueue->count != 0) {
        u32 mesgID = mesgQueue->queue[0].id;
        Object* sender = mesgQueue->queue[0].sender;
        void* mesgArg = mesgQueue->queue[0].arg;

        if (sender != NULL && !(sender->srt.flags & OBJFLAG_UNK_2)) {
            sender = NULL;
        }

        const char *senderName = (sender == NULL || sender->def == NULL) 
            ? "<unknown>" 
            : sender->def->name;
        s32 senderObjID = sender != NULL ? sender->id : -1;
        s32 senderUID = (sender != NULL && sender->setup != NULL) 
            ? sender->setup->uID 
            : -1;
        const char *receiverName = (obj->def == NULL) 
            ? "<unknown>" 
            : obj->def->name;
        s32 receiverUID = (obj->setup != NULL) 
            ? obj->setup->uID 
            : -1;

        recomp_printf("%s (%d, 0x%X) received obj message 0x%X from %s (%d, 0x%X) with arg 0x%X\n",
            receiverName, obj->id, receiverUID,
            mesgID,
            senderName, senderObjID, senderUID,
            mesgArg);
    }
}
