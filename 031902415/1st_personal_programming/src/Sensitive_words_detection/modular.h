#include <iostream>
using namespace std;


// ���ļ�
void open_file(char* argv[], ifstream& file_in_words, ifstream& file_in_org,
	ofstream& file_out_ans);
// �ر��ļ�
void close_file(ifstream& file_in_words, ifstream& file_in_org, ofstream& file_out_ans);

// UTF8תGBK
string UTF8ToGBK(const char* strUTF8);

// GBKתUTF8
string GBKToUTF8(const char* src_str);

// intתstring
string int_to_char_to_string(int a);

// ����ת��ƴ�� ����
int ChineseConvertPy(string& dest_chinese, string& out_py);