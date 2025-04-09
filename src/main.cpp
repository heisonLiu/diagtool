#include "DoipClient.h"
#include "Common.h"
#include "DiagClient.h"
int main()
{
    DiagClient bgwclient(0x0005, 0x0E80, BGW_IP, BGW_PORT);
    bgwclient.initAction();
    bgwclient.readEcuDid(0xF118);
}