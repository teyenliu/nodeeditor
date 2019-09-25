#include "HelloWorldImg_DCPS.hpp"

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

v_copyin_result
__HelloWorldImg_RGB__copyIn(
    c_type dbType,
    const class ::HelloWorldImg::RGB *from,
    struct _HelloWorldImg_RGB *to)
{
    v_copyin_result result = V_COPYIN_RESULT_OK;
    (void) dbType;

    to->r = (c_octet)from->r();
    to->g = (c_octet)from->g();
    to->b = (c_octet)from->b();
    return result;
}

v_copyin_result
__HelloWorldImg_Image__copyIn(
    c_type dbType,
    const class ::HelloWorldImg::Image *from,
    struct _HelloWorldImg_Image *to)
{
    v_copyin_result result = V_COPYIN_RESULT_OK;
    (void) dbType;

    to->imgID = (c_long)from->imgID();
    to->category = c_stringNew(c_getBase(dbType), from->category_.c_str());
    {
        typedef struct _HelloWorldImg_RGB _DestType[224][224];
        _DestType *dest = &to->matrix;
        c_type type0 = c_memberType(c_structureMember(c_structure(dbType), 2));
        while(c_baseObjectKind(type0) == M_COLLECTION && c_collectionTypeKind(type0) == OSPL_C_ARRAY) {;
            type0 = c_collectionTypeSubType(type0);
        }
        int i1;
        int i2;

        for (i1 = 0; (i1 < 224) && result; i1++) {
            for (i2 = 0; (i2 < 224) && result; i2++) {
                extern v_copyin_result __HelloWorldImg_RGB__copyIn(c_type dbType,
                    const ::HelloWorldImg::RGB *From,
                    struct _HelloWorldImg_RGB *To);

                if(V_COPYIN_RESULT_IS_OK(result)){
                    result = __HelloWorldImg_RGB__copyIn(type0, (::HelloWorldImg::RGB *)&((from->matrix()).data())[i1][i2], (struct _HelloWorldImg_RGB *)&(*dest)[i1][i2]);
                }
            }
        }
    }
    return result;
}

void
__HelloWorldImg_RGB__copyOut(
    const void *_from,
    void *_to)
{
    const struct _HelloWorldImg_RGB *from = (const struct _HelloWorldImg_RGB *)_from;
    class ::HelloWorldImg::RGB *to = (class ::HelloWorldImg::RGB *)_to;
    to->r((uint8_t)from->r);
    to->g((uint8_t)from->g);
    to->b((uint8_t)from->b);
}

void
__HelloWorldImg_Image__copyOut(
    const void *_from,
    void *_to)
{
    const struct _HelloWorldImg_Image *from = (const struct _HelloWorldImg_Image *)_from;
    class ::HelloWorldImg::Image *to = (class ::HelloWorldImg::Image *)_to;
    to->imgID((int32_t)from->imgID);
    to->category(from->category ? from->category : "");
    {
        typedef struct _HelloWorldImg_RGB _DestType[224][224];
        const _DestType *src = &from->matrix;

        dds::core::array< dds::core::array< ::HelloWorldImg::RGB, 224 >, 224 > *dst = &to->matrix();

        int i1;
        int i2;

        for (i1 = 0; (i1 < 224); i1++) {
            for (i2 = 0; (i2 < 224); i2++) {
                extern void __HelloWorldImg_RGB__copyOut(const void *, void *);
                __HelloWorldImg_RGB__copyOut((const void *)&(*src)[i1][i2], (void *)&(*dst)[i1][i2]);
            }
        }
    }
}

