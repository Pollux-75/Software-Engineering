#include <bits/stdc++.h>
#include <unordered_map>
#include "windows.h"
using namespace std;

/*--------------------------------------------------*/
class word
{
public:
	bool isEnd;		// 是否为敏感词最后一个字
	int num;				// 该敏感词属于的类型
	unordered_map<string, word> next;

	word(bool isEnd=false, int num=-1)
	{
		this->isEnd = isEnd;
		this->num = num;
	}
};
class ans_word
{
public:
	ans_word(int line_num, int type_num, string word_find)
	{
		this->line_num = line_num;
		this->type_num = type_num;
		this->word_find = word_find;
	}

	int line_num;
	int type_num;
	string word_find;
};

ifstream file_in_words;
ifstream file_in_org;
ofstream file_out_ans;

vector<string> words_string;			// 敏感词表（用于提取）
unordered_map<string, word> words_hash;	// 敏感词哈希表（用于查找）
string sentence;	// sentence用来储存当前读入的一句话
int sentence_num;	// sentence_num用来储存当前读入的话的编号
unordered_map<string, word>* now_hash = &words_hash;	
// now_hash是当下在敏感词哈希表中追踪的敏感词的字
stack<string> word_stack;	// 存储当前找到的敏感词
int total = 0;	// 存储找到的敏感词个数
vector<ans_word> ans;	// 存储答案
bool is_chinese = false;


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

string int_to_char_to_string(int a)
{
	char int_to_char_to_string_c = a;
	string int_to_char_to_string_s = "";
	int_to_char_to_string_s = int_to_char_to_string_s + int_to_char_to_string_c;
	return int_to_char_to_string_s;
}

/*--------------------------------------------------*/

unordered_map<string, word>* get_words()		// 获得敏感词列表
{
	int word_num = 0;	// 用来记录敏感词序号
	string s_temp;		// 用来临时读取每一个敏感词
	while (getline(file_in_words,s_temp))	// 读入一个敏感词
	{
		s_temp = UTF8ToGBK(s_temp.data());		// UTF8转GBK
		int i_temp = 0;
		int l_temp = s_temp.length();				// 获得s_temp长度
		unordered_map<string, word>* word_hash_temp = &words_hash;
		// word_hash_temp是临时的敏感词表节点（指向当前的敏感词的某个位置）
		// words_hash是全部敏感词哈希表

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
	return &words_hash;
}

/*--------------------------------------------------*/

void have_words(int i,int l, unordered_map<string, word>* get_now_hash)	//对一句话检测
{
	now_hash = get_now_hash;
	// i表示当前位置，l表示总长度
	while (i < l)		// 从下标i开始检查敏感词
	{
		// (sentence[i_temp]>=0 && sentence[i_temp]<=47)
		// (sentence[i_temp]>=58 && sentence[i_temp]<=64)
		// (sentence[i_temp]>=91 && sentence[i_temp]<=96)
		// (sentence[i_temp]>=123 && sentence[i_temp]<=127)
		if ((sentence[i] >= 0 && sentence[i] <= 47) ||
			(sentence[i] >= 58 && sentence[i] <= 64) ||
			(sentence[i] >= 91 && sentence[i] <= 96) ||
			(sentence[i] >= 123 && sentence[i] <= 127) ||
			(sentence[i] >= 48 && sentence[i] <= 57))	// 发现一个字符
		{
			string c_to_s = "";
			c_to_s = c_to_s + sentence[i];
			word_stack.push(c_to_s);	// 字符（除字母、数字、换行的若干字符）被直接放入栈
			// 中文：被字符分割
			// 英文：被字符、数字分割
			// 注意，if中最后一句暂时判断中文中加入数字也算敏感词
			i++;
		}
		else	// 否则
		{
			string s_temp = "";		// s_temp用来临时记录一个汉字/字母
			if (sentence[i] & 0x80)
			{
				s_temp = s_temp + sentence[i] + sentence[i + 1];	// 若为汉字
				is_chinese = true;
			}
			else
			{
				s_temp = s_temp + sentence[i];	// 若为字母
				is_chinese = false;
			}

			string to_lower_string = int_to_char_to_string(tolower(s_temp[0]));
			if ((is_chinese && (*now_hash).count(s_temp)) || (!is_chinese && (*now_hash).count(to_lower_string)))
				// 这个汉字/字母在敏感词列表中
				// 对于汉字，直接判断是否在表中
				// 对于英文，转为小写再判断是否在表中
			{
				word_stack.push(s_temp);	// 填入敏感词栈，可能是大写也可能是小写
				if(!is_chinese) s_temp = to_lower_string;	// 英文转换成小写，方便后面判断（输出这个敏感词的时候可能需要是大写，但是已经存入栈所以无所谓）
				if ((*now_hash)[s_temp].isEnd)		// 这是最后一个敏感字，找到一个完整的敏感词
				{
					// word_stack中的内容被输出到ans_string中，并且记录到ans中
					string ans_string = "";
					while (!word_stack.empty())
					{
						ans_string = word_stack.top() + ans_string;
						word_stack.pop();
					}

					ans.push_back(*(new ans_word(sentence_num, (*now_hash)[s_temp].num, ans_string)));
					// 寻找成功，写入答案
					return;
				}
				else				// 不是最后一个敏感字，继续往后找
				{
					bool is_chinese_temp = is_chinese;
					now_hash = &((*now_hash)[s_temp].next);	// now_hash移动到下一位
					if (sentence[i] & 0x80) have_words(i + 2, l, now_hash);	// 因为是汉字，i要进2字节
					else have_words(i + 1, l, now_hash);	// 因为是英文，i进1字节
					is_chinese = is_chinese_temp;	// is_chinese_temp保存当前层是否为中文
					return;
				}
			}
			else
			{
				return;		// 这个汉字不在敏感词列表中，寻找失败，直接结束
			}
		}
	}

	return;
}

/*--------------------------------------------------*/

void deal_org(unordered_map<string, word>* now_hash)	// 对文本检测
{
	// sentence用来储存当前读入的一句话
	// sentence_num用来储存当前读入的话的编号
	// 利用全局参数传参
	sentence_num = 1;
	while (getline(file_in_org, sentence))	// 读入一句话
	{
		// sentence得到当前的一句话，检测其中的敏感词
		sentence = UTF8ToGBK(sentence.data());	// UTF8转GBK
		int i = 0, l = sentence.length();
		
		for (i = 0; i < l;)		// 从每个字检测敏感词
		{
			now_hash = &words_hash;		// 初始化在敏感词哈希表中追踪敏感词的指针
			while (!word_stack.empty()) word_stack.pop();	// 清空存储句中敏感词的栈
			is_chinese = false;
			
			have_words(i, l, now_hash);
			if (is_chinese) i += 2;
			else i += 1;
		}
		sentence_num++;
	}
}
/*--------------------------------------------------*/

void write_ans()	// 输出答案
{
	file_out_ans << "Total: " << total << endl;
	for (std::vector<ans_word>::iterator it = ans.begin(); it != ans.end(); ++it)
	{
		cout << "Line" << (*it).line_num << ": <" << words_string[(*it).type_num] << "> " << (*it).word_find << endl;
		// 注意：word_find即找到的敏感词在输出的时候要保留分隔符
	}
}

/*--------------------------------------------------*/
int main(int argc, char *argv[])
{
	open_file(argv);	// 打开文件

	deal_org(get_words());			
	// get_words获得敏感词哈希表
	// deal_org检测敏感词，并输出

	close_file();		// 关闭文件

	return 0;
}



/*
目前进展：
	构建了基本的敏感词表、敏感词哈希表，不包括谐音，拼音，部首等识别
	暂时认为中文插入数字也算敏感词
	英文大小写


	C++子程序之间的全局变量值不同(now_hash虽然是全局变量，但是在
	have_words和deal_org中取值不同)：
	是可以相同的，比如全局变量int a;在子程序中也建立变量int a;这在语
	法上是允许的但要搞清楚，局部变量虽然使用同全局变量相同名字，但它是
	另外开设的变量，可以理解成名称是不同的(编译程序会在子程序中仅对应局
	部变量，而不会对应全局变量)




*/