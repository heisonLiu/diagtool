#ifndef DIAGCLIENT_H
#define DIAGCLIENT_H
#include <iostream>
#include <vector>

#include "DoipClient.h"
class DiagClient
{
private:
    /* data */
public:
    DoipClient m_doip_client;
    std::vector<uint8_t> m_uds_data;
    DiagClient(uint16_t, uint16_t, std::string, uint16_t);
    ~DiagClient();
    bool sendAndCheckAns(const std::vector<uint8_t> &data);
    bool readEcuDid(uint16_t did);
    void initAction();
};


#endif