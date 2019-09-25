#include "HelloWorldData_DCPS.hpp"

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

v_copyin_result
__HelloWorldData_Msg__copyIn(
    c_type dbType,
    const class ::HelloWorldData::Msg *from,
    struct _HelloWorldData_Msg *to)
{
    v_copyin_result result = V_COPYIN_RESULT_OK;
    (void) dbType;

    to->userID = (c_long)from->userID();
    to->message = c_stringNew(c_getBase(dbType), from->message_.c_str());
    return result;
}

void
__HelloWorldData_Msg__copyOut(
    const void *_from,
    void *_to)
{
    const struct _HelloWorldData_Msg *from = (const struct _HelloWorldData_Msg *)_from;
    class ::HelloWorldData::Msg *to = (class ::HelloWorldData::Msg *)_to;
    to->userID((int32_t)from->userID);
    to->message(from->message ? from->message : "");
}

