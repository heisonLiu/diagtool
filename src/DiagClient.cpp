#include "DiagClient.h"

DiagClient::DiagClient(uint16_t ta, uint16_t sa, std::string ip, uint16_t port):m_doip_client(ta, sa, ip, port)
{

}

DiagClient::~DiagClient()
{
    
}
bool DiagClient::sendAndCheckAns(const std::vector<uint8_t> &data)
{
    m_doip_client.diagParaReset();
    if(!m_doip_client.sendData(data))
    {
        std::cout << "send data error" << std::endl;
        return false;
    }
    auto pair_result = m_doip_client.getUdsResponse();
    bool result = pair_result.first;
    m_uds_data = pair_result.second;
    return result;
}

void DiagClient::initAction()
{
    m_doip_client.startTcpClient();
}

bool DiagClient::readEcuDid(uint16_t did)
{
    std::vector<uint8_t> request = 
    {
        0x22,
        static_cast<uint8_t>((did & 0xFF00) >> 8),
        static_cast<uint8_t>(did & 0x00FF)
    };
    std::cout << "request" << std::endl;
    for(auto ch:request)
    {
        std::cout << std::hex << static_cast<int>(ch) << std::endl;
    }
    bool result = sendAndCheckAns(request);
    if(result)
    {
        if(m_uds_data[0] == 0x62)
        {
            std::cout << "pos res" << std::endl;
            for(auto data:m_uds_data)
            { 
                std::cout << std::hex << (int)data << std::endl;
            }
        }
        else
        {
            std::cout << "neg res" << std::endl;
        }
    }
    else
    {
        std::cout << "no res" << std::endl;
    }
}
