#include "bytearray.h"
#include "endian.h"
#include "log.h"
#include <string.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");
 
namespace sylar
{

ByteArray::Node::Node(size_t s)
    :size(s)
    ,ptr(new char[s])
    ,next(nullptr)
{

}
ByteArray::Node::Node()
    :size(0)
    ,ptr(nullptr)
    ,next(nullptr)
{

}
ByteArray::Node::~Node()
{
    if(ptr)
        delete[] ptr;
}

bool ByteArray::isLittleEndian() const
{
    return m_endian == SYLAR_LITTLE_ENDIAN;
}
void ByteArray::setIsLittleEndian(bool val)
{
    if(val)
        m_endian = SYLAR_LITTLE_ENDIAN;
    else 
        m_endian = SYLAR_BIG_ENDIAN;
}

ByteArray::ByteArray(size_t base_size)
    :m_baseSize(base_size)
    ,m_capacity(base_size)
    ,m_readPosition(0)
    ,m_writePosition(0)
    ,m_root(new Node(base_size))
    ,m_size(0)
    ,m_readCur(m_root)
    ,m_writeCur(m_root)
    ,m_tail(m_root)
{

}

ByteArray::~ByteArray()
{
    while(m_root != nullptr)
    {
        m_readCur = m_root;
        m_root = m_root->next;
        delete m_readCur;
    }
}
//write
void ByteArray::writeFint8(int8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFuint16(uint16_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFint32(int32_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFuint32(uint32_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFint64(int64_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFuint64(uint64_t value)
{
    if(m_endian != SYLAR_BYTE_ORDER)
        value = byteswap(value);
    write(&value, sizeof(value));
}
void ByteArray::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);

}
void ByteArray::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

static uint32_t EncodeZigzag32(int32_t v)
{
    return (v << 1) ^ (v >> 31);
}

static int32_t DecodeZigzag32(uint32_t v)
{
    return (((uint32_t)v) >> 1) ^ -(v & 1);
}
static uint64_t EncodeZigzag64(int64_t v)
{
    return (v << 1) ^ (v >> 31);
}

static int64_t DecodeZigzag64(uint64_t v)
{
    return (((uint64_t)v) >> 1) ^ -(v & 1);
}

void ByteArray::writeInt32(int32_t value)
{
    writeUint32(EncodeZigzag32(value));
}
void ByteArray::writeUint32(uint32_t value)
{
    uint8_t tmp[5] = {0};
    uint8_t i = 0;
    while(value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}
void ByteArray::writeInt64(int64_t value)
{
    writeUint64(EncodeZigzag64(value));
}
void ByteArray::writeUint64(uint64_t value)
{
    uint8_t tmp[10];
    uint8_t i = 0;
    while(value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}


void ByteArray::writeStringF16(const std::string &value)
{
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string &value)
{
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string &value)
{
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}
void ByteArray::writeStringVint(const std::string &value)
{
    writeUint64(value.size());
    write(value.c_str(), value.size());
}
void ByteArray::writeStringWithoutLength(const std::string &value)
{
    write(value.c_str(), value.size());
}

//read
int8_t   ByteArray::readFint8()
{
    int8_t v;
    read(&v,sizeof(v));
    return v;
}

uint8_t  ByteArray::readFuint8()
{
    uint8_t v;
    read(&v,sizeof(v));
    return v;
}   

int16_t  ByteArray::readFint16()
{
    int16_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}
uint16_t ByteArray::readFuint16()
{
    uint16_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}
int32_t  ByteArray::readFint32()
{
    int32_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}

uint32_t ByteArray::readFuint32()
{
    uint32_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}
int64_t  ByteArray::readFint64()
{
    int64_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}
uint64_t ByteArray::readFuint64()
{
    uint64_t v;
    read(&v,sizeof(v));
    if(m_endian != SYLAR_BYTE_ORDER)
    {
        v = byteswap(v);
    }
    return v;
}

float    ByteArray::readFloat()
{
    float v;
    uint32_t value = readFuint32();
    memcpy(&v, &value, sizeof(v));
    return v;
}
double   ByteArray::readDouble()
{
    double v;
    uint64_t value = readFuint64();
    memcpy(&v, &value, sizeof(v));
    return v;
}

int32_t  ByteArray::readInt32()
{   
    return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32()
{
    uint32_t result = 0;
    for(int i = 0;i < 32;i += 7)
    {
        uint8_t b = readFuint8();
        if(b < 0x80)
        {
            result |= ((uint32_t)b) << i;
            break;
        }else{
            result |= ((u_int32_t)(b & 0x7f)) << i;
        }
    }
    return result;
}
int64_t  ByteArray::readInt64()
{
    return DecodeZigzag64(readUint64());
}
uint64_t ByteArray::readUint64()
{
    uint64_t result = 0;
    for(int i = 0;i < 64;i += 7)
    {
        uint8_t b = readFuint8();
        if(b < 0x80)
        {
            result |= ((uint64_t)b) << i;
            break;
        }else{
            result |= ((u_int64_t)(b & 0x7f)) << i;
        }
        //SYLAR_LOG_INFO(g_logger) << i;
    }

    return result;
}

std::string ByteArray::readStringF16()
{
    uint16_t len = readFuint16();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}
std::string ByteArray::readStringF32()
{
    uint32_t len = readFuint32();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}
std::string ByteArray::readStringF64()
{
    uint64_t len = readFuint64();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}
std::string ByteArray::readStringVint()
{
    uint64_t len = readUint64();
    std::string buf;
    buf.resize(len);
    read(&buf[0], len);
    return buf;
}

//inner function
void ByteArray::clear()
{
    m_readPosition = m_writePosition = 0;
    m_capacity = m_baseSize;
    m_size = 0;
    Node *tmp = m_root->next;
    while(tmp)
    {
        m_readCur = tmp;
        tmp = tmp->next;
        delete m_readCur;
    }
    m_root->next = nullptr;
    m_readCur = m_writeCur = m_tail = m_root;
}

void ByteArray::setReadPosition(size_t v)
{
    if(v > m_size)
    {
        throw std::out_of_range("setReadPosition function: readPosition out of range");
    }
    m_readCur = m_root;
    m_readPosition = v;
    while(v >= m_baseSize)
    {
        v -= m_baseSize;
        m_readCur = m_readCur->next;
    }
    
}
void ByteArray::setWritePosition(size_t v)
{
    if(v >= m_capacity)
    {
        addCapacity(v - m_writePosition);
    }
    m_writeCur = m_root;
    m_writePosition = v;
    while(v >= m_baseSize)
    {
        v -= m_baseSize;
        m_writeCur = m_writeCur->next;
    }
    m_size = std::max(m_writePosition, m_size);
}


void ByteArray::write(const void *buf, size_t size)
{
    if(size == 0)
        return;
    addCapacity(size);
    size_t bpos = m_writePosition % m_baseSize;
    size_t npos = 0;
    m_writePosition += size;
    while(size > 0)
    {
        if(size >= m_baseSize - bpos)
        {
            memcpy(m_writeCur->ptr + bpos, (char *)buf + npos, m_baseSize - bpos);
            m_writeCur = m_writeCur->next;
            npos += m_baseSize - bpos;
            size -= m_baseSize - bpos;
            bpos = 0;
        }else{
            memcpy(m_writeCur->ptr + bpos, (char *)buf + npos, size);
            size = 0;
        }
    }
    m_size = std::max(m_size, m_writePosition);
}

void ByteArray::read(void *buf, size_t size)
{
    if(getReadSize() < size)
        throw std::out_of_range(" read size to larger!!!");
    size_t bpos = m_readPosition % m_baseSize;
    size_t npos = 0;
    m_readPosition += size;
    while(size > 0)
    {
        if(size >= m_baseSize - bpos)
        {
            memcpy((char *)buf + npos, m_readCur->ptr + bpos, m_baseSize - bpos);
            m_readCur = m_readCur->next;
            npos += m_baseSize - bpos;
            size -= m_baseSize - bpos;
            bpos = 0;
        }else{
            memcpy((char *)buf + npos, m_readCur->ptr + bpos, size);
            size = 0;
        }
    }
}


bool ByteArray::writeToFile(const std::string &name) const
{
    return false;
}

void ByteArray::readFromFile(const std::string &name)
{

}

std::string ByteArray::toString()
{
    std::string str;
    str.resize(getReadSize());
    if(str.empty())
        return str;
    read(&str[0], str.size());
    m_readPosition -= str.size();
    return str;
}
std::string ByteArray::toHexString()
{
    std::string str = toString();
    std::stringstream ss;

    for(size_t i = 0; i < str.size(); ++i) {
        if(i > 0 && i % 32 == 0) {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " ";
    }

    return ss.str();
}

void ByteArray::addCapacity(size_t size)
{
    if(m_capacity > m_writePosition + size)
        return;
    size_t capacity = m_writePosition + size;
    while(m_capacity <= capacity)
    {
        m_tail->next = new Node(m_baseSize);
        m_tail = m_tail->next;
        m_capacity += m_baseSize;
    }
}
uint64_t ByteArray::getReadBuffers(std::vector<iovec> &buffers, uint64_t len)
{
    if(len == 0 || len > getReadSize())
        return 0;
    uint64_t res = len;
    size_t npos = m_readPosition % m_baseSize;
    struct iovec iov;
    Node *cur = m_readCur;
    while(len > 0)
    {
        if(len >= m_baseSize - npos)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = m_baseSize - npos;
            len -= m_baseSize - npos;
            cur = cur->next;
            npos = 0;
        }else{
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        buffers.push_back(iov);
    }
    return res;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec> &buffers, uint64_t len)
{
    if(len == 0)
        return 0;
    addCapacity(len);
    uint64_t res = len;
    size_t npos = m_writePosition % m_baseSize;
    struct iovec iov;
    Node *cur = m_writeCur;
    while(len > 0)
    {
        if(len >= m_baseSize - npos)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = m_baseSize - npos;
            len -= m_baseSize - npos;
            cur = cur->next;
            npos = 0;
        }else{
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        buffers.push_back(iov);
    }
    return res;
}


}