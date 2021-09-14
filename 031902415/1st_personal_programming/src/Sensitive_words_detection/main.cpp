#include <bits/stdc++.h>
#include <unordered_map>
#include "modular.h"
using namespace std;

class word
{
public:
	bool isEnd;		// 是否为敏感词最后一个字
	int num;				// 该敏感词属于的类型
	unordered_map<string, word>* next;

	word(bool isEnd=false, int num=-1)
	{
		this->isEnd = isEnd;
		this->num = num;
		this->next = new unordered_map<string, word>;
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

vector<string> words_string;			// 敏感词表（用于输出）
unordered_map<string, word> words_hash;	// 敏感词哈希表（用于查找）

string sentence;						// sentence用来储存当前读入的一句话
int sentence_num;						// sentence_num用来储存当前读入的话的编号

stack<string> word_stack;								// 存储当前找到的敏感词
vector<ans_word> ans;									// 存储答案
bool is_chinese = false;								// 判断刚刚处理的字是否为中文
unordered_map<string, word>* now_hash = &words_hash;	// now_hash是当下在敏感词哈希表中追踪的敏感词的字

/*--------------------------------------------------*/

// 判断英文符号
bool eng_sign(char c_temp)
{
	// c_temp就是句中随机抽出来的一个字符，没有任何筛选
	if (c_temp < 0) return false;	// 首先排除中文
	if ((c_temp >= 65 && c_temp <= 90) || (c_temp >= 97 && c_temp <= 122)) return false;
	// 然后排除英文字母
	return true;	// 剩下的是英文符号（包括数字）（ASCII单字节符号）
}

// 判断中文符号
bool cn_sign(char c_temp)
{
	// c_temp就是句中随机抽出来的一个字符，没有任何筛选
	if (c_temp > 0) return false;	// 首先排除英文
	return true;
	// 暂时把中文符号也看作汉字
}


/*--------------------------------------------------*/

// 拓展的识别模块：
// 备注：
//	这里的拓展仅对一个字拓展，head_hash表示需要找到当前字的hash位置，tail_hash表示
//	需要指向的hash位置， head_hash -> <在这里增加拓展> -> tail_hash
// 
//	word_string_temp表示单个要拓展的字
//	word_num表示这个字所属的敏感词的序号
//	head_hash表示需要找到当前字的hash位置
//	tail_hash表示需要指向的hash位置

// 完整拼音识别
void put_full_pinyin_into_words_hash(string word_string_temp, int word_num,
	unordered_map<string, word>* head_hash, unordered_map<string, word>* tail_hash, bool isEnd)

{
	string pinyin_string = "";
	if (ChineseConvertPy(word_string_temp, pinyin_string)) cout << "拼音转换失败";

	int i_temp = 0, l_temp = pinyin_string.length();
	unordered_map<string, word>* pinyin_hash = head_hash;
	
	for (i_temp = 0; i_temp < l_temp; i_temp++)
	{
		string s_temp = "";		// s_temp为用string形式存放当前单个字母
		s_temp = s_temp + pinyin_string[i_temp];
		if (!(*pinyin_hash).count(s_temp))		// 未找到该字母
		{
			(*pinyin_hash)[s_temp] = *(new word(false, word_num));
		}
		if (i_temp == l_temp - 1)		// 是最后一个字母，最后一个字母连接到tail_hash
		{
			(*pinyin_hash)[s_temp].next = tail_hash;
			(*pinyin_hash)[s_temp].isEnd = isEnd;
		}
		else							// 不是最后一个字母
		{
			pinyin_hash = (*pinyin_hash)[s_temp].next;	// pinyin_hash 转到新建的字母的位置
		}
	}

	/*-------------------------*/
	// 实行拼音首字母的识别
	
	/*-------------------------*/

	return;
}

// 首字母拼音识别
void put_head_pinyin_into_words_hash(string word_string_temp, int word_num,
	unordered_map<string, word>* head_hash, unordered_map<string, word>* tail_hash, bool isEnd)
{
	
}

/*--------------------------------------------------*/

// 把单个敏感词放入words_hash以供之后查找判断
void put_into_words_hash(string s_temp,int word_num)
{
	int i_temp = 0;
	int l_temp = s_temp.length();				// 获得s_temp长度
	unordered_map<string, word>* word_hash_temp = &words_hash;
	// word_hash_temp是临时的敏感词表节点（指向当前的敏感词的某个位置）
	// words_hash是全部敏感词哈希表

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
			s_temp[i_temp] = (char)tolower(s_temp[i_temp]);	// 对存入敏感词哈希表的字母转小写
			word_string_temp += s_temp[i_temp];
			i_temp += 1;
		}

		if (!(*word_hash_temp).count(word_string_temp))	// 如果当前汉字/字母在表头不存在
		{
			(*word_hash_temp)[word_string_temp] = *(new word(false, word_num));
			// 新建该汉字/字母

			if (word_string_temp[0] & 0x80)		// 如果新建的结点是汉字
			{
				// 围绕这个汉字，分别增加以下几种识别:

				// 完整拼音识别
				put_full_pinyin_into_words_hash(word_string_temp, word_num, word_hash_temp, (*word_hash_temp)[word_string_temp].next, (i_temp >= l_temp));

				// 首字母拼音识别
				put_head_pinyin_into_words_hash(word_string_temp, word_num, word_hash_temp, (*word_hash_temp)[word_string_temp].next, (i_temp >= l_temp));

				// 繁体字识别
				

				// 拆分偏旁识别

				// 另外：同音字识别需要在遇到具体文本才能判断
			}
		}

		if (i_temp >= l_temp)	// 如果已经是敏感词最后一个汉字/字母
		{
			(*word_hash_temp)[word_string_temp].isEnd = true;
		}
		else					// 否则，还有要添加的汉字/字母
		{
			word_hash_temp = (*word_hash_temp)[word_string_temp].next;
			// 移动到该汉字/字母的位置
		}
	}
}

