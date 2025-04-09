#ifndef DOIPPAYLOAD_H
#define DOIPPAYLOAD_H
#include <iostream>
#include <vector>


class DoipPayload
{
private:
    /* data */
public:

    uint8_t m_protol_version;
    uint8_t m_inverse_protol_version;
    uint16_t m_payload_type;
    uint32_t m_length;
    std::vector<uint8_t> m_payload;

    DoipPayload();
    ~DoipPayload();
    bool isCompleteData(const std::vector<u_int8_t>&);
    void getPayLoadFromData(const std::vector<u_int8_t>&);
    std::vector<uint8_t> getDoipDataFromData(std::vector<uint8_t>&);

};


#endif