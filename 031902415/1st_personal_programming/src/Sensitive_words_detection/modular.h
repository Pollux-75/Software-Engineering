#include <iostream>
using namespace std;


// 打开文件
void open_file(char* argv[], ifstream& file_in_words, ifstream& file_in_org,
	ofstream& file_out_ans);
// 关闭文件
void close_file(ifstream& file_in_words, ifstream& file_in_org, ofstream& file_out_ans);

// UTF8转GBK
string UTF8ToGBK(const char* strUTF8);

// GBK转UTF8
string GBKToUTF8(const char* src_str);

// int转string
string int_to_char_to_string(int a);

// 中文转换拼音 函数
int ChineseConvertPy(string& dest_chinese, string& out_py);