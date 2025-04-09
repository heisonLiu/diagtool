#include "TcpClient.h"

TcpClient& TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

TcpClient::TcpClient():m_tcp_fc(-1),m_thread_pool(4)
{
    m_deal_buffer_cb = [this](const std::vector<uint8_t>&data){
        this->rxDiagsMsgs(data);
    };
}

TcpClient::~TcpClient()
{

};

bool TcpClient::clientConnect(const std::string &ip_address, int port) 
{
    std::lock_guard<std::mutex> lock(m_conn_mtx);
    if(!m_connected)
    {
        m_tcp_fc = socket(AF_INET, SOCK_STREAM, 0);
        if(m_tcp_fc < 0)
        {
            perror("socket create fail");
            return false;
        }
        sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, ip_address.c_str(), &serverAddr.sin_addr);
        if (connect(m_tcp_fc, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Connection failed");
            close(m_tcp_fc);
            return -1;
        }
        std::cout << "connect success" << std::endl;
        m_connected = true;
        startReceive();
        return true;
    }

}

bool TcpClient::sendData(const std::vector<uint8_t>&doip_data)
{
    std::lock_guard<std::mutex> lock(m_send_mtx);
    ssize_t sent_len = send(m_tcp_fc, doip_data.data(), doip_data.size(), 0);
    if (sent_len < 0) {
        perror("Send failed");
        return false;
    } else {
        std::cout << "DoIP message sent to ECU " << std::endl;
        for(auto ch:doip_data)
        {
            std::cout << std::hex << static_cast<int>(ch) << std::endl;
        }
        return true;
    }
}

void TcpClient::registerObserver(uint16_t ta, MsgCallBack callback)
{
    std::unique_lock<std::mutex> lock(m_cb_mtx);
    m_observers[ta] = std::move(callback);
}

void TcpClient::startReceive()
{
    std::cout << "start in" << std::endl;
    if(m_recv_flag.load() == false)
    {
        std::cout << "start recv thread" << std::endl;
        m_recv_flag.store(true);
        m_recv_thread = std::thread(&TcpClient::receiveLoop, this);
        m_recv_thread.detach();
    }
}

void TcpClient::stopReceive()
{
    if(m_recv_flag.load())
    {
        m_recv_flag.store(false);
    }
}

void TcpClient::receiveLoop()
{
    std::cout << "receive loop in" << std::endl;
    uint8_t buffer[1024];
    while(m_recv_flag.load() == true)
    {
        std::cout << "before recv" << std::endl;
        ssize_t len = recv(m_tcp_fc, buffer, sizeof(buffer), 0);
        if(len > 0)
        {
            std::cout << "DOIP Received Data: ";
            for (ssize_t i = 0; i < len; ++i)
            {
                std::cout << std::hex << (int)buffer[i] << " ";
            }       
            std::cout << std::endl;
            if(m_deal_buffer_cb)
            {
                m_deal_buffer_cb(std::vector<u_int8_t>(buffer, buffer + len));
            }
            else
            {
                std::cout << "not init function obj" << std::endl;
            }
        }
    }
}

void TcpClient::rxDiagsMsgs(const std::vector<uint8_t>& data)
{
    currentPayloadData.insert(currentPayloadData.end(), data.begin(), data.end());
    DoipPayload temp_payload;
    while(temp_payload.isCompleteData(currentPayloadData))
    {        
        uint16_t address = currentPayloadData[8] << 8 | currentPayloadData[9];
        std::cout << "received data from ecu "<< std::hex << address << std::endl;
        std::vector<uint8_t> para_payload = temp_payload.getDoipDataFromData(currentPayloadData);
        {
            std::unique_lock<std::mutex> lock(m_cb_mtx);
            auto it = m_observers.find(address);
            if(it != m_observers.end())
            {
                m_thread_pool.enqueue(it->second, std::move(para_payload));
            }
            else
            {
                std::cout << "find callback error" << std::endl;
            }
        }

    }
}