/*--------------------------------------------------*/

// 获得敏感词列表
unordered_map<string, word>* get_words()
{
	int word_num = 0;	// 用来记录敏感词序号
	string s_temp;		// 用来临时读取每一个敏感词
	while (getline(file_in_words,s_temp))	// 读入一个敏感词
	{
		s_temp = UTF8ToGBK(s_temp.data());		// UTF8转GBK
		words_string.push_back(s_temp);	// 把敏感词存入敏感词表以供输出，word_num是该敏感词的下标-----------------------------------记录标准敏感词
		// 要被输出的标准敏感词在这里被存入列表，大小写被记录，因此后面构建哈希表的时候，可以全部转化为小写
		
		put_into_words_hash(s_temp,word_num);	// 输入敏感词和敏感词序号，将其放入words_hash以供查找

		word_num++;	// 敏感词序号加1
	}
	return &words_hash;
}

/*--------------------------------------------------*/

// 从一句话下标i开始检测是否为敏感词
void have_words(int i,int l, unordered_map<string, word>* get_now_hash)	
{
	now_hash = get_now_hash;
	// i表示当前位置，l表示总长度
	while (i < l)		// 从下标i开始检查敏感词
	{
		//	(sentence[i] >= 161 && sentence[i] < 176 ) ||
		//	(sentence[i] >= 248 && sentence[i] <= 255) ||
		//	(sentence[i] >= 0 && sentence[i] <= 47) ||
		//	(sentence[i] >= 58 && sentence[i] <= 64) ||
		//	(sentence[i] >= 91 && sentence[i] <= 96) ||
		//	(sentence[i] >= 123 && sentence[i] <= 127) ||
		//	(sentence[i] >= 48 && sentence[i] <= 57)
		if (eng_sign(sentence[i]))	// 发现一个字符可能是英文符号/中文符号
		{
			// 如果这个符号在首，则毫无意义，不能为敏感词，直接返回
			string c_to_s = "";
			// 中文：被字符分割
			// 英文：被字符、数字分割
			// 注意，暂时判断中文中加入数字也算敏感词
			if (eng_sign(sentence[i]))		// 是英文符号
			{
				c_to_s = c_to_s + sentence[i];
				i += 1;
				is_chinese = false;

				if (word_stack.size() == 0) return;
			}
			else
			{
				c_to_s = c_to_s + sentence[i] + sentence[i + 1];
				i += 2;
				is_chinese = true;

				if (word_stack.size() == 0) return;
			}

			word_stack.push(c_to_s);	// 字符（除字母、数字、换行的若干字符）被直接放入栈
			// 如果前面完全没有敏感词，是不需要把这个符号压进去的
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
				word_stack.push(s_temp);	// 填入敏感词栈，可能是大写也可能是小写--------------------------------------------------存入检测敏感词
				if (!is_chinese) s_temp = to_lower_string;
				// 英文转换成小写，方便后面判断（输出这个敏感词的时候可能需要是大写，但是已经存入栈所以无所谓）
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
					now_hash = (*now_hash)[s_temp].next;	// now_hash移动到下一位
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

// 对文本检测敏感词
void deal_org(unordered_map<string, word>* now_hash)	// 对文本检测
{
	// sentence用来储存当前读入的一句话
	// sentence_num用来储存当前读入的话的编号
	// 利用全局参数传参
	sentence_num = 1;
	while (getline(file_in_org, sentence))	// 读入一句话----------------------------------------------------------------读入一句话
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
			// 遇到的是中文还是英文，避免错位
			if (is_chinese) i += 2;
			else i += 1;
		}
		sentence_num++;
	}
}
/*--------------------------------------------------*/

// 输出答案
void write_ans()
{
	file_out_ans << "Total: " << ans.size() << endl;
	for (std::vector<ans_word>::iterator it = ans.begin(); it != ans.end(); ++it)
	{
		file_out_ans << "Line" << (*it).line_num << ": <" << words_string[(*it).type_num] << "> " << (*it).word_find << endl;
		// 注意：word_find即找到的敏感词在输出的时候要保留分隔符
	}
}

/*--------------------------------------------------*/
int main(int argc, char *argv[])
{
	open_file(argv, file_in_words, file_in_org, file_out_ans);	// 打开文件

	deal_org(get_words());			
	// get_words获得敏感词哈希表
	// deal_org检测敏感词，并输出

	write_ans();

	close_file(file_in_words, file_in_org, file_out_ans);		// 关闭文件

	return 0;
}



/*
目前进展：
	1.实现了 基本敏感词（包括英文大小写） 的识别

	2.实现了 插入字符、数字的敏感词 的识别（暂时无法区分中文符号）

	3.实现了 全拼敏感词 的识别

	4.

	

	敏感词哈希表中存储的都是小写的英文字母，需要输出的标准敏感词被存在
	words_string，检测敏感词被存在wrod_stack中，都保留大写

	读写未统一 UTF-8

	暂时把中文符号也看作汉字


	1.
	C++子程序之间的全局变量值不同(now_hash虽然是全局变量，但是在
	have_words和deal_org中取值不同)：
	是可以相同的，比如全局变量int a;在子程序中也建立变量int a;这在语
	法上是允许的但要搞清楚，局部变量虽然使用同全局变量相同名字，但它是
	另外开设的变量，可以理解成名称是不同的(编译程序会在子程序中仅对应局
	部变量，而不会对应全局变量)

	2.
	同一个project里面的源文件之间的函数一般情况下是可以互相调用的，前提
	是要声明以及不限制它的作用域！


	备份：

*/