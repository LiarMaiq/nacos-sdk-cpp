#ifndef __SVC_INFO_H_
#define __SVC_INFO_H_

#include <vector>
#include <list>
#include <sys/time.h>
#include "NacosString.h"
#include "constant/ConfigConstant.h"
#include "utils/url.h"
#include "utils/ParamUtils.h"
#include "naming/Instance.h"

namespace nacos{
class ServiceInfo {
private:
    //@JSONField(serialize = false)
    NacosString _jsonFromServer;

    NacosString _name;

    NacosString _groupName;

    NacosString _clusters;

    long _cacheMillis;

    //@JSONField(name = "hosts")
    std::list <Instance> _hosts;

    long _lastRefTime;

    NacosString _checksum;

    volatile bool _allIPs;

public:
    ServiceInfo() : _jsonFromServer(""), _cacheMillis(1000L), _lastRefTime(0L), _checksum(""), _allIPs(false) {
    }

    bool isAllIPs() const{
        return _allIPs;
    }

    void setAllIPs(bool allIPs) {
        _allIPs = allIPs;
    }

    explicit ServiceInfo(const NacosString &key)  : _jsonFromServer(""), _cacheMillis(1000L), _lastRefTime(0L), _checksum(""),
                                          _allIPs(false) {
        std::vector <NacosString> segs;
        ParamUtils::Explode(segs, key, ConfigConstant::SERVICE_INFO_SPLITER);

        if (segs.size() == 2) {
            setGroupName(segs[0]);
            setName(segs[1]);
        } else if (segs.size() == 3) {
            setGroupName(segs[0]);
            setName(segs[1]);
            setClusters(segs[2]);
        }
    }

    ServiceInfo(const NacosString &name, const NacosString &clusters) {
        _name = name;
        _clusters = clusters;
    }

    int ipCount() {
        return _hosts.size();
    }

    bool expired() const{
        //TODO:extract this snippet to a common util
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

        return ms - _lastRefTime > _cacheMillis;
    }

    void setHosts(std::list <Instance> &hosts) {
        _hosts = hosts;
    }

    bool isValid() {
        return _hosts.size() > 0;
    }

    NacosString getName() {
        return _name;
    }

    void setName(const NacosString &name) {
        _name = name;
    }

    NacosString getGroupName() {
        return _groupName;
    }

    void setGroupName(const NacosString &groupName) {
        _groupName = groupName;
    }

    void setLastRefTime(long lastRefTime) {
        _lastRefTime = lastRefTime;
    }

    long getLastRefTime() {
        return _lastRefTime;
    }

    NacosString getClusters() {
        return _clusters;
    }

    void setClusters(const NacosString &clusters) {
        _clusters = clusters;
    }

    long getCacheMillis() {
        return _cacheMillis;
    }

    void setCacheMillis(long cacheMillis) {
        _cacheMillis = cacheMillis;
    }

    std::list <Instance> getHosts() {
        return _hosts;
    }

    std::list <Instance> *getHostsNocopy() {
        return &_hosts;
    }

    bool validate() const{
        if (isAllIPs()) {
            return true;
        }

        //TODO: Idk what does this mean in Java, ignore in C++
        /*std::list<Instance> validHosts;
        for (std::list<Instance>::iterator it = _hosts.begin()
        it != _hosts.end(); it++)
        {
            if (it->isHealthy())
            {
                continue;
            }

            for (int i = 0; i < it->getWeight(); i++)
            {
                validHosts.push_back(*it);
            }
        }*/

        return true;
    }

    //@JSONField(serialize = false)
    NacosString getJsonFromServer() const{
        return _jsonFromServer;
    }

    void setJsonFromServer(const NacosString &jsonFromServer) {
        _jsonFromServer = jsonFromServer;
    }

    //@JSONField(serialize = false)
    NacosString getKey() const{
        return getKey(_name, _clusters);
    }

    //@JSONField(serialize = false)
    NacosString getKeyEncoded() const{
        return getKey(urlencode(_name), _clusters);
    }

    //@JSONField(serialize = false)
    static void fromKey(ServiceInfo &serviceInfo, const NacosString &key) {
        std::vector <NacosString> segs;
        ParamUtils::Explode(segs, key, ConfigConstant::SERVICE_INFO_SPLITER);

        if (segs.size() == 2) {
            serviceInfo.setGroupName(segs[0]);
            serviceInfo.setName(segs[1]);
        } else if (segs.size() == 3) {
            serviceInfo.setGroupName(segs[0]);
            serviceInfo.setName(segs[1]);
            serviceInfo.setClusters(segs[2]);
        }
    }

    //@JSONField(serialize = false)
    static NacosString getKey(const NacosString &name, const NacosString &clusters) {
        if (!ParamUtils::isBlank(clusters)) {
            return name + ConfigConstant::SERVICE_INFO_SPLITER + clusters;
        }

        return name;
    }

    //@Override
    NacosString toString() const{
        return getKey();
    }

    //!!BE CAREFUL!!
    //This function is very expensive!! call it with care!
    NacosString toInstanceString() const{
        NacosString res = "{\"lastRefTime\":" + NacosStringOps::valueOf(_lastRefTime) + " [\n";
        for (std::list<Instance>::const_iterator it = _hosts.begin();
            it != _hosts.end(); it++)
        {
            res += it->toString() + "\n";
        }

        res += "\n]}";

        return res;
    }

    NacosString getChecksum() const{
        return _checksum;
    }

    void setChecksum(const NacosString &checksum) {
        _checksum = checksum;
    }
};
}//namespace nacos

#endif