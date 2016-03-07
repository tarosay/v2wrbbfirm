/*
 * カーネル関連
 *
 * Copyright (c) 2015-2016 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * https://github.com/tarosay/Wakayama-mruby-board/blob/master/MITL
 */
#ifndef _SKERNEL_H_
#define _SKERNEL_H_  1

//**************************************************
// ライブラリを定義します
//**************************************************
void kernel_Init(mrb_state *mrb);

//**************************************************
// WRBB - SAKURAピン番コンバート
//**************************************************
int wrb2sakura(int pin);

#endif // _SKERNEL_H_
