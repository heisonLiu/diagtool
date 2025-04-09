#include "DoipHead.h"    

DoipHead::DoipHead(uint32_t doip_payload_length, uint16_t source_address, uint16_t target_address):m_version(0x02),
m_version_inver(0xfd), m_message_type(0x8001), m_doip_payload_length(doip_payload_length), m_source_address(source_address), m_target_address(target_address)
{

}

DoipHead::~DoipHead()
{

}

std::pair<uint8_t, uint8_t> splitToBytes(uint16_t value) 
{
    return std::make_pair(
        static_cast<uint8_t>(value >> 8),
        static_cast<uint8_t>(value & 0xFF)
    );
}

std::array<uint8_t, 4> splitToFourBytes(uint16_t value) 
{
    return {
        static_cast<uint8_t>(value >> 24),
        static_cast<uint8_t>(value >> 16),
        static_cast<uint8_t>(value >> 8),
        static_cast<uint8_t>(value & 0xFF)
    };
}
std::vector<uint8_t> DoipHead::buildDoipHeadData ()const
{
    std::vector<uint8_t> data;
    data.reserve(10);
    data.push_back(m_version);
    data.push_back(m_version_inver);

    auto msg_bytes = splitToBytes(m_message_type);
    data.push_back(msg_bytes.first);
    data.push_back(msg_bytes.second);

    auto len_bytes = splitToFourBytes(m_doip_payload_length);
    data.insert(data.end(), len_bytes.begin(), len_bytes.end());

    auto src_bytes = splitToBytes(m_source_address);
    data.push_back(src_bytes.first);
    data.push_back(src_bytes.second);

    auto tar_bytes = splitToBytes(m_target_address);
    data.push_back(tar_bytes.first);
    data.push_back(tar_bytes.second);
    return data;
}
