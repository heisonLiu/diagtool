#ifndef DOIPCONST_H
#define DOIPCONST_H
#include <iostream>

namespace Doip
{
    enum class PayloadType:uint16_t
    {
        ROUTING_ACT_REQ_TYPE = 0x0005,
        ROUTING_ACT_RES_TYPE = 0x0006,
        DIAG_MSG_POS_ACK_TYPE = 0x8002,
        DIAG_MSG_NEG_ACK_TYPE = 0x8003,
        DIAG_MSG_TYPE = 0x8001    
    };

    constexpr uint32_t ACK_TIMEOUT_MS = 220; 
};
#endif