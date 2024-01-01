#include "gsm.h"
#include "gsm_task.h"
#include <string.h>

extern uint8_t out_buffer[100];

const char ca_cert[] =
"-----BEGIN CERTIFICATE-----\r\n\
MIIEyDCCA7CgAwIBAgIQDPW9BitWAvR6uFAsI8zwZjANBgkqhkiG9w0BAQsFADBh\r\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\r\n\
MjAeFw0yMTAzMzAwMDAwMDBaFw0zMTAzMjkyMzU5NTlaMFkxCzAJBgNVBAYTAlVT\r\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh\r\n\
bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD\r\n\
ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV\r\n\
cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy\r\n\
FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc\r\n\
3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8\r\n\
osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT\r\n\
zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAYIwggF+MBIGA1Ud\r\n\
EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHSFgMBmx9833s+9KTeqAx2+7c0XMB8G\r\n\
A1UdIwQYMBaAFE4iVCAYlebjbuYP+vq5Eu0GF485MA4GA1UdDwEB/wQEAwIBhjAd\r\n\
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYIKwYBBQUHAQEEajBoMCQG\r\n\
CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG\r\n\
NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH\r\n\
Mi5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t\r\n\
L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA9BgNVHSAENjA0MAsGCWCGSAGG/WwC\r\n\
ATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgGBmeBDAECAzANBgkqhkiG\r\n\
9w0BAQsFAAOCAQEAkPFwyyiXaZd8dP3A+iZ7U6utzWX9upwGnIrXWkOH7U1MVl+t\r\n\
wcW1BSAuWdH/SvWgKtiwla3JLko716f2b4gp/DA/JIS7w7d7kwcsr4drdjPtAFVS\r\n\
slme5LnQ89/nD/7d+MS5EHKBCQRfz5eeLjJ1js+aWNJXMX43AYGyZm0pGrFmCW3R\r\n\
bpD0ufovARTFXFZkAdl9h6g4U5+LXUZtXMYnhIHUfoyMo5tS58aI7Dd8KvvwVVo4\r\n\
chDYABPPTHPbqjc1qCmBaZx2vN4Ye5DUys/vZwP9BFohFrH/6j/f3IL16/RZkiMN\r\n\
JCqVJUzKoZHm1Lesh3Sz8W2jmdv51b2EQJ8HmA==\r\n\
-----END CERTIFICATE-----\r\n\
-----BEGIN CERTIFICATE-----\r\n\
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\r\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\r\n\
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\r\n\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\r\n\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\r\n\
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\r\n\
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\r\n\
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\r\n\
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\r\n\
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\r\n\
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\r\n\
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\r\n\
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\r\n\
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\r\n\
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\r\n\
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\r\n\
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\r\n\
MrY=\r\n\
-----END CERTIFICATE-----\r\n";


int gsm_check_modem(void)
{
	int result;

	result = send_command_check_result("AT\r\n");

	return result;
}

int gsm_check_sim(sim_info_t * sim_info)
{
	int result;
	char out_buffer_temp[100];

	result = send_command_check_result_and_response("AT+CCID\r\n", out_buffer, RESP_TYPE_HEADER);
	gsm_remove_response_header(out_buffer, out_buffer_temp);
	strcpy(sim_info->iccid, out_buffer_temp);

	result = send_command_check_result_and_response("AT+CIMI\r\n", out_buffer, RESP_TYPE_PAYLOAD_DATA);
	strcpy(sim_info->imsi, out_buffer);

	result = send_command_check_result_and_response("AT+GSN\r\n", out_buffer, RESP_TYPE_PAYLOAD_DATA);
	strcpy(sim_info->imei, out_buffer);

	result = send_command_check_result("AT+CPIN?\r\n");

	return result;
}

int gsm_check_network()
{
	// Get Signal Strength
	send_command_check_result_and_response("AT+CSQ\r\n", out_buffer, RESP_TYPE_HEADER);

	// Check if connected to network
	char *ptr;
	char val;

	do
	{
		send_command_check_result_and_response("AT+CREG?\r\n", out_buffer, RESP_TYPE_HEADER);


		ptr = strchr(out_buffer, ',');
		++ptr;
		val = *ptr;
	} while(val != '1');

	// Check if which network
	send_command_check_result_and_response("AT+COPS?\r\n", out_buffer, RESP_TYPE_HEADER);



}


int gsm_connect_gprs()
{
	send_command_check_result_and_response("AT+QIFGCNT=0\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QICSGP=1,\"http.globe.com.ph\"\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QIREGAPP\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QIACT\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_response("AT+QILOCIP\r\n", out_buffer);
}

int gsm_setup_tls()
{
	send_command_check_result_and_response("AT+QMTCFG=\"SSL\",0,1,2\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_response("AT+QSECWRITE=\"RAM:cacert.pem\",3064,100\r\n", out_buffer);
	send_command_check_response(ca_cert, out_buffer);
	send_command_check_result_and_response("AT+QSSLCFG=\"cacert\",2,\"RAM:cacert.pem\"\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QSSLCFG=\"seclevel\",2,1\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QSSLCFG=\"ciphersuite\",2,\"0xFFFF\"\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QSSLCFG=\"ignorertctime\",1\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
}

int gsm_mqtt_setup()
{
	send_command_check_result_and_response("AT+QMTCFG=\"KEEPALIVE\",2,60\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QMTCFG=\"VERSION\",2,1\r\n", out_buffer, RESP_TYPE_RESULT_CODE);
	send_command_check_result_and_response("AT+QMTOPEN=0,\"mqtt3.thingspeak.com\",8883\r\n", out_buffer, RESP_TYPE_HEADER);
	send_command_check_result_and_response("AT+QMTCONN=0,\"JBU4DxUHGSA6DAsUFDwZMjA\",\"JBU4DxUHGSA6DAsUFDwZMjA\",\"UsPR69tE0YJcB+PiCaCbVimx\"\r\n", out_buffer, RESP_TYPE_HEADER);
}
