#include <iostream>
#include <string.h>
#include <cmqc.h> // Standard MQ Header

int main() {
    MQHCONN hConn;      // Connection handle
    MQHOBJ  hObj;       // Object handle (for the queue)
    MQLONG  compCode;   // Completion code
    MQLONG  reason;     // Reason code
    char    qMgrName[] = "QM1";
    
    // 1. Connect to Queue Manager
    MQCONN(qMgrName, &hConn, &compCode, &reason);
    if (compCode == MQCC_FAILED) {
        std::cerr << "Connect failed with Reason Code: " << reason << std::endl;
        return (int)reason;
    }

    // 2. Open the Queue
    MQOD objDesc = {MQOD_DEFAULT};
//  strncpy(objDesc.ObjectName, "TEST.QUEUE", MQ_OBJ_NAME_LENGTH);
    strncpy(objDesc.ObjectName, "TEST.QUEUE", MQ_Q_NAME_LENGTH);
    MQLONG openOptions = MQOO_OUTPUT | MQOO_FAIL_IF_QUIESCING;

    MQOPEN(hConn, &objDesc, openOptions, &hObj, &compCode, &reason);
    if (compCode == MQCC_OK) {
        // 3. Put a Message
        MQPMO putMsgOpts = {MQPMO_DEFAULT};
        MQMD msgDesc = {MQMD_DEFAULT};
        char messageBody[] = "Hello IBM MQ from Ubuntu 24.04!";
        
        MQPUT(hConn, hObj, &msgDesc, &putMsgOpts, (MQLONG)strlen(messageBody), messageBody, &compCode, &reason);
        
        if (compCode == MQCC_OK) {
            std::cout << "Successfully put message to TEST.QUEUE!" << std::endl;
        }
        
        // 4. Close the Queue
        MQLONG closeOptions = MQCO_NONE;
        MQCLOSE(hConn, &hObj, closeOptions, &compCode, &reason);
    }

    // 5. Disconnect
    MQDISC(&hConn, &compCode, &reason);
    return 0;
}
