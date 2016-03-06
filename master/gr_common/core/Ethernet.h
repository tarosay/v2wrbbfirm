#ifndef __T4ETHERNET_H__
#define	__T4ETHERNET_H__

#include "utility/T4_src/type.h"
#include "utility/T4_src/IPAddress.h"
#include "utility/T4_src/r_t4_itcpip.h"
#include "utility/T4_src/r_t4_dhcp_client_rx_if.h"
#include "utility/T4_src/r_dhcp_client.h"
#include "utility/T4_src/r_t4_dns_client_rx_if.h"
#include "utility/T4_src/r_dns_client.h"
#include "utility/T4_src/ether.h"
#include "utility/T4_src/ip.h"
#include "utility/T4_src/tcp.h"
#include "utility/driver/timer.h"
#include "utility/driver/r_ether.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define ARDUINO_TCP_CEP          1
#define ARDUINO_UDP_CEP          1
#define T4_CLOSED               0
#define T4_TCPS_ESTABLISHED     2
#define T4_TCPS_CLOSE_WAIT      4
#define T4_TCPS_CLOSED          0
#define DHCP_NOTHING_HAPPEND    0
#define DHCP_RENEW_SUCCESS      2
#define TCPUDP_WORK                     1780/sizeof(UW)     /*20150520 wed review*/
#define UDP_RCV_DAT_DATAREAD_MAXIMUM    1472
#define UDP_RCV_BUFFER_SIZE             1024
#define TCP_MSS                         1460
#define UDP_TX_PACKET_MAX_SIZE          24                  /*Along with Arduino original code*/

typedef struct _CEP{
    uint32_t    status;
    T_IPV4EP    dst_addr;
    T_IPV4EP    src_addr;
    int32_t     current_rcv_data_len;
    int32_t     total_rcv_len;
    UB          rcv_buf[TCP_MSS];
    UB          snd_buf[TCP_MSS];
    int32_t     _1sec_counter;
    int32_t     _1sec_timer;
    int32_t     pre_1sec_timer;
}CEP;

extern _TCB   *head_tcb;
extern UB _t4_channel_num;
extern T_TCP_CREP tcp_crep[];
extern T_TCP_CCEP tcp_ccep[];
extern T_UDP_CCEP udp_ccep[];
extern const H __tcprepn;
extern const H __tcpcepn;
extern const H __udpcepn;
extern uint8_t dnsaddr1[];
extern uint8_t dnsaddr2[];
extern UW tcpudp_work[TCPUDP_WORK];
extern TCPUDP_ENV tcpudp_env;
extern volatile UH wait_timer;
extern uint8_t     cepid_max;
extern NAME_TABLE  name_table;
extern DNS_MNG     dns_mng;

extern "C"{
    ER tcp_read_stat(ID cepid);
    ER tcp_force_clr(ID cepid);
    void ConfigurePortPins(void);
    void EnablePeripheralModules(void);
    ER http_callback(ID cepid, FN fncd , VP p_parblk);
    ER dns_callback(ID cepid, FN fncd , VP p_parblk);
    ER t4_tcp_callback(ID cepid, FN fncd , VP p_parblk);
}
extern "C" ER USB_dataReceive_callback(ID cepid, FN fncd,VP p_parblk);
extern "C" void queueInit(void);
void setup_terminal_wait();


class EthernetClass;
class EthernetClient;
extern class EthernetClass Ethernet;

