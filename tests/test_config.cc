#include "sylar/config.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include <yaml-cpp/yaml.h>

sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port",(int)8080,"system port");
sylar::ConfigVar<float>::ptr g_float_value_config =
    sylar::Config::Lookup("system.port",(float)10.2f,"system port");

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/log.yml");
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;
}
int main()
{
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->toString();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->getValue();


    return 0;
}