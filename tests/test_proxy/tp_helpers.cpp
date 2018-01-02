//
//  tp_helpers.cpp
//  test_proxy
#include "tp_helpers.hpp"
bool hasContentLength(MessageBase& msg)
{
    return (msg.hasHeader(HttpHeader::Name::ContentLength));
}
std::size_t getContentLength(MessageBase& msg)
{
    assert(msg.hasHeader(HttpHeader::Name::ContentLength));
    int len = std::stoi(msg.getHeader(HttpHeader::Name::ContentLength));
    return len;
}

void setContentLength(MessageBase& msg, std::size_t length)
{
    msg.setHeader(HttpHeader::Name::ContentLength, std::to_string(length));
}
void setContent(MessageBase& msg, Marvin::BufferChainSPtr content)
{
    msg.setContent(content);
}