class EthernetClass : public Print{
	private:
        uint32_t    dhcpIPAddressLeaseTime_sec;
        uint32_t    dhcpUse;
        bool        tcp_acp_cep_call_flg;
        void dhcpSuccess(DHCP *tmpDhcpPt){
            memcpy(tcpudp_env.ipaddr, tmpDhcpPt->ipaddr, 4);
#ifdef T4_ETHER_DEBUG
            Serial.print("ip = ");
            Serial.println(localIP());
#endif
            memcpy(tcpudp_env.maskaddr, tmpDhcpPt->maskaddr, 4);
#ifdef T4_ETHER_DEBUG
            Serial.print("snm = ");
            Serial.println(subnetMask());
#endif
            memcpy(tcpudp_env.gwaddr, tmpDhcpPt->gwaddr, 4);
#ifdef T4_ETHER_DEBUG
            Serial.print("gw = ");
            Serial.println(gatewayIP());
#endif
            memcpy((char *)dnsaddr1, (char *)tmpDhcpPt->dnsaddr, 4);
#ifdef T4_ETHER_DEBUG
            Serial.print("dns = ");
            Serial.println(dnsServerIP());
#endif
            memcpy(dhcpSvMac.bytes, ((DHCP_PACKET*)tcpudp_work)->ether.source_address, EP_ALEN);
#ifdef T4_ETHER_DEBUG
            Serial.print("dhcpSvmac = ");
            Serial.print(dhcpSvMac.bytes[0],HEX);
            Serial.print(":");
            Serial.print(dhcpSvMac.bytes[1],HEX);
            Serial.print(":");
            Serial.print(dhcpSvMac.bytes[2],HEX);
            Serial.print(":");
            Serial.print(dhcpSvMac.bytes[3],HEX);
            Serial.print(":");
            Serial.print(dhcpSvMac.bytes[4],HEX);
            Serial.print(":");
            Serial.println(dhcpSvMac.bytes[5],HEX);
#endif
            memcpy(dhcpSvIp.bytes, ((DHCP_PACKET*)tcpudp_work)->ipv4.source_ip, IP_ALEN);
#ifdef T4_ETHER_DEBUG
            Serial.print("dhcpSvIP = ");
            Serial.println(dhcpSvIp.dword,HEX);
#endif
            dhcpUse = true;
            Ethernet.dhcpIPuse_sec = 0;                 /*dhcp lease time local countup start*/
            Ethernet.fromSystemGetLastTime = millis();
            dhcpIPAddressLeaseTime_sec = tmpDhcpPt->dhcpIPAddressLeaseTime;     /*ip lease limit from dhcpSv*/
#ifdef T4_ETHER_DEBUG
            Serial.print("dhcpIPAddressLeaseTime_sec = ");
            Serial.println(dhcpIPAddressLeaseTime_sec);
#endif
        }
        void dhcpLeaseTimeCopy(DHCP *);
        int32_t dhcp_release(DHCP *dhcp, DHCP_PACKET *dhcp_packet);
        void startLANController(void){
            ER  ercd;
            ercd = lan_open();
#ifdef T4_ETHER_DEBUG
            Serial.print("lan_open() = ");
            Serial.println(ercd);
#endif
            if (ercd != E_OK){
                while(1);
            }
        }
        void initialize_TCP_IP(void){
            UW          size;
            ER          ercd;

            size = tcpudp_get_ramsize();
#ifdef T4_ETHER_DEBUG
            Serial.print("tcpudp_get_ramsize() = ");
            Serial.println(size);
#endif
            if (size > (sizeof(tcpudp_work))){
                while(1);
            }
            ercd = tcpudp_open(tcpudp_work);
#ifdef T4_ETHER_DEBUG
            Serial.print("tcpudp_open() = ");
            Serial.println(ercd);
#endif
            if (ercd != E_OK){
                while(1);
            }
        }

	public:
        EthernetClass(){
            dhcpIPAddressLeaseTime_sec = 0;
            dhcpUse = false;
            dhcpIPuse_sec = 0;
            fromSystemGetLastTime = 0;
            tcp_acp_cep_call_flg = 0;
		}
        virtual ~EthernetClass(){}
        int  begin(byte* mac);
		void begin(byte* mac, IPAddress local_ip);
		void begin(byte* mac, IPAddress local_ip, IPAddress dns_server);
		void begin(byte* mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
		void begin(byte* mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
		IPAddress localIP(void);
		IPAddress subnetMask(void);
		IPAddress gatewayIP(void);
		IPAddress dnsServerIP(void);
		int maintain(void);
		void maininit(void);
        void mainloop(void){
            R_ETHER_LinkProcess();
            if(dhcpUse){
                uint32_t    sec,ms;
                ms = millis();
                sec = (ms - Ethernet.fromSystemGetLastTime)/1000;
                if(sec){
                    Ethernet.fromSystemGetLastTime = ms;
                    Ethernet.dhcpIPuse_sec += sec;
                }
            }
        }
        size_t write(uint8_t){
            return 1;
        }
        size_t write(const uint8_t *buffer, size_t size){
            return 1;
        }
        uint32_t    dhcpIPuse_sec;
        uint32_t    fromSystemGetLastTime;

	protected:
        union _dhcp_sv_ip{
            uint8_t bytes[IP_ALEN];
            uint32_t dword;
        } dhcpSvIp;

        union _dhcp_sv_mac{
                uint8_t bytes[EP_ALEN+2];
                uint64_t lword;
        } dhcpSvMac;
        void stop(void);
        bool get_tcp_acp_cep_call_flg(){
            return Ethernet.tcp_acp_cep_call_flg;
        }
        void set_tcp_acp_cep_call_flg(){
            Ethernet.tcp_acp_cep_call_flg = true;
        }
        void clr_tcp_acp_cep_call_flg(){
            Ethernet.tcp_acp_cep_call_flg = false;
        }
};

class EthernetServer : public EthernetClass{
	private:
		uint16_t _port;
	
	public:
		EthernetServer(){
		    _port = 0;
		}
        EthernetServer(uint16_t port);
		virtual ~EthernetServer(){}
		size_t write(){return 0;};
        size_t write(uint8_t b);
        size_t write(const uint8_t *buffer, size_t size);
		void begin(void);
		void begin(uint16_t port){
			_port = port;
			begin();
		}
		EthernetClient available(void);
        using Print::write;
        size_t println(const char c[]){     /* get the length of the string * s return. '\ 0' is not included in length. */
            size_t  n = strlen(c);
            char ch[n+3];
            strcpy(ch,c);
            ch[n+0] = '\r';
            ch[n+1] = '\n';
            ch[n+2] = 0;
            print(ch);
            n += 2;
            return n;
        }
        size_t println(void){
            char ch[3];
            ch[0] = '\r';
            ch[1] = '\n';
            ch[2] = 0;
            print(ch);
            return 2;
        }


	protected:
        int16_t t4_set_tcp_crep(ID repid, UH portno){
            if (repid == 0 || repid > __tcprepn){
                return -1;
            }
            tcp_crep[repid-1].myaddr.portno = portno;
            return 0;
        }
        int16_t t4_set_tcp_ccep(ID cepid, UB ch, INT rbufsz){
            if (cepid == 0 || cepid > __tcpcepn){
                return -1;
            }
            tcp_ccep[cepid-1].cepatr = ch;
            tcp_ccep[cepid-1].rbufsz = rbufsz;
            return 0;
        }
};

class EthernetClient : public EthernetServer{
    public:
      EthernetClient(){
      }
      virtual ~EthernetClient(){}
      int read(void);
      int read(uint8_t *buf, size_t size);
      int8_t connected(void);
      int connect(IPAddress ip, uint16_t port);
      int connect(const char *host, uint16_t port);
      int available();
      void flush();
      void stop();
      operator bool(){
          return connected();
      }
      bool operator==(const bool value){
#ifdef T4_ETHER_DEBUG
          Serial.print("t4:EthernetClient:==:");
          Serial.println(bool() == value);
#endif
          return bool() == value;
      }
      bool operator!=(const bool value){
#ifdef T4_ETHER_DEBUG
          Serial.print("t4:EthernetClient:!=:");
          Serial.println(bool() != value);
#endif
          return bool() != value;
      }
      bool operator==(const EthernetClient& rhs){
#ifdef T4_ETHER_DEBUG
          Serial.println("t4:EthernetClient:==:true");
#endif
          return true;
      }
      bool operator!=(const EthernetClient& rhs){
#ifdef T4_ETHER_DEBUG
          Serial.print("t4:EthernetClient:!=:");
          Serial.println(!this->operator==(rhs));
#endif
          return !this->operator==(rhs);
      }
    private:
};

class EthernetUDP : public EthernetClass{
    private:
      uint16_t  _port;
      uint16_t  _offset;
      int       _remaining;

      T_IPV4EP  _sendIPV4EP;
      uint8_t   _sendBuf[UDP_RCV_DAT_DATAREAD_MAXIMUM+1];
      uint8_t   _recvBuf[UDP_RCV_BUFFER_SIZE];

    public:
      EthernetUDP(){
          _remaining = 0;
          _port=0;
          _offset=0;

          _sendIPV4EP.ipaddr = 0;
          _sendIPV4EP.portno = 0;
      }
      virtual ~EthernetUDP(){}
      uint8_t begin(uint16_t);
      void stop();

      int beginPacket(IPAddress ip, uint16_t port);
      int endPacket();
      size_t write(uint8_t);
      size_t write(const uint8_t *buffer, size_t size);

      using Print::write;

      int parsePacket();
      int available();
      int read();
      int read(unsigned char* buffer, size_t len);
      int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };

      IPAddress remoteIP();
      uint16_t remotePort();
};

#endif
