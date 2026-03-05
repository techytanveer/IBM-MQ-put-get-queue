#include <iostream>
#include <string.h>
#include <cmqc.h>
#include <chrono>
#include <iomanip>
#include <sstream>

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

        // Wait up to 15 seconds for a message if it's not there
        //gmo.Options = MQGMO_WAIT;
        gmo.Options = MQGMO_WAIT | MQGMO_SYNCPOINT; // Wait message. use transactions
        gmo.WaitInterval = 15000; // Wait for 15,000 milliseconds (15 seconds)

        MQGET(hConn, hObj, &md, &gmo, sizeof(buffer)-1, buffer, &msgLen, &compCode, &reason);

	if (reason == MQRC_NO_MSG_AVAILABLE) {
	    std::cout << "Timed out after 15 seconds. No message arrived." << std::endl;
        }

        if (compCode == MQCC_OK) {
            buffer[msgLen] = '\0'; // Null terminate the string
	    
	    // 1. Capture the Current Time (Get Time)
            auto now = std::chrono::system_clock::now();
	    std::time_t get_time_t = std::chrono::system_clock::to_time_t(now);
				   
	    // 2. Parse the MQMD PutDate (YYYYMMDD) and PutTime (HHMMSSmm)
            // MQ stores these as character arrays, not null-terminated strings.
            std::string pDate(md.PutDate, 8);
            std::string pTime(md.PutTime, 8);

	    // 2a. Timestamp to display
	    // Format Date to display: YYYYMMDD -> YYYY-MM-DD
            std::string fDate = pDate.substr(0, 4) + "-" + 
                                pDate.substr(4, 2) + "-" + 
                                pDate.substr(6, 2);

            // Format Time to display: HHMMSS -> HH:MM:SS
            std::string fTime = pTime.substr(0, 2) + ":" + 
                                pTime.substr(2, 2) + ":" + 
                                pTime.substr(4, 2);

	    // 2b. Letancy Calucation
	    // Parse MQMD PutDate (YYYYMMDD) and PutTime (HHMMSSmm)
            struct tm put_tm = {0};
            std::string sYear(md.PutDate, 4);
            std::string sMon(md.PutDate + 4, 2);
            std::string sDay(md.PutDate + 6, 2);
            std::string sHour(md.PutTime, 2);
            std::string sMin(md.PutTime + 2, 2);
            std::string sSec(md.PutTime + 4, 2);

	    put_tm.tm_year = std::stoi(sYear) - 1900; // Years since 1900
            put_tm.tm_mon  = std::stoi(sMon) - 1;     // Months 0-11
            put_tm.tm_mday = std::stoi(sDay);
            put_tm.tm_hour = std::stoi(sHour);
            put_tm.tm_min  = std::stoi(sMin);
            put_tm.tm_sec  = std::stoi(sSec);
            put_tm.tm_isdst = 0; // MQ is UTC, so no DST

            // Convert to time_t (Using timegm for UTC)
            std::time_t put_time_t = timegm(&put_tm); 

            // Calculate Difference
            double latency = std::difftime(get_time_t, put_time_t);


	    // 3. COUT

            std::cout << "Message Received: " << buffer << std::endl;
	    std::cout << "MQ Put Timestamp: " << pDate.c_str() << " " << pTime.c_str() << std::endl;
	    std::cout << "MQ Timestamp (UTC): " << fDate.c_str() << " " << fTime.c_str() << std::endl;
	    std::cout << "Latency in Secconds: " << latency << std::endl;

	    // 4. Simple Latency Calculation (Approximate)
            // For high precision, we'd convert MQ time to epoch, 
            // but for now, let's show the "Received At" timestamp.
            auto now_time_t = std::chrono::system_clock::to_time_t(now);
            std::cout << "Local System Time: " << std::put_time(std::localtime(&now_time_t), "%H:%M:%S") << std::endl;

	    // 5. syncpointing section
            MQCMIT(hConn, &compCode, &reason);
            if (compCode != MQCC_OK) {
	       std::cout << "Commit failed with reason: " << reason << std::endl;
            } else {
	       std::cout << "Transaction committed successfully." << std::endl;
            }

        } else {
            std::cout << "Get failed. Reason: " << reason << std::endl;
        }

        MQCLOSE(hConn, &hObj, MQCO_NONE, &compCode, &reason);
    }
    MQDISC(&hConn, &compCode, &reason);
    return 0;
}
