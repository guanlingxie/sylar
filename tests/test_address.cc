#include "sylar/sylar.h"
#include "sylar/address.h"
#include "sylar/endian.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test()
{
    std::vector<sylar::Address::ptr> addrs;
    bool v = sylar::Address::Lookup(addrs, "www.sylar.top");
    if(!v)
    {
        SYLAR_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }
    for(size_t i = 0;i < addrs.size(); ++i)
    {
        SYLAR_LOG_INFO(g_logger) << addrs[i]->toString();
    }
    sylar::IPAddress::ptr qq = sylar::IPAddress::LookupAnyIPAddress("www.sylar.top");
    if(v)
        SYLAR_LOG_INFO(g_logger) << qq->toString();

}

void test_iface()
{
    std::multimap<std::string, std::pair<sylar::Address::ptr, uint32_t>> results;
    sylar::Address::GetInterfaceAddresses(results);
    for(auto &i : results)
    {
        SYLAR_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString() << " - " << i.second.second;
    }

}

void test_ipv4()
{
    //std::cout << sylar::IPv4Address::Create("192.168.56.129")->toString();
    uint32_t ipaddress = 0;
    ipaddress <<= 8;
    ipaddress += 192; 
    ipaddress <<= 8;
    ipaddress += 168; 
    ipaddress <<= 8;
    ipaddress += 56; 
    ipaddress <<= 8;
    ipaddress += 129; 
    sylar::IPv4Address v4("192.168.56.129", 65533);
    std::cout << v4.toString();
}
int main()
{
    test_ipv4();
    //test_iface();
}

