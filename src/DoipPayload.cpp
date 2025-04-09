#include "DoipPayload.h"
DoipPayload::DoipPayload():m_protol_version(0), m_length(0), m_inverse_protol_version(0), m_payload_type(0)
{

}

DoipPayload::~DoipPayload()
{

}

bool DoipPayload::isCompleteData(const std::vector<uint8_t>& data)
{
    if(data.size() >= 8)
    {
        uint32_t messageLength = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        if(data.size() - 8 >= messageLength)
        {
            m_length = messageLength;
            return true;
        }
    }
    return false;
}

void DoipPayload::getPayLoadFromData(const std::vector<u_int8_t>&data)
{
    m_protol_version = data[0];
    m_inverse_protol_version = data[1];
    m_payload_type = (data[2] << 8) | data[3];
    m_length = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    m_payload.assign(data.begin() + 8, data.begin() + 8 + m_length);
}

std::vector<uint8_t> DoipPayload::getDoipDataFromData(std::vector<uint8_t>&data)
{
    std::vector<uint8_t> doip_data(data.begin(), data.begin() + 8 + m_length);
    data.assign(data.begin() + 8 + m_length, data.end());
    return std::move(doip_data);
}


