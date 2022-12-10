#include "bytearray.h"
#include "endian.h"
#include "log.h"
#include <string.h>
#include <fstream>

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
    ,m_position(0)
    ,m_root(new Node(base_size))
    ,m_size(0)
    ,m_cur(m_root)
{

}
ByteArray::~ByteArray()
{
    while(m_root != nullptr)
    {
        m_cur = m_root;
        m_root = m_root->next;
        delete m_cur;
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
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node *tmp = m_root->next;
    while(tmp)
    {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr;
}
void ByteArray::write(const void *buf, size_t size)
{
    if(size == 0)
        return;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while(size > 0)
    {
        if(ncap >= size)
        {
            memcpy(m_cur->ptr + npos, (char *)buf + bpos, size);
            m_position += size;
            if(m_cur->size == (npos + size))
            {
                if(m_cur->next == nullptr)
                {
                    m_cur->next = new Node(m_baseSize);
                }
                m_cur = m_cur->next;
            }
            bpos += size;
            size = 0;
        }else{
            memcpy(m_cur->ptr + npos, (char *)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            npos = 0;
            if(m_cur->next == nullptr)
            {
                m_cur->next = new Node(m_baseSize);
            }
            m_cur = m_cur->next;
            ncap = m_cur->size;
        }
    }
    if(m_position > m_size)
        m_size = m_position;
}

void ByteArray::read(void *buf, size_t size)
{
    if(size > getReadSize())
    {
        throw std::out_of_range("not enough len");
    }
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_baseSize - npos;
    size_t bpos = 0;

    while(size > 0)
    {
        if(ncap >= size)
        {
            memcpy((char *)buf + bpos, m_cur->ptr + npos, size);
            m_position += size;
            if(m_cur->size == (npos + size))
            {
                m_cur = m_cur->next;
            }
            bpos += size;
            size = 0;
        }else{
            memcpy((char *)buf + bpos, m_cur->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            npos = 0;
            m_cur = m_cur->next;
            ncap = m_cur->size;
        }
    }

}

void ByteArray::setPosition(size_t v)
{
    if(v > m_size)
    {
        throw std::out_of_range("set_position out of ranger");
        return;
    }
    m_position = v;
    m_cur = m_root;
    while(v >= m_baseSize)
    {
        v -= m_baseSize;
        m_cur = m_cur->next;
    }
}

bool ByteArray::writeToFile(const std::string &name) const
{
    return false;
}

void ByteArray::readFromFile(const std::string &name)
{

}

std::string toString()
{
    return std::string();
}
std::string toHexString()
{
    return std::string();
}

}