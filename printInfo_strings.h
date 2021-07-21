/*
 * printInfo_strings.h
 *
 * Created: 13.07.2021 10:12:59
 *  Author: qfj
 */ 


#ifndef PRINTINFO_STRINGS_H_
#define PRINTINFO_STRINGS_H_




#ifdef LEVELMETER
#define XBEE_AI_MESSAGE "AI Message"
#define XBEE_REASSOCIATE_   "Reassociating.  "
#define XBEE_REASSOCIATE__  "Reassociating . "
#define XBEE_REASSOCIATE___ "Reassociating  ."
#define XBEE_DEVICE_ASSOCIATED "Device Associated"
#define XBEE_PRINT_NULL    ""
#define XBEE_FAILED "failed"
#define XBEE_NETWORK_ERROR      "Network error"
#define XBEE_NETWORK_ERROR_ADDR "Network err.addr"
#define XBEE_SENDING            "Sending..."
#define XBEE_SENDING_ERROR      "Sending error"
#define XBEE_SENDING_OK         "Sending  ok"
#define XBEE_CHECK_NETWORK      "Check network..."
#define XBEE_NO_NETWORK		   "...no network"
#define XBEE_RECONNECTING       "reconnecting..."
#define XBEE_SENDING_MESSAGE    "sending message..."
#define XBEE_PING				"Ping.."
#endif

#ifdef  GASCOUNTER_MODULE
#define XBEE_AI_MESSAGE    "AImess" // API command "AI" => Reads possible errors with the last association request, result 0: everything is o.k.
#define XBEE_REASSOCIATE_   "con.  "
#define XBEE_REASSOCIATE__  "con . "
#define XBEE_REASSOCIATE___ "con  ."
#define XBEE_DEVICE_ASSOCIATED "DevAss"
#define XBEE_PRINT_NULL    ""
#define XBEE_FAILED "failed"
#define XBEE_NETWORK_ERROR      "NW err"
#define XBEE_NETWORK_ERROR_ADDR "AD err"
#define XBEE_SENDING            "SendMs"
#define XBEE_SENDING_ERROR      "Snd er"
#define XBEE_SENDING_OK         "SendOK"
#define XBEE_CHECK_NETWORK      "CkNetw"
#define XBEE_NO_NETWORK		    "NoNetw "
#define XBEE_RECONNECTING       "recon."
#define XBEE_SENDING_MESSAGE    "SendMs"
#define XBEE_PING				"Ping.."
#endif



#endif /* PRINTINFO_STRINGS_H_ */