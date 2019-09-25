#ifndef HELLOWORLDIMGSPLDCPS_H
#define HELLOWORLDIMGSPLDCPS_H

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>
#include <v_copyIn.h>

#include "HelloWorldImg.h"


extern c_metaObject __HelloWorldImg_HelloWorldImg__load (c_base base);

extern const char *HelloWorldImg_RGB_metaDescriptor[];
extern const c_ulong HelloWorldImg_RGB_metaDescriptorArrLength;
extern const c_ulong HelloWorldImg_RGB_metaDescriptorLength;
extern c_metaObject __HelloWorldImg_RGB__load (c_base base);
struct _HelloWorldImg_RGB ;
extern  v_copyin_result __HelloWorldImg_RGB__copyIn(c_type dbType, const class HelloWorldImg::RGB *from, struct _HelloWorldImg_RGB *to);
extern  void __HelloWorldImg_RGB__copyOut(const void *_from, void *_to);
struct _HelloWorldImg_RGB {
    c_octet r;
    c_octet g;
    c_octet b;
};

extern const char *HelloWorldImg_Image_metaDescriptor[];
extern const c_ulong HelloWorldImg_Image_metaDescriptorArrLength;
extern const c_ulong HelloWorldImg_Image_metaDescriptorLength;
extern c_metaObject __HelloWorldImg_Image__load (c_base base);
struct _HelloWorldImg_Image ;
extern  v_copyin_result __HelloWorldImg_Image__copyIn(c_type dbType, const class HelloWorldImg::Image *from, struct _HelloWorldImg_Image *to);
extern  void __HelloWorldImg_Image__copyOut(const void *_from, void *_to);
struct _HelloWorldImg_Image {
    c_long imgID;
    c_string category;
    struct _HelloWorldImg_RGB matrix[224][224];
};

#undef OS_API
#endif
