#ifndef COMMON_H
#define COMMON_H

#define BGW_IP "127.0.0.1"
#define BGW_PORT 13400

enum class EcuId {
    BGW = 0x0005,
    VCU = 0x0008,
    BMS = 0x000C
    // 继续添加其他ECU的ID
};

enum class SourceAdd {
    DIAG_SA = 0x0E80,
    PSWAP_SA = 0x0E84,
    // 继续添加其他ECU的ID
};
#endif