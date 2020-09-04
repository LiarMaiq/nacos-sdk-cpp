#ifndef __PROPERTIES_H_
#define __PROPERTIES_H_
#include <map>
#include "NacosString.h"

class Properties : public std::map<NacosString, NacosString>
{
public:
    NacosString &toString() const
    {
        NacosString content = "";
        for (std::map<NacosString, NacosString>::const_iterator it = begin(); it != end(); it++)
        {
            content += (it->first + "=" + it->second + "\n");
        }
        return content;
    }
};

#endif