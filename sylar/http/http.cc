#include "http.h"


namespace sylar
{
namespace http
{

HttpMethod StringToHttpMethod(const std::string &m)
{
#define XX(num, name, string)\
    if(strcmp(m.c_str(), #name) == 0)\
        return HttpMethod::name;
    HTTP_METHOD_MAP(XX)
#undef XX
    return HttpMethod::INVALID;
}

HttpMethod CharsToHttpMethod(const char *m)
{
#define XX(num, name, string)\
    if(strcmp(m, #name) == 0)\
        return HttpMethod::name;
    HTTP_METHOD_MAP(XX)
#undef XX 
    return HttpMethod::INVALID;
}

static const char *s_method_string[] = 
{
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};


const char *HttpMethodToString(const HttpMethod m)
{
    int idx = (int)m;
    if(idx >= (sizeof(s_method_string) / sizeof(s_method_string[0])))
    {
        return "INVALID_METHOD";
    }
    return s_method_string[idx];
}

const char *HttpStatusToString(const HttpStatus m)
{
    switch (m)
    {
#define XX(num, name, desc)\
    case HttpStatus::name:\
        return #desc;\
        break;
    HTTP_STATUS_MAP(XX);
#undef XX
    default:
        break;
    }
    return "INVALID_STATUS";
}

bool CaseInsensitiveLess::operator()(const std::string &lhs, const std::string &rhs) const
{
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

HttpRequest::HttpRequest(uint8_t version, bool close)
    :m_version(version)
    ,m_close(close)
    ,m_method(HttpMethod::GET)
    ,m_path("/")
{

}

std::string HttpRequest::getHeader(const std::string &key, const std::string &def) const
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
        return it->second;
    return def;
}

std::string HttpRequest::getParam(const std::string &key, const std::string &def) const
{
    auto it = m_params.find(key);
    if(it != m_params.end())
        return it->second;
    return def;
}

std::string HttpRequest::getCookie(const std::string &key, const std::string &def) const
{
    auto it = m_cookies.find(key);
    if(it != m_cookies.end())
        return it->second;
    return def;
}

void HttpRequest::setHeader(const std::string &key, const std::string &value)
{
    if(::strcasecmp(key.c_str(), "connection") == 0)
    {
        if(::strcasecmp(value.c_str(), "close") == 0)
            setClose(true);
        else if(::strcasecmp(value.c_str(), "keep-alive") == 0)
            setClose(false);
    }
    m_headers[key] = value;
}
void HttpRequest::setParam(const std::string &key, const std::string &value)
{
    m_params[key] = value;
}
void HttpRequest::setCookie(const std::string &key, const std::string &value)
{   
    m_cookies[key] = value;
}
void HttpRequest::delHead(const std::string &key)
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
        m_headers.erase(it);
}
void HttpRequest::delParam(const std::string &key)
{
    auto it = m_params.find(key);
    if(it != m_params.end())
        m_params.erase(it);
}
void HttpRequest::delCookie(const std::string &key)
{
    auto it = m_cookies.find(key);
    if(it != m_cookies.end())
        m_cookies.erase(it);
}
bool HttpRequest::hasHead(const std::string &key, std::string *val) const
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
    {
        if(val != nullptr)
            *val = it->second;
        return true;
    }
    return false;
}
bool HttpRequest::hasParam(const std::string &key, std::string *val) const
{
    auto it = m_params.find(key);
    if(it != m_params.end())
    {
        if(val != nullptr)
            *val = it->second;
        return true;
    }
    return false;
}
bool HttpRequest::hasCookie(const std::string &key, std::string *val) const
{
    auto it = m_cookies.find(key);
    if(it != m_cookies.end())
    {
        if(val != nullptr)
            *val = it->second;
        return true;
    }
    return false;
}


std::ostream &HttpRequest::dump(std::ostream &os)
{
    os  << HttpMethodToString(m_method) << " "
        << m_path
        << (m_query.empty() ? "" : "?")
        << m_query
        << (m_fragment.empty() ? "" : "#")
        << m_fragment
        << " HTTP/"
        << ((uint32_t)(m_version >> 4))
        << "."
        << ((uint32_t)(m_version & 0x0F))
        << "\r\n";
    
    for(auto &i : m_headers)
    {
        if(strcasecmp(i.first.c_str(), "connection") == 0)
            continue;
        os  << i.first 
            << ":"
            << i.second << "\r\n";
    }
    os << "connection:" << (m_close ? "close" : "keep-alive") << "\r\n";
    if(!m_body.empty())
    {
        os << "connection-length: " << m_body.size() << "\r\n";
    }
    os << "\r\n" << m_body;
    return os;
}

HttpResponse::HttpResponse(uint8_t version, bool close)
    :m_status(HttpStatus::OK)
    ,m_close(close)
    ,m_version(version)
{

}

std::string HttpResponse::getHeader(const std::string &key, const std::string &def) const
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
        return it->second;
    return def;
}


void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
    m_headers[key] = value;
}
void HttpResponse::delHead(const std::string &key)
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
        m_headers.erase(it);
}

bool HttpResponse::hasHead(const std::string &key, std::string *val) const
{
    auto it = m_headers.find(key);
    if(it != m_headers.end())
    {
        if(val != nullptr)
            *val = it->second;
        return true;
    }
    return false;
}

std::ostream &HttpResponse::dump(std::ostream &os)
{
    os  << "HTTP/"
         << ((uint32_t)(m_version >> 4))
        << "."
        << ((uint32_t)(m_version & 0x0F))
        << " "
        << (uint32_t)m_status
        <<  " "
        << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
        << "\r\n";
    
    for(auto &i : m_headers)
    {
        if(strcasecmp(i.first.c_str(), "connection") == 0 || strcasecmp(i.first.c_str(), "connection-length") == 0)
            continue;
        os  << i.first 
            << ":"
            << i.second << "\r\n";
    }
    os  << "connection:" << (m_close ? "close" : "keep-alive") << "\r\n"
        << "connection-length: " << m_body.size() << "\r\n"
        << "\r\n" << m_body;
    return os;
}

}
}