#include <stdio.h>
#include <string.h>
#include <cmqc.h>

int main() {
    MQHCONN hConn;
    MQHOBJ hObj;
    MQLONG compCode, reason;
    MQOD od = {MQOD_DEFAULT};
    MQMD md = {MQMD_DEFAULT};
    MQGMO gmo = {MQGMO_DEFAULT};
    char buffer[4096];
    char qMgrName[] = "QM1";
    MQLONG messLen;

    // 1. Connect to Queue Manager
    MQCONN(qMgrName, &hConn, &compCode, &reason);

    // 2. Open the DLQ for Input
    strncpy(od.ObjectName, "DEV.DEAD.LETTER.QUEUE", MQ_Q_NAME_LENGTH);
    MQOPEN(hConn, &od, MQOO_INPUT_AS_Q_DEF | MQOO_FAIL_IF_QUIESCING, &hObj, &compCode, &reason);

    if (compCode != MQCC_OK) {
        printf("Failed to open DLQ. Reason: %d\n", reason);
        return 1;
    }

    printf("--- Clearing Dead Letter Queue ---\n");

    // 3. Loop through and GET all messages
    gmo.Options = MQGMO_NO_WAIT | MQGMO_SYNCPOINT; 
    
    while (compCode == MQCC_OK) {
        memcpy(md.MsgId, MQMI_NONE, sizeof(md.MsgId));
        memcpy(md.CorrelId, MQCI_NONE, sizeof(md.CorrelId));

        MQGET(hConn, hObj, &md, &gmo, sizeof(buffer)-1, buffer, &messLen, &compCode, &reason);

        if (compCode == MQCC_OK) {
            buffer[messLen] = '\0';
            printf("Recovered Message: %s\n", buffer);
            printf("Original Put Date/Time: %.8s %.8s\n", md.PutDate, md.PutTime);
            
            // Commit immediately to remove it from the DLQ
            MQCMIT(hConn, &compCode, &reason);
        }
    }

    if (reason == MQRC_NO_MSG_AVAILABLE) {
        printf("DLQ is now empty.\n");
    }

    MQCLOSE(hConn, &hObj, MQCO_NONE, &compCode, &reason);
    MQDISC(&hConn, &compCode, &reason);
    return 0;
}
