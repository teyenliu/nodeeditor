#ifndef HELLOWORLDDATASPLDCPS_H
#define HELLOWORLDDATASPLDCPS_H

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>
#include <v_copyIn.h>

#include "HelloWorldData.h"


extern c_metaObject __HelloWorldData_HelloWorldData__load (c_base base);

extern const char *HelloWorldData_Msg_metaDescriptor[];
extern const c_ulong HelloWorldData_Msg_metaDescriptorArrLength;
extern const c_ulong HelloWorldData_Msg_metaDescriptorLength;
extern c_metaObject __HelloWorldData_Msg__load (c_base base);
struct _HelloWorldData_Msg ;
extern  v_copyin_result __HelloWorldData_Msg__copyIn(c_type dbType, const class HelloWorldData::Msg *from, struct _HelloWorldData_Msg *to);
extern  void __HelloWorldData_Msg__copyOut(const void *_from, void *_to);
struct _HelloWorldData_Msg {
    c_long userID;
    c_string message;
};

#undef OS_API
#endif
