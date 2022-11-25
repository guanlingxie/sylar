#include "config.h"

namespace sylar
{


ConfigVarBase::ptr Config::LookupBase(const std::string &name)
{
    RWMutexType::ReadLock lock(GetMutex());
    auto it = Config::GetDatas().find(name);
    return it == Config::GetDatas().end() ? nullptr : it->second;
}
/*
"A.B"
A: 
    B:
    C:
*/
static void ListAllMember(  const std::string &prefix,
                            const YAML::Node &node, 
                            std::list<std::pair<std::string,const YAML::Node>> &output)
{
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos)
    {
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return ;
    }
    //SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << prefix << "***";
    output.push_back(std::make_pair(prefix,node));
    if(node.IsMap())
    {
        for(auto it = node.begin(); it != node.end(); ++it)
        {
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second,output);
        }
        
    }
}                                

void Config::LoadFromYaml(const YAML::Node &root)
{

    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);
    for(auto &i : all_nodes)
    {
        std::string key = i.first;
        if(key.empty())
            continue;
        std::transform(key.begin(),key.end(),key.begin(),::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        if(var)
        {
            if(i.second.IsScalar())
            {
                var->fromString(i.second.Scalar());
            }else{
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
    //std::cout << sylar::LoggerMgr::GetInstance()->toYamlString();
}

void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb)
{
    RWMutexType::ReadLock lock(GetMutex());
    ConfigVarMap &m = GetDatas();
    for(auto it = m.begin();it != m.end();++it)
    {
        cb(it->second);
    }
}

}