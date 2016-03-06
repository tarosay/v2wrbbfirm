/*
 * EEPROM FIle Loader
 *
 * Copyright (c) 2015-2016 Minao Yamamoto
 *
 * This software is released under the MIT License.
 * 
 * https://github.com/tarosay/Wakayama-mruby-board/blob/master/MITL
 */

void lineinput(char *arry);
void writefile(const char *fname, int size, char code, char *readData);
void readfile(const char *fname, char code);
int fileloader(const char* str0, const char* str1);
