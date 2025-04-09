#ifndef DOIPCLIENT_H
#define DOIPCLIENT_H
#include<string>
#include <utility>  // for std::pair

#include "TcpClient.h"
#include "Common.h"
#include "DoipPayload.h"
#include "DoipHead.h"
#include "DoipConst.h"

#define DOIP_ACK_TIME_OUT 2

class DoipClient
{

private:
    /* data */
TcpClient& m_tcp_client;
uint16_t m_doip_target_addr = static_cast<uint16_t>(EcuId::BGW);
uint16_t m_doip_source_addr = static_cast<uint16_t>(SourceAdd::DIAG_SA);
std::string m_server_ip = BGW_IP;
uint16_t m_server_port = BGW_PORT;
bool m_doip_ack_received = false;
std::mutex m_deal_mtx;
std::condition_variable m_doip_ack_cv;
std::vector<uint8_t> m_uds_data;
uint8_t m_uds_timeout = 20;
uint8_t m_uds_timercnt = 0;
uint8_t m_pending_cnt = 0;
bool m_uds_received = false;




public:

    DoipClient();
    DoipClient(uint16_t, uint16_t, std::string, uint16_t);

    ~DoipClient();

    bool sendData(const std::vector<uint8_t>&);

    bool waitDoipAck();
    std::pair<bool, std::vector<uint8_t>> getUdsResponse();

    void doipMsgDeal(const std::vector<uint8_t>&&);
    
    void startTcpClient();
    void diagParaReset();

};


#endif