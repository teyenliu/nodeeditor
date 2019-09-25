#ifndef ISOCPP2_HELLOWORLDIMG_H
#define ISOCPP2_HELLOWORLDIMG_H

#include "dds/dds.hpp"

#include "HelloWorldImg.h"
#include "HelloWorldImgSplDcps.h"

#include "org/opensplice/topic/TopicTraits.hpp"
#include "org/opensplice/topic/DataRepresentation.hpp"

namespace org { namespace opensplice { namespace topic {
template <>
class TopicTraits<HelloWorldImg::Image>
{
public:
    static ::org::opensplice::topic::DataRepresentationId_t getDataRepresentationId()
    {
        return ::org::opensplice::topic::OSPL_REPRESENTATION;
    }

    static ::std::vector<os_uchar> getMetaData()
    {
        return ::std::vector<os_uchar>();
    }

    static ::std::vector<os_uchar> getTypeHash()
    {
        return ::std::vector<os_uchar>();
    }

    static ::std::vector<os_uchar> getExtentions()
    {
        return ::std::vector<os_uchar>();
    }

    static const char *getKeyList()
    {
        return "imgID";
    }

    static const char *getTypeName()
    {
        return "HelloWorldImg::Image";
    }

    static std::string getDescriptor()
    {
        const char *elements[] = {
            "<MetaData version=\"1.0.0\"><Module name=\"HelloWorldImg\"><Struct name=\"RGB\"><Member name=\"r\"><Octet/>",
"</Member><Member name=\"g\"><Octet/></Member><Member name=\"b\"><Octet/></Member></Struct><Struct name=\"Image\">",
"<Member name=\"imgID\"><Long/></Member><Member name=\"category\"><String/></Member><Member name=\"matrix\">",
"<Array size=\"224\"><Array size=\"224\"><Type name=\"RGB\"/></Array></Array></Member></Struct></Module>",
"</MetaData>"
        };
        std::string descriptor;
        descriptor.reserve(441);
        for (int i = 0; i < 5; i++) {
            descriptor.append(elements[i]);
        }

        return descriptor;
    }

    static copyInFunction getCopyIn()
    {
        return (copyInFunction) __HelloWorldImg_Image__copyIn;
    }

    static copyOutFunction getCopyOut()
    {
        return (copyOutFunction) __HelloWorldImg_Image__copyOut;
    }
};
}}}

namespace dds { namespace topic {
template <>
struct topic_type_name<HelloWorldImg::Image>
{
    static std::string value()
    {
        return org::opensplice::topic::TopicTraits<HelloWorldImg::Image>::getTypeName();
    }
};
}}

REGISTER_TOPIC_TYPE(HelloWorldImg::Image)

#endif /* ISOCPP2_HELLOWORLDIMG_H */
