#include "DoipClient.h"
DoipClient::DoipClient(uint16_t ta, uint16_t sa, std::string ip, uint16_t port)
    :m_tcp_client(TcpClient::getInstance()), 
     m_doip_target_addr(ta), 
     m_doip_source_addr(sa), 
     m_server_ip(ip), 
     m_server_port(port)
{

}

DoipClient::~DoipClient()
{
}

bool DoipClient::waitDoipAck()
{
    std::unique_lock<std::mutex> lock(m_deal_mtx);
    return m_doip_ack_cv.wait_for(lock, std::chrono::milliseconds(Doip::ACK_TIMEOUT_MS), [this](){ return m_doip_ack_received;});
}


bool DoipClient::sendData(const std::vector<uint8_t>& data)
{
    uint32_t length = data.size() + 4;
    DoipHead temp_doip_head(length, m_doip_source_addr, m_doip_target_addr);
    std::vector<uint8_t> doip_data = temp_doip_head.buildDoipHeadData();
    doip_data.insert(doip_data.end(), data.begin(), data.end());
    m_tcp_client.sendData(doip_data);
    if(!waitDoipAck())
    {
        std::cout << "wait doip ack timeout" << std::endl;
        return false;
    }
    return true;
}

std::pair<bool, std::vector<uint8_t>> DoipClient::getUdsResponse()
{
    std::unique_lock<std::mutex> lock(m_deal_mtx);
    std::cout << "get uds response" << std::endl;
    while(m_uds_received == false && m_uds_timercnt < m_uds_timeout && m_pending_cnt < 100)
    {
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        lock.lock();
        if(m_uds_timercnt == 0)
        {
            m_pending_cnt +=2;
        }
        m_uds_timercnt += 1;
    }
    std::cout << "out udsreceived" << m_uds_received << "timercnt " << (int)m_uds_timercnt << "m_pending_cnt " << (int)m_pending_cnt << std::endl;
    lock.unlock();
    return {m_uds_received, m_uds_data};
}

void DoipClient::doipMsgDeal(const std::vector<uint8_t>&& data)
{
    DoipPayload temp_payload;
    temp_payload.getPayLoadFromData(data);
    std::cout << "doip Msg Deal " << std::endl;
    if(!(temp_payload.m_protol_version == 0x02 && temp_payload.m_inverse_protol_version == 0xfd))
    {
        std::cout << "doip protol error" << std::endl;
        return;
    }
    if(temp_payload.m_payload_type == static_cast<uint16_t>(Doip::PayloadType::DIAG_MSG_POS_ACK_TYPE))
    {
        std::cout << "doip ack" << std::endl;
        std::unique_lock<std::mutex> lock(m_deal_mtx);
        m_doip_ack_received = true;
        m_doip_ack_cv.notify_one();
        return;
    }
    else if(temp_payload.m_payload_type == static_cast<uint16_t>(Doip::PayloadType::DIAG_MSG_TYPE))
    {
        uint16_t source_addr = temp_payload.m_payload[0] << 8 | temp_payload.m_payload[1];
        uint16_t target_addr = temp_payload.m_payload[2] << 8 | temp_payload.m_payload[3];
        std::cout << "recv from  sa " << std::hex << source_addr << "ta " << std::hex << target_addr << std::endl;
        m_uds_data.assign(temp_payload.m_payload.begin() + 4, temp_payload.m_payload.end());
        uint8_t sid = m_uds_data[0];
        std::unique_lock<std::mutex> lock(m_deal_mtx);
        if(sid == 0x7F && m_uds_data[2] == 0x78)
        {
            m_uds_timeout = 1000; //P6 client*
            m_uds_timercnt = 0;
        }
        else
        {
            m_uds_received = true;
        }
        return;
    }
    else if(temp_payload.m_payload_type == static_cast<uint16_t>(Doip::PayloadType::DIAG_MSG_NEG_ACK_TYPE))
    {
        std::cout << "doip nack" << std::endl;
    }
    else
    {
        std::cout << "unexpected doip payload" << std::endl;
    }
}

void DoipClient::startTcpClient()
{
    m_tcp_client.clientConnect(m_server_ip, m_server_port);
    m_tcp_client.registerObserver(m_doip_target_addr, [this](std::vector<uint8_t>&& data){
        this->doipMsgDeal(std::move(data));
        });
}

void DoipClient::diagParaReset()
{
    m_doip_ack_received = false;
    m_uds_received = false;
    m_uds_timeout = 20;
    m_uds_timercnt = 0;
    m_uds_data.clear();
}
