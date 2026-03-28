#include <iostream>
#include <string.h>
#include <cmqc.h>  // Standard MQ Header
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdio.h>

int main() {
    MQHCONN hConn;
    MQHOBJ hObj;
    MQLONG compCode, reason;
    MQOD od = {MQOD_DEFAULT};
    MQMD md = {MQMD_DEFAULT};
    MQPMO pmo = {MQPMO_DEFAULT};
    char qMgrName [] = "QM1";

    // 1. Connect to Queue Manager
    MQCONN(qMgrName, &hConn, &compCode, &reason);
    if (compCode != MQCC_OK) {
	std::cout << "MQCONN failed with reason " << reason << std::endl;
        return 1;
    }

    // 2. Open the TEST.QUEUE
    strncpy(od.ObjectName, "TEST.QUEUE", MQ_Q_NAME_LENGTH);
    MQOPEN(hConn, &od, MQOO_OUTPUT | MQOO_FAIL_IF_QUIESCING, &hObj, &compCode, &reason);

    // 3. Craft the "Poison" Payload
    const char* payload = "ACTION:POISON_DATA | Failed Message";
    pmo.Options = MQPMO_SYNCPOINT;

    // 4. Put the message
    MQPUT(hConn, hObj, &md, &pmo, (MQLONG)strlen(payload), (void*)payload, &compCode, &reason);

    if (compCode == MQCC_OK) {
        MQCMIT(hConn, &compCode, &reason);
	std::cout << "Poison message injected into TEST.QUEUE" << std::endl;
    } else {
	std::cout << "Failed to inject poison. Reason: " << reason << std::endl;
    }

    MQCLOSE(hConn, &hObj, MQCO_NONE, &compCode, &reason);
    MQDISC(&hConn, &compCode, &reason);
    return 0;
}
