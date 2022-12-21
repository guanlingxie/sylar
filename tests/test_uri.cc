#include "sylar/uri.h"
#include <iostream>


int main()
{
    sylar::Uri::ptr uri = sylar::Uri::Create("http://admin@www.baidu.com:8080/test/chinese/uri?id=13&name=sylar&vv=chinese#fragment");
    std::cout << uri->toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << addr->toString() << std::endl;
    return 0;
}