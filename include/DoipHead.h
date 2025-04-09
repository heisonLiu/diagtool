#ifndef DOIPHEAD_H
#define DOIPHEAD_H
#include <iostream>
#include <vector>
#include <array>
class DoipHead
{
private:
    /* data */
public:
    uint8_t m_version;
    uint8_t m_version_inver;
    uint16_t m_message_type;
    uint32_t m_doip_payload_length;
    uint16_t m_source_address;
    uint16_t m_target_address;
    DoipHead(uint32_t, uint16_t, uint16_t);
    ~DoipHead();
    std::vector<uint8_t> buildDoipHeadData ()const;
};

#endif