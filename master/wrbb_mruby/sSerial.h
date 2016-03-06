/*
 * シリアル通信関連
 *
 * Copyright (c) 2015 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * http://opensource.org/licenses/mit-license.php
 */
#ifndef _SSERIAL_H_
#define _SSERIAL_H_  1

//typedef struct RB_Serial
//{
//	bool enable;
//	CSerial *serial;
//	sci_str *sci;	
//
//} RB_Serial;

//**************************************************
// ライブラリを定義します
//**************************************************
void serial_Init(mrb_state *mrb);

#endif // _SSERIAL_H_
