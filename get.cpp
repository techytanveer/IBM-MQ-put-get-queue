#include <iostream>
#include <string.h>
#include <cmqc.h>

int main() {
    MQHCONN hConn;
    MQHOBJ  hObj;
    MQLONG  compCode, reason;
    char    qMgrName[] = "QM1";
    
    MQCONN(qMgrName, &hConn, &compCode, &reason);

    // Open for INPUT (Reading)
    MQOD objDesc = {MQOD_DEFAULT};
    strncpy(objDesc.ObjectName, "TEST.QUEUE", MQ_Q_NAME_LENGTH);
    MQLONG openOptions = MQOO_INPUT_AS_Q_DEF | MQOO_FAIL_IF_QUIESCING;

    MQOPEN(hConn, &objDesc, openOptions, &hObj, &compCode, &reason);

    if (compCode == MQCC_OK) {
        MQGMO gmo = {MQGMO_DEFAULT}; // Get Message Options
        MQMD  md  = {MQMD_DEFAULT};  // Message Descriptor
        char  buffer[100];           // Buffer for the message
        MQLONG msgLen;               // Actual length received

        // Wait up to 3 seconds for a message if it's not there
        gmo.Options = MQGMO_WAIT;
        gmo.WaitInterval = 3000;

        MQGET(hConn, hObj, &md, &gmo, sizeof(buffer)-1, buffer, &msgLen, &compCode, &reason);

        if (compCode == MQCC_OK) {
            buffer[msgLen] = '\0'; // Null terminate the string
            std::cout << "Message Received: " << buffer << std::endl;
        } else {
            std::cout << "Get failed. Reason: " << reason << std::endl;
        }

        MQCLOSE(hConn, &hObj, MQCO_NONE, &compCode, &reason);
    }
    MQDISC(&hConn, &compCode, &reason);
    return 0;
}
