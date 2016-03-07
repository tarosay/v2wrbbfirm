/*
 * Rubic用の処理
 *
 * Copyright (c) 2015-2016 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * https://github.com/tarosay/Wakayama-mruby-board/blob/master/MITL
 *
 * Light weight Lanuage Board Ruby
 * Wakayamarb board 
 *
 */
#include <Arduino.h>
#include <MsTimer2.h>
#include <specific_instructions.h>

int Ack_FE_mode = 0;		//0xFEを読んだときのモード。0:0xFEを読んだときに0xFEを返さない。普通にバッファに残す 1:0xFEを返す, 2:割り込みを発生させない

//**************************************************
//  Rubicの0xFEに0xFEを返す処理の切り替え
//**************************************************
void rubic_ACK(int mode)
{
	return;

	if(mode == 1){
		Ack_FE_mode = 1;
		MsTimer2::start();
	}
	else{
		Ack_FE_mode = 0;
		MsTimer2::stop();
	}
}

////**************************************************
////  1000Hz 10msec割り込み
////**************************************************
void timer1000hz()
{
	return;

	interrupts();
	if(Ack_FE_mode == 1 && Serial.peek() == -2){
		Serial.read();
		Serial.write(0xFE);
	}
}

//**************************************************
// 割り込みタイマー設定
//**************************************************
void rubic_set()
{
	return;

	//割り込みタイマー設定
	MsTimer2::stop();
	MsTimer2::set(1 ,timer1000hz);

	rubic_ACK(Ack_FE_mode);
}