#ifndef __SYLAR_HTTP_SERVLET_H__
#define __SYLAR_HTTP_SERVLET_H__

#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

#include "http.h"
#include "http_session.h"
#include "sylar/thread.h"
#include "sylar/util.h"

namespace sylar
{
namespace http
{
class Servlet
{
public:
    typedef std::shared_ptr<Servlet> ptr;
    Servlet(const std::string &name)
        :m_name(name){}
    virtual ~Servlet(){}
    virtual int32_t handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) = 0;
    const std::string &getName() const {return m_name; }
protected:
    std::string m_name;

};

class FunctionServlet : public Servlet
{
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<uint32_t(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session)> callback;

    FunctionServlet(callback cb);
    virtual int32_t handle(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session) override;
private:
    callback m_cb;
};

class ServletDispatch
{
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef RWMutex RWMutexType;
    ServletDispatch();
     /**
     * @brief 添加servlet
     * @param[in] uri uri
     * @param[in] slt serlvet
     */
    void addServlet(const std::string& uri, Servlet::ptr slt);

    /**
     * @brief 添加servlet
     * @param[in] uri uri
     * @param[in] cb FunctionServlet回调函数
     */
    void addServlet(const std::string& uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);
     /**
     * @brief 删除servlet
     * @param[in] uri uri
     */
    void delServlet(const std::string& uri);

    /**
     * @brief 删除模糊匹配servlet
     * @param[in] uri uri
     */
    void delGlobServlet(const std::string& uri);

    /**
     * @brief 返回默认servlet
     */
    Servlet::ptr getDefault() const { return m_default;}

    /**
     * @brief 设置默认servlet
     * @param[in] v servlet
     */
    void setDefault(Servlet::ptr v) { m_default = v;}


    /**
     * @brief 通过uri获取servlet
     * @param[in] uri uri
     * @return 返回对应的servlet
     */
    Servlet::ptr getServlet(const std::string& uri);

    /**
     * @brief 通过uri获取模糊匹配servlet
     * @param[in] uri uri
     * @return 返回对应的servlet
     */
    Servlet::ptr getGlobServlet(const std::string& uri);

    /**
     * @brief 通过uri获取servlet
     * @param[in] uri uri
     * @return 优先精准匹配,其次模糊匹配,最后返回默认
     */
    Servlet::ptr getMatchedServlet(const std::string& uri);
    void dispatch(HttpRequest::ptr req, HttpResponse::ptr rsp, HttpSession::ptr session);
private:
    RWMutexType m_mutex;
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    Servlet::ptr m_default;
};

class NotFoundServlet : public Servlet {
public:
    /// 智能指针类型定义
    typedef std::shared_ptr<NotFoundServlet> ptr;
    /**
     * @brief 构造函数
     */
    NotFoundServlet(const std::string& name);
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   , sylar::http::HttpResponse::ptr response
                   , sylar::http::HttpSession::ptr session) override;

private:
    std::string m_name;
    std::string m_content;
};

}
}

#endif