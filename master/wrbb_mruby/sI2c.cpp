/*
 * I2C通信関連
 *
 * Copyright (c) 2016 Wakayama.rb Ruby Board developers
 *
 * This software is released under the MIT License.
 * https://github.com/wakayamarb/wrbb-v2lib-firm/blob/master/MITL
 *
 */
#include <Arduino.h>
#include <wire.h>

#include <mruby.h>

#include "../wrbb.h"
#include "sKernel.h"

#define WIRE_MAX	5

TwoWire *RbWire[WIRE_MAX];		//0:Wire1, 1:Wire3, 2:Wire2, 3:Wire6 4:Wire7

//**************************************************
// I2C通信を行うピンを初期化します: I2c.begin
//  I2c.begin( num )
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//**************************************************
mrb_value mrb_i2c_begin(mrb_state *mrb, mrb_value self)
{
int num;

	mrb_get_args(mrb, "i", &num);

	if (num >= 0 && num < WIRE_MAX)
	{
		RbWire[num]->begin();
	}

	return mrb_nil_value();			//戻り値は無しですよ。
}

//**************************************************
// アドレスにデータを書き込みます: I2c.write
//  I2c.write( num, deviceID, address, data )
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//	deviceID: デバイスID
//  address: 書き込みアドレス
//  data: データ
//
//  戻り値は以下のとおり
//		0: 成功
//		4: その他のエラー
//**************************************************
mrb_value mrb_i2c_write(mrb_state *mrb, mrb_value self)
{
int num, deviceID, addr, dat;

	mrb_get_args(mrb, "iiii", &num, &deviceID, &addr, &dat);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( 4 );
	}

	RbWire[num]->beginTransmission(deviceID);
	RbWire[num]->write(addr);
	RbWire[num]->write(dat);

    return mrb_fixnum_value(RbWire[num]->endTransmission());
}

//**************************************************
// アドレスからデータを読み込み: I2c.read
//  I2c.read( num, deviceID, addressL[, addressH] )
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//	deviceID: デバイスID
//  addressL: 読み込み下位アドレス
//  addressH: 読み込み上位アドレス
//
//  戻り値は読み込んだ値
//**************************************************
mrb_value mrb_i2c_read(mrb_state *mrb, mrb_value self)
{
int num, deviceID, addrL, addrH;
int dat = 0;
byte datH;

	int n = mrb_get_args(mrb, "iii|i", &num, &deviceID, &addrL, &addrH);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( dat );
	}

	RbWire[num]->beginTransmission(deviceID);
	RbWire[num]->write(addrL);
	RbWire[num]->endTransmission();

	RbWire[num]->requestFrom(deviceID, 1);
	dat = RbWire[num]->read();

	if( n>=4 ){
		RbWire[num]->beginTransmission(deviceID);
		RbWire[num]->write(addrH);
		RbWire[num]->endTransmission();

		RbWire[num]->requestFrom(deviceID, 1);
		datH = RbWire[num]->read();

		dat += (datH<<8);
	}

    return mrb_fixnum_value( dat );
}

//**************************************************
// I2Cデバイスに対して送信を開始するための準備をする: I2c.begin
//	I2c.begin( num, deviceID )
//	この関数は送信バッファを初期化するだけで、実際の動作は行わない。繰り返し呼ぶと、送信バッファが先頭に戻る。
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//	deviceID: デバイスID 0～0x7Fまでの純粋なアドレス
//**************************************************
mrb_value mrb_i2c_beginTransmission(mrb_state *mrb, mrb_value self)
{
int num, deviceID;

	mrb_get_args(mrb, "ii", &num, &deviceID);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_nil_value();			//戻り値は無しですよ。
	}

	RbWire[num]->beginTransmission(deviceID);

	return mrb_nil_value();			//戻り値は無しですよ。
}

//**************************************************
// 送信バッファの末尾に数値を追加する: I2c.lwrite
//	I2c.lwrite( num, data )
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//	data: セットする値
//
// 戻り値は、送信したバイト数(バッファに溜めたバイト数)を返す。
//	送信バッファ(260バイト)に空き容量が無ければ失敗して0を返す
//**************************************************
mrb_value mrb_i2c_lwrite(mrb_state *mrb, mrb_value self)
{
int num, dat;

	mrb_get_args(mrb, "ii", &num, &dat);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_nil_value();			//戻り値は無しですよ。
	}

	return mrb_fixnum_value( RbWire[num]->write(dat) );
}

//**************************************************
// デバイスに対してI2Cの送信シーケンスを発行する: I2c.end
//	I2c.end(num)
//	I2Cの送信はこの関数を実行して初めて実際に行われる。
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//
// 戻り値は以下のとおり
//	0: 成功
//	4: その他のエラー
//**************************************************
mrb_value mrb_i2c_endTransmission(mrb_state *mrb, mrb_value self)
{
int num;

	mrb_get_args(mrb, "i", &num);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( 4 );
	}

	return mrb_fixnum_value( RbWire[num]->endTransmission() );
}

//**************************************************
// デバイスに対して受信シーケンスを発行しデータを読み出す: I2c.request
//	I2c.request( num, address, count )
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//	address: 読み込み開始アドレス
//	count: 読み出す数
//
//  戻り値は、実際に受信したバイト数。
//**************************************************
mrb_value mrb_i2c_requestFrom(mrb_state *mrb, mrb_value self)
{
int num, addr, cnt;

	mrb_get_args(mrb, "iii", &num, &addr, &cnt);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( 0 );
	}

    return mrb_fixnum_value( RbWire[num]->requestFrom(addr, cnt) );
}

//**************************************************
// デバイスに対して受信シーケンスを発行しデータを読み出す: I2c.lread
//	I2c.lread(num)
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//
//  戻り値は読み込んだ値
//**************************************************
mrb_value mrb_i2c_lread(mrb_state *mrb, mrb_value self)
{
int num;

	mrb_get_args(mrb, "i", &num);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( -1 );
	}

	return mrb_fixnum_value( RbWire[num]->read() );
}

//**************************************************
// デバイスに対して受信バッファ内にあるデータ数を調べる: I2c.available
//	I2c.available(num)
//  num: 通信番号(0:SDA-0/SCL-1, 1:SDA-5/SCL-6, 2:SDA-7/SCL-8, 3:SDA-12/SCL-11, 4:SDA-9(26)/SCL-3)
//
//  戻り値はデータ数
//**************************************************
mrb_value mrb_i2c_available(mrb_state *mrb, mrb_value self)
{
int num;

	mrb_get_args(mrb, "i", &num);

	if (num < 0 && num >= WIRE_MAX)
	{
		return mrb_fixnum_value( 0 );
	}

	return mrb_fixnum_value( RbWire[num]->available() );
}

////**************************************************
//// 周波数を変更する: I2c.freq
////  I2c.freq( Hz )
////  Hz: クロックの周波数をHz単位で指定する。
////      有効な値は1～200000程度。基本的にソフトでやっているので400kHzは出ない。
////**************************************************
//mrb_value mrb_i2c_freq(mrb_state *mrb, mrb_value self)
//{
//int fq;
//
//	mrb_get_args(mrb, "i", &fq);
//
//	Wire.setFrequency( fq );
//
//	return mrb_nil_value();			//戻り値は無しですよ。
//}

//**************************************************
// ライブラリを定義します
//**************************************************
void i2c_Init(mrb_state *mrb)
{
	//0:Wire1, 1:Wire3, 2:Wire2, 3:Wire6 4:Wire7
	RbWire[0] = &Wire1;
	RbWire[1] = &Wire3;
	RbWire[2] = &Wire2;
	RbWire[3] = &Wire6;
	RbWire[4] = &Wire7;

	struct RClass *i2cModule = mrb_define_module(mrb, "I2c");

	mrb_define_module_function(mrb, i2cModule, "begin", mrb_i2c_begin, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, i2cModule, "write", mrb_i2c_write, MRB_ARGS_REQ(4));
	mrb_define_module_function(mrb, i2cModule, "read", mrb_i2c_read, MRB_ARGS_REQ(3)|MRB_ARGS_OPT(1));
	
	mrb_define_module_function(mrb, i2cModule, "begin", mrb_i2c_beginTransmission, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, i2cModule, "lwrite", mrb_i2c_lwrite, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, i2cModule, "end", mrb_i2c_endTransmission, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, i2cModule, "request", mrb_i2c_requestFrom, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, i2cModule, "lread", mrb_i2c_lread, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, i2cModule, "available", mrb_i2c_available, MRB_ARGS_REQ(1));
}
