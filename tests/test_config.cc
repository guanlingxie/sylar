#include "sylar/config.h"
#include "sylar/log.h"
#include "sylar/util.h"

#include <yaml-cpp/yaml.h>
#include <iostream>

sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port",(int)8080,"system port");
sylar::ConfigVar<float>::ptr g_float_value_config =
    sylar::Config::Lookup("system.value",(float)10.2f,"system value");
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
     sylar::Config::Lookup("system.int_vec",std::vector<int>{1,2},"system int vec");
sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config =
     sylar::Config::Lookup("system.int_list",std::list<int>{1,2},"system int list");
sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
    sylar::Config::Lookup("system.int_set",std::set<int>{1,2},"system int set");
sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_unordered_set_value_config = 
    sylar::Config::Lookup("system.int_unordered_set",std::unordered_set<int>{1,2},"system int unordered_set");
sylar::ConfigVar<std::map<std::string, int>>::ptr g_int_map_value_config = 
    sylar::Config::Lookup("system.int_map",std::map<std::string, int>{{"1",1},{"2",2}},"system int map");
sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_unordered_map_value_config = 
    sylar::Config::Lookup("system.int_unordered_map",std::unordered_map<std::string, int>{{"4",1},{"5",2}},"system int unordered_map");

void print_yaml(const YAML::Node &node,int level)
{
    if(node.IsScalar())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    }else if(node.IsNull())
    {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT())   << std::string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level;
    }else if(node.IsMap())
    {
        for(auto it = node.begin();it != node.end();++it)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT())  << std::string(level * 4, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second,level + 1);
        }
    }else if(node.IsSequence())
    {
        for(size_t i = 0;i < node.size();++i)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT())  << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_config()
{
    
    
#define XX(g_var, name, prefix) \
    {\
        auto &v = g_var->getValue();\
        for(auto &i : v)\
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": "<< i;\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "\n" << g_var->toString();\
    }

#define XXMAP(g_var, name, prefix) \
    {\
        auto &v = g_var->getValue();\
        for(auto &i : v)\
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": "<< i.first << ":" << i.second;\
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "\n" << g_var->toString();\
    }
    //XX(g_int_vec_value_config,int_vec,before)
    //XX(g_int_list_value_config,int_list,before)
    //XX(g_int_set_value_config,int_set,before);
    //XX(g_int_unordered_set_value_config,int_unordered_set,before)
    //XXMAP(g_int_map_value_config,int_map,before)
    //XXMAP(g_int_unordered_map_value_config,int_unordered_map,before);
    YAML::Node root = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);
    
    
    //XX(g_int_vec_value_config,int_vec,after)
    //XX(g_int_list_value_config,int_list,after)
    //XX(g_int_set_value_config,int_set,after);
    //XX(g_int_unordered_set_value_config,int_unordered_set,after)
    //XXMAP(g_int_map_value_config,int_map,after)
    //XXMAP(g_int_unordered_map_value_config,int_unordered_map,after);

}
void test_yaml()
{
    YAML::Node node = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/log.yml");
    print_yaml(node , 0);
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root;

}


class Person
{
public:
    std::string m_name = "";
    int m_age = 0;
    bool m_sex = 0;
    std::string toString () const
    {
        std::stringstream ss;
        ss  << "[Person name = " << m_name  
            << " age = " << m_age
            << " sex = " << m_sex
            << "]";
        return ss.str();
    }
    bool operator==(const Person &p) const
    {
        return m_name == p.m_name && m_age == p.m_age && m_sex == p.m_sex;
    }
};
sylar::ConfigVar<Person>::ptr g_person_config =
    sylar::Config::Lookup("class.person",Person(),"class person");


void test_class()
{
    // g_person_config->addListener([](const Person &old_value, const Person &new_value){
    //     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "\n" << old_value.toString() << "\n" << new_value.toString();
    // });
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:" << g_person_config->getValue().toString()  << "-" << g_person_config->toString();
    YAML::Node root = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/config_log.yml");
    sylar::Config::LoadFromYaml(root);
    
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before:" << g_person_config->getValue().toString()  << "-" << g_person_config->toString();
}

namespace sylar{

template<>
class LexicalCast<std::string,Person>
{
public:
    Person operator()(const std::string &v)
    {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string>
{
public:
    std::string operator()(const Person &p)
    {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

}
void test_log()
{
    //std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << "\n\n\n" << std::endl;
    YAML::Node root = YAML::LoadFile("/home/xitong/sylar/workspace/sylar/bin/conf/config_log.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;
}

int main()
{
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->toString();
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->getValue();
    //test_yaml();

    //test_config();
    //test_class();
    //test_log();
    return 0;
}