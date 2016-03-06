/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : tcp.h
* Version      : 1.0
* Description  : Processing for TCP protocol header file
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.04.2014 1.00     First Release
***********************************************************************************************************************/
#if defined(__GNUC__) || defined(GRSAKURA)
#include "t4define.h"
#endif

#define _TCPH_LEN  20 // minimum TCP  header length (in bytes)
#define _UDPH_LEN  8 // UDP header length (in bytes)

#define E_NO_RCV  -10
#define E_RST_RCV  -2
#define E_ERR   -2
#define E_SYN_OK   0
#define E_INI   -1

#define _TCP_DEFAULT_MSS 536

/* ====== TCB ===== */
#define _ITS_NORMAL 0
#define _ITS_SHT 1
#define _ITS_RST 2

#define _TCP_RTO_INIT  (2 * 100)  // 2 sec.
#define _TCP_RTO_INT_MAX (60 * 100)  // 1 min.

#define _TCPS_CLOSED  0x0000
#define _TCPS_LISTEN  0x0001
#define _TCPS_SYN_SENT  0x0002
#define _TCPS_SYN_RECEIVED 0x0004
#define _TCPS_ESTABLISHED 0x0008
#define _TCPS_FIN_WAIT1  0x0010
#define _TCPS_FIN_WAIT2  0x0020
#define _TCPS_CLOSE_WAIT 0x0040
#define _TCPS_LAST_ACK  0x0080
#define _TCPS_CLOSING  0x0100
#define _TCPS_TIME_WAIT  0x0200

#define  _TCPF_URG     0x20
#define  _TCPF_ACK     0x10
#define  _TCPF_PSH     0x08
#define  _TCPF_RST     0x04
#define  _TCPF_SYN     0x02
#define  _TCPF_FIN     0x01

#define _TCBF_NEED_SEND  0x0001
#define _TCBF_PEND_ARP  0x0002
#define _TCBF_PEND_ICMP  0x0004
#define _TCBF_PEND_ZWIN  0x0008
#define _TCBF_PEND_DRV  0x0010
#define _TCBF_NEED_INIT  0x0020
#define _TCBF_FIN_RCVD  0x0040
#define _TCBF_NEED_API  0x0080
#define _TCBF_SND_ICMP  0x0100
#define _TCBF_SND_TCP  0x0200
#define _TCBF_SND_RTX  0x0400
#define _TCBF_SND_ZWIN  0x0800
#define _TCBF_SND_ARP_REQ 0x1000
#define _TCBF_SND_ARP_REP 0x2000
#define _TCBF_RET_LISTEN 0x4000
#define _TCBF_AVOID_DACK 0x8000
#define _TCBF_14  0x0000

#define _TCP_API_ACPCP 1 // tcp_acp_cep()
#define _TCP_API_CONCP 2 // tcp_con_cep()
#define _TCP_API_SHTCP 3 // tcp_sht_cep()
#define _TCP_API_CLSCP 4 // tcp_cls_cep()
#define _TCP_API_SNDDT 5 // tcp_snd_dat()
#define _TCP_API_RCVDT 6 // tcp_rcv_dat()
#define _TCP_API_CANCP 7 // tcp_can_cep()

#define _TCP_API_STAT_INIT   0
#define _TCP_API_STAT_UNTREATED  1
#define _TCP_API_STAT_INCOMPLETE 2
#define _TCP_API_STAT_COMPLETE  3
#define _TCP_API_STAT_TMOUT   4

#define _TCP_API_FLAG_CANCELED  1

typedef struct
{
    uint16  my_port;
    T_IPVxEP *dstaddr;
    uint16  repid;
} _TCP_API_CNR;


typedef struct
{
    sint16   dtsiz;
    uchar  *datap;
} _TCP_API_DR;

typedef struct
{
    uchar   type;
    volatile uchar stat;
    sint16   tmout;
    ER    *error;
    uint16   flag;
    uint16   reserved;
    union
    {
        _TCP_API_CNR  cnr;
        _TCP_API_DR   dr;
        void far *parblk;
    } d;
} _API_REQ;


typedef struct
{
    uint16    sport;
    uint16    dport;
    uint32    seq;
    uint32    ack;
    uchar     len;
    uchar     flg;
    uint16    win_size;
    uint16    cksum;
    uint16    urg_ptr;
    uchar     opt[2];
} _TCP_HDR;

typedef struct
{
    _TCP_HDR  th;
    uchar     data[1];
} _TCPS;



typedef struct
{
    ER  ercd;
} _TCP_API_REQ;

typedef struct
{
    volatile uchar stat;
    uchar dummy;
    _TCP_API_REQ req;
} _TCP_CB;
#define GET_TCP_CALLBACK_INFO_PTR(cepid) \
    (&head_tcb[cepid-1].callback_info)
enum
{
    _TCP_CB_STAT_CALLBACK = 1,
    _TCP_CB_STAT_LOCK  = 8,
};
#define _TCP_CB_STAT_IS_VIA_CALLBACK(stat)  ((stat) & _TCP_CB_STAT_CALLBACK)
#define _TCP_CB_STAT_SET_CALLBACK_FLG(stat)  ((stat)=(stat)|((_TCP_CB_STAT_CALLBACK)))
#define _TCP_CB_STAT_CLEAR_CALLBACK_FLG(stat) ((stat)=(stat)&(~(_TCP_CB_STAT_CALLBACK)))
#define _TCP_CB_STAT_IS_API_LOCKED(stat)  ((stat) & _TCP_CB_STAT_LOCK)
#define _TCP_CB_STAT_SET_API_LOCK_FLG(stat)  ((stat)=(stat)|((_TCP_CB_STAT_LOCK)))
#define _TCP_CB_STAT_CLEAR_API_LOCK_FLG(stat) ((stat)=(stat)&(~(_TCP_CB_STAT_LOCK)))
typedef ER(*_TCP_CALLBACK_FUNC)(ID cepid, FN fncd , VP p_parblk);
#define _TCP_CB_GET_CALLBACK_FUNC_PTR(cepid) (tcp_ccep[(cepid)-1].callback)
#define _TCP_CB_CALL_CALLBACK(cepid, fncd, pTcpTcb)       \
    do {\
        if ( (*(pTcpTcb->req.error) < 0 && pTcpTcb->req.stat != _TCP_API_STAT_COMPLETE) || \
                (*(pTcpTcb->req.error) >=0 && pTcpTcb->req.stat == _TCP_API_STAT_INCOMPLETE) ) \
        {                     \
            pTcpTcb->req.stat = _TCP_API_STAT_COMPLETE;      \
            \
            _tcp_call_callback(cepid, fncd, (VP)pTcpTcb->req.error);  \
        }                 \
    }while(0)
#define _TCP_CB_CALL_CALLBACK_WITH_CLOSE(cepid, fncd, req_error, req_stat) \
    do {\
        if ( (*(req_error) < 0 && req_stat != _TCP_API_STAT_COMPLETE) || \
                (*(req_error) >=0 && req_stat == _TCP_API_STAT_INCOMPLETE) ) \
        {                     \
            req_stat = _TCP_API_STAT_COMPLETE;         \
            \
            _tcp_call_callback(cepid, fncd, (VP)req_error);     \
        }                 \
    }while(0)                \
        \
         
typedef struct
{
    uchar   sadr[IP_ALEN];
    uchar   dadr[IP_ALEN];
    uchar   reserve;
    uchar   prtcl;
    uint16  len;
} _TCP_PHDR;

typedef struct
{
    uchar src_addr[IP_ALEN];
    uchar dst_addr[IP_ALEN];
    uchar reserve;
    uchar proto;
    uint16 len;
} _TCPUDP_PHDR;


typedef struct
{
    uchar    *data;
    uchar     hdr_flg;
    uint16   len;
    uint16    rst_cnt;
    uint16    nxt_rtx_cnt;
    uint16    cur_int;
    uint32    seq;
} _TCP_RTX_Q;

typedef struct
{
    uchar    *data;
    uchar     hdr_flg;
    uint16   len;
    uint32    seq;
} _TCP_RTX_Q2;

typedef struct
{
    uint16          flag;
#if defined(_TCP)
    uint16   cepid;
    uint16   status;
    uint16   nxt_status;
    uchar   it_stat;
    uchar           hdr_flg;
    uint16          mss;
    _API_REQ  req;
    _API_REQ  req_can;
    uint32          suna;
    uint32          snxt;
    uint32          risn;
    uint32          rnxt;
    uchar           rem_ip[IP_ALEN];
    uint16          rem_port;
    uint16          loc_port;
    uint16          rtchk_cnt;
    _TCP_RTX_Q      retrans_q;
    _TCP_RTX_Q2  retrans_q2;
    uchar          *nxtdat;
    uchar          *rwin;
    uchar          *rwin_bnry;
    uchar          *rwin_curr;
    uint16          swsize;
    uint16          rmt_rwsize;
    uint16          sdsize;
    uint16          rdsize;
    uint16          mslcnt;
    uint16   zwin_int;
    uint16          nxt_zero;
    uint16          zwp_noack_cnt;

    _TCP_CB   callback_info;

#endif
} _TCB;


typedef struct
{
    _IP_HDR  iph;    // IP Header
    union
    {
        uchar tcph[_TCPH_LEN+4]; // TCP Header (20:header+4:MSS option)
        uchar icmph[_ICMP_HLEN]; // ICMP Header
#if defined(_UDP)
        uchar udph[_UDPH_LEN]; // UDP Header
#endif
    } thdr;
} _TX_IPH ;

typedef struct
{
#if defined(_ETHER)
    _ETH_HDR eh;     // Ether Header
#elif defined(_PPP)
    uchar  address;   /* 0xff */
    uchar  control;   /* 0x03 */
    uint16  proto;
#endif
    union
    {
        _TX_IPH  tip;   // IP Header
#if defined(_ETHER)
#if !defined(_IPV6)
        _ARP_PKT tarp;
#endif
#endif
    } ihdr;
    uint16  hlen;
} _TX_HDR;

void _proc_api(void);
void _proc_rcv(void);
void _proc_snd(void);

void _tcp_init_tcb(_TCB *_ptcb);
ER  _tcp_api_req(ID cepid);
void _tcp_clr_req(ID cepid);
void _tcp_cancel_api(ER ercd);
void _tcp_stat(void);
ER  _tcp_rcv_rst(void);
ER  _tcp_rcv_syn(void);
void _tcp_rcv_ack(void);
sint16 _tcp_rcv_opt(void);
sint16 _tcp_proc_data(void);
ER  _tcp_rcv_fin(void);
void _tcp_swin_updt(void);
void _tcp_cpy_rwdat(void);
void _tcp_clr_rtq(_TCB *ptcb);
void _tcp_return_listen(void);
void _tcp_snd(void);

void _tcp_api_acpt(void);
void _tcp_api_con(void);
void _tcp_api_sht_cls(void);
void _tcp_api_snddt(void);
void _tcp_api_rcvdt(void);
void _tcp_api_wup(ID);
void _tcp_api_slp(ID cepid);
ER _tcp_check_cepid_arg(ID cepid);
ER _tcp_check_len_arg(INT len);
ER  _tcp_check_tmout_arg(uint16 api_type, TMO tmout, _TCP_CB* pTcpcb);
uint16 _tcp_is_tcb_queue_over(uint16 api_type, _TCB* pTcb,  _TCP_CB* pTcpcb);
uint16 _tcp_call_callback(ID cepid, FN fncd, VP p_parblk);
FN _tcp_api_type_to_fn(uint16 api_type);
ER _tcp_recv_polling(_TCB* pTcb, uchar *buf, uint16 size);
void _tcp_init_callback_info(_TCP_CB* pCallbackInfo);

