#include <bits/stdc++.h>
#include <unordered_map>
#include "windows.h"
using namespace std;

/*--------------------------------------------------*/
class word
{
public:
	bool isEnd;		// 是否为敏感词最后一个字
	int num;				// 为第几个敏感词
	unordered_map<string, word> next;

	word(bool isEnd=false, int num=-1)
	{
		this->isEnd = isEnd;
		this->num = num;
	}
};

ifstream file_in_words;
ifstream file_in_org;
ofstream file_out_ans;

vector<string> words_string;			// 敏感词表（用于提取）
unordered_map<string, word> words_hash;	// 敏感词哈希表（用于查找）

/*--------------------------------------------------*/

void open_file(char* argv[])	// 打开文件
{
	// 打开文件
	// *argv[1]:敏感词词汇文件
	// *argv[2]:待检测文件
	// *argv[3]:答案文件

/*
	cout << endl << argv[1] << endl;
	cout << endl << argv[2] << endl;
	cout << endl << argv[3] << endl;
*/

/*
	file_in_words.open(argv[1]);
	file_in_org.open(argv[2]);
	file_out_ans.open(argv[3]);		
*/

	// 直接从sln同个目录下读文件（方便debug），在最终阶段需要改成在控制台读
	file_in_words.open("words.txt");
	file_in_org.open("org.txt");
	file_out_ans.open("ans.txt");


	// 文件打开失败，说明并结束程序
	if (!(file_in_words.is_open() && file_in_org.is_open() && file_out_ans.is_open()))
	{
		if (!file_in_words.is_open())	cout << "敏感词词汇文件打开失败" << endl;
		if (!file_in_org.is_open())		cout << "待检测文件打开失败" << endl;
		if (!file_out_ans.is_open())	cout << "答案文件打开失败" << endl;
		exit(0);
	}
}

/*--------------------------------------------------*/

void close_file()	// 关闭文件
{
	// 关闭文件
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "程序运行完成";
}

/*--------------------------------------------------*/

static string UTF8ToGBK(const char* strUTF8)	// UTF8转GBK
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

/*--------------------------------------------------*/

void get_words()	// 获得敏感词列表
{
	int word_num = 0;	// 用来记录敏感词序号
	string s_temp;	// 用来临时读取每一个敏感词
	while (getline(file_in_words,s_temp))	// 读入一个敏感词
	{
		int i_temp=0, l_temp;
		unordered_map<string, word> *word_hash_temp = &words_hash;
		// word_hash_temp是临时的敏感词表节点（指向当前的敏感词的某个位置）
		// words_hash是全部敏感词哈希表

		s_temp = UTF8ToGBK(s_temp.data());		// UTF8转GBK
		l_temp = s_temp.length();				// 获得s_temp长度

		words_string.push_back(s_temp);	// 把敏感词存入敏感词表，word_num是该敏感词的下标
		while (i_temp < l_temp)	// 遍历s_temp，每次处理一个汉字/字母
		{
			string word_string_temp = "";
			// 临时存储一个汉字/字母

			if (s_temp[i_temp] & 0x80)	// 发现一个汉字
			{
				word_string_temp = word_string_temp + s_temp[i_temp] + s_temp[i_temp + 1];
				i_temp += 2;
			}
			else						//发现一个字母
			{
				word_string_temp += s_temp[i_temp];
				i_temp += 1;
			}

			if (!(*word_hash_temp).count(word_string_temp))	// 如果当前汉字/字母在表头不存在										// 如果当前汉字/字母在表头
			{
				(*word_hash_temp)[word_string_temp] = *(new word(false, word_num));
				// 新建该汉字/字母
			}

			if (i_temp >= l_temp)	// 如果已经是敏感词表最后一个汉字/字母
			{
				(*word_hash_temp)[word_string_temp].isEnd = true;
			}
			else					// 否则，还有要添加的汉字/字母
			{
				word_hash_temp = &(*word_hash_temp)[word_string_temp].next;
				// 移动到该汉字/字母的位置
			}
		}
		word_num++;	// 敏感词序号加1
	}
}
/*--------------------------------------------------*/
void deal_org()		// 检测敏感词
{

}
/*--------------------------------------------------*/
void write_ans()	// 输出答案
{

}
/*--------------------------------------------------*/
int main(int argc, char *argv[])
{
	open_file(argv);	// 打开文件

	get_words();		// 获得敏感词列表

	deal_org();			// 检测敏感词

	write_ans();		// 输出答案

	close_file();		// 关闭文件

	return 0;
}
