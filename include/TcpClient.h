#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <mutex>
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <thread>
#include <functional>
#include <unordered_map>

#include "ThreadPool.h"
#include "DoipHead.h"
#include "DoipPayload.h"
class TcpClient
{
public:

    using MsgCallBack = std::function<void(std::vector<uint8_t>)>;
    static TcpClient& getInstance();
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    bool clientConnect(const std::string &ip_address, int port);
    bool sendData(const std::vector<uint8_t>&doip_data);
    void registerObserver(uint16_t, MsgCallBack);

private:
    bool m_connected;
    int m_tcp_fc;
    std::mutex m_conn_mtx;
    std::mutex m_send_mtx;
    std::mutex m_cb_mtx;
    std::atomic<bool>m_recv_flag;
    std::thread m_recv_thread;
    std::vector<uint8_t> currentPayloadData; 
    std::function<void(const std::vector<uint8_t>&)> m_deal_buffer_cb;
    std::unordered_map<uint16_t, MsgCallBack> m_observers;
    ThreadPool m_thread_pool;

    TcpClient();
    ~TcpClient();
    void startReceive();
    void stopReceive();
    void receiveLoop();
    void rxDiagsMsgs(const std::vector<uint8_t>& data);


};
#endif