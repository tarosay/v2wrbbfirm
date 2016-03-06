/*
 * シリアル通信関連
 *
 * Copyright (c) 2015-2016 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * https://github.com/tarosay/Wakayama-mruby-board/blob/master/MITL
 */
#include <Arduino.h>
#include <HardwareSerial.h>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/array.h>

#include "../wrbb.h"


#define SERIAL_MAX	5

HardwareSerial *rbserial[SERIAL_MAX];		//0:Serial(USB), 1:Serial1, 2:Serial3, 3:Serial2, 4:Serial7
unsigned char BuffRead[SERIAL_BUFFER_SIZE];


//**************************************************
// シリアル通信を初期化します: Serial.begin
//  Serial.begin(num, bps)
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//  bps: ボーレート 
//**************************************************
mrb_value mrb_serial_begin(mrb_state *mrb, mrb_value self)
{
int num, bps;

	mrb_get_args(mrb, "ii", &num, &bps);

	if (num >= 0 && num < SERIAL_MAX)
	{
		rbserial[num]->begin(bps);
	}

	return mrb_nil_value();			//戻り値は無しですよ。
}

//**************************************************
// シリアルに出力します: Serial.print|Serial.println
//**************************************************
void mrb_serial_msprint(int num, mrb_value text)
{

	if(num < 10)
	{
		rbserial[num]->print( RSTRING_PTR(text) );
	}
	else if(num < 20)
	{
		rbserial[num - 10]->println( RSTRING_PTR(text) );
	}
	else{
		rbserial[num - 20]->println();
	}
}

mrb_value msprintMode(mrb_state *mrb, mrb_value self, int mode)
{
int num;
mrb_value text;

	int n = mrb_get_args(mrb, "i|S", &num, &text);

	if (num < 0 || num >= SERIAL_MAX){
		return mrb_nil_value();
	}

	if(mode == 0){
		if(n >= 2){
			mrb_serial_msprint(num,  text);
		}
	}
	else{
		if(n >= 2){
			mrb_serial_msprint(num + 10,  text);
		}
		else{
			mrb_serial_msprint(num + 20,  text);
		}
	}
	return mrb_nil_value();			//戻り値は無しですよ。
}

//**************************************************
// シリアルに出力します: Serial.print
//  Serial.print(num[,str])
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//  str: 文字列
//    省略時は何も出力しません
//**************************************************
mrb_value mrb_serial_print(mrb_state *mrb, mrb_value self)
{
	return msprintMode(mrb, self, 0);
}

//**************************************************
// シリアルに\r\n付きで出力します: Serial.println
//  Serial.println(num[,str])
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//  str: 文字列
//    省略時は改行のみ
//**************************************************
mrb_value mrb_serial_println(mrb_state *mrb, mrb_value self)
{
	return msprintMode(mrb, self, 1);
}

//**************************************************
// シリアルから1バイト取得します: Serial.read
//  Serial.read(num)
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//
// 戻り値
//	受信バッファから読み込んだバイト数, データ配列
//**************************************************
mrb_value mrb_serial_read(mrb_state *mrb, mrb_value self)
{
int num;
int len = 0;
mrb_value arv[2];

	mrb_get_args(mrb, "i", &num);

	if (num >= 0 && num < SERIAL_MAX){
		len = rbserial[num]->available();
		if(len > 0)
		{
			for(int i=0; i<len; i++){
				BuffRead[i] = rbserial[num]->read();
			}
		}
	}

	arv[0] = mrb_fixnum_value(len);
	arv[1] = mrb_str_new_cstr(mrb, (const char*)BuffRead);
	return mrb_ary_new_from_values(mrb, 2, arv);
}

//**************************************************
// シリアルにデータを出力します: Serial.write
//  Serial.write(num, buf, len)
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//	buf: 出力データ
//	len: 出力データサイズ
// 戻り値
//	出力したバイト数
//**************************************************
mrb_value mrb_serial_write(mrb_state *mrb, mrb_value self)
{
int		num;
int		len;
mrb_value value;
char	*str;

	mrb_get_args(mrb, "iSi", &num, &value, &len);

	str = RSTRING_PTR(value);
	
	if (num >= 0 && num < SERIAL_MAX){
		return mrb_fixnum_value( rbserial[num]->write( (const unsigned char *)str, len));
	}
	
	return mrb_fixnum_value( 0 );
}

//**************************************************
// シリアルデータがあるかどうか調べます: Serial.available
//  Serial.available(num)
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//  戻り値 シリアルバッファにあるデータのバイト数。0の場合はデータなし
//**************************************************
mrb_value mrb_serial_available(mrb_state *mrb, mrb_value self)
{
int		num;
	
	mrb_get_args(mrb, "i", &num);

	if (num >= 0 && num < SERIAL_MAX){
		return mrb_fixnum_value(rbserial[num]->available());
	}
	return mrb_fixnum_value( 0 );
}

//**************************************************
// シリアルデータをフラッシュします: Serial.flash
//  Serial.flash(num)
//  num: 通信番号(0:USB, 1:TX0/RX1, 2:TX5/RX6, 3:TX7/RX8, 4:TX9/RX3)
//  戻り値 シリアルバッファにあるデータのバイト数。0の場合はデータなし
//**************************************************
mrb_value mrb_serial_flash(mrb_state *mrb, mrb_value self)
{
int		num;
	
	mrb_get_args(mrb, "i", &num);

	if (num >= 0 && num < SERIAL_MAX){
		rbserial[num]->flush();
	}
	return mrb_nil_value();			//戻り値は無しですよ。
}

////**************************************************
//// シリアルにデータを出力します: Serial.write
////  Serial.write( buf )
////	buf: 出力データ
////**************************************************
///*
//mrb_value mrb_system_write(mrb_state *mrb, mrb_value self)
//{
//int dat;
//
//	mrb_get_args(mrb, "i", &dat);
//
//	int ret = Serial.write( (unsigned char)dat );
//	
//	return mrb_fixnum_value( ret );
//}
//*/

//**************************************************
// ライブラリを定義します
//**************************************************
void serial_Init(mrb_state *mrb)
{
	rbserial[0] = &Serial;
	rbserial[1] = &Serial1;
	rbserial[2] = &Serial3;
	rbserial[3] = &Serial2;
	rbserial[4] = &Serial7;

	struct RClass *serialModule = mrb_define_module(mrb, "Serial");

	mrb_define_module_function(mrb, serialModule, "begin", mrb_serial_begin, MRB_ARGS_REQ(2));
	mrb_define_module_function(mrb, serialModule, "print", mrb_serial_print, MRB_ARGS_REQ(1)|MRB_ARGS_OPT(1));
	mrb_define_module_function(mrb, serialModule, "println", mrb_serial_println, MRB_ARGS_REQ(1)|MRB_ARGS_OPT(1));

	mrb_define_module_function(mrb, serialModule, "read", mrb_serial_read, MRB_ARGS_REQ(1));
	mrb_define_module_function(mrb, serialModule, "write", mrb_serial_write, MRB_ARGS_REQ(3));
	mrb_define_module_function(mrb, serialModule, "flash", mrb_serial_flash, MRB_ARGS_REQ(1));

	mrb_define_module_function(mrb, serialModule, "available", mrb_serial_available, MRB_ARGS_REQ(1));
}
