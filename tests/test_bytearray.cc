#include "sylar/sylar.h"
#include "sylar/bytearray.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

void test_typearray()
{
    sylar::ByteArray byteArray;
    for(int i = 0; i < 10; ++i)
    {
        auto num = random();
        byteArray.writeStringWithoutLength(std::to_string(num) + " ");
        std::cout << num << " ";
        if(i%10 == 9)
            std::cout << std::endl;
    }
    
    // for(int i = 0; i < 10; ++i)
    // {
    //     std::cout << byteArray.readInt64() << " ";
    //     if(i%10 == 9)
    //         std::cout << std::endl;
    // }
    byteArray.setWritePosition(4096);
    byteArray.writeDouble(1.0);
    
    //byteArray.setReadPosition(1000);
    std::cout << "HexString: "<< byteArray.toHexString() << std::endl;
    std::cout << "m_size: "<< byteArray.getSize() << std::endl;
    std::cout << "writePosition: "<< byteArray.getWritePosition() << std::endl;
    std::cout << "readPosition: "<< byteArray.getReadPosition() << std::endl;
    std::cout << "capacity : "<< byteArray.getCapacity() << std::endl;
}

void test_iov()
{
    sylar::ByteArray byteArray(1);
    std::vector<iovec> vec_iov;
    byteArray.getWriteBuffers(vec_iov, 1000);
    std::cout << "vector<iovec> size: " << vec_iov.size() << std::endl;
    std::cout << "m_size: "<< byteArray.getSize() << std::endl;
    std::cout << "writePosition: "<< byteArray.getWritePosition() << std::endl;
    std::cout << "readPosition: "<< byteArray.getReadPosition() << std::endl;
    std::cout << "capacity : "<< byteArray.getCapacity() << std::endl;
}

int main()
{
    test_iov();
}