/*
 * Rubic用の処理
 *
 * Copyright (c) 2015-2016 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * https://github.com/tarosay/Wakayama-mruby-board/blob/master/MITL
 */
#ifndef _RUBIC_H_
#define _RUBIC_H_ 1

//**************************************************
//  Rubicの0xFEに0xFEを返す処理の切り替え
//**************************************************
void rubic_ACK(int mode);

//**************************************************
//  1000Hz 10msec割り込み
//**************************************************
void timer1000hz();

//**************************************************
// 割り込みタイマー設定
//**************************************************
void rubic_set();

#endif // _RUBIC_H_
