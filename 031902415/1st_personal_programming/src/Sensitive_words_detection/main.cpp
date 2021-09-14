#include <bits/stdc++.h>
#include <unordered_map>
#include "modular.h"
using namespace std;

class word
{
public:
	bool isEnd;		// �Ƿ�Ϊ���д����һ����
	int num;				// �����д����ڵ�����
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

vector<string> words_string;			// ���дʱ����������
unordered_map<string, word> words_hash;	// ���дʹ�ϣ�����ڲ��ң�

string sentence;						// sentence�������浱ǰ�����һ�仰
int sentence_num;						// sentence_num�������浱ǰ����Ļ��ı��

stack<string> word_stack;								// �洢��ǰ�ҵ������д�
vector<ans_word> ans;									// �洢��
bool is_chinese = false;								// �жϸոմ�������Ƿ�Ϊ����
unordered_map<string, word>* now_hash = &words_hash;	// now_hash�ǵ��������дʹ�ϣ����׷�ٵ����дʵ���

/*--------------------------------------------------*/

// �ж�Ӣ�ķ���
bool eng_sign(char c_temp)
{
	// c_temp���Ǿ�������������һ���ַ���û���κ�ɸѡ
	if (c_temp < 0) return false;	// �����ų�����
	if ((c_temp >= 65 && c_temp <= 90) || (c_temp >= 97 && c_temp <= 122)) return false;
	// Ȼ���ų�Ӣ����ĸ
	return true;	// ʣ�µ���Ӣ�ķ��ţ��������֣���ASCII���ֽڷ��ţ�
}

// �ж����ķ���
bool cn_sign(char c_temp)
{
	// c_temp���Ǿ�������������һ���ַ���û���κ�ɸѡ
	if (c_temp > 0) return false;	// �����ų�Ӣ��
	return true;
	// ��ʱ�����ķ���Ҳ��������
}


/*--------------------------------------------------*/

// ��չ��ʶ��ģ�飺
// ��ע��
//	�������չ����һ������չ��head_hash��ʾ��Ҫ�ҵ���ǰ�ֵ�hashλ�ã�tail_hash��ʾ
//	��Ҫָ���hashλ�ã� head_hash -> <������������չ> -> tail_hash
// 
//	word_string_temp��ʾ����Ҫ��չ����
//	word_num��ʾ��������������дʵ����
//	head_hash��ʾ��Ҫ�ҵ���ǰ�ֵ�hashλ��
//	tail_hash��ʾ��Ҫָ���hashλ��

// ����ƴ��ʶ��
void put_full_pinyin_into_words_hash(string word_string_temp, int word_num,
	unordered_map<string, word>* head_hash, unordered_map<string, word>* tail_hash, bool isEnd)

{
	string pinyin_string = "";
	if (ChineseConvertPy(word_string_temp, pinyin_string)) cout << "ƴ��ת��ʧ��";

	int i_temp = 0, l_temp = pinyin_string.length();
	unordered_map<string, word>* pinyin_hash = head_hash;
	
	for (i_temp = 0; i_temp < l_temp; i_temp++)
	{
		string s_temp = "";		// s_tempΪ��string��ʽ��ŵ�ǰ������ĸ
		s_temp = s_temp + pinyin_string[i_temp];
		if (!(*pinyin_hash).count(s_temp))		// δ�ҵ�����ĸ
		{
			(*pinyin_hash)[s_temp] = *(new word(false, word_num));
		}
		if (i_temp == l_temp - 1)		// �����һ����ĸ�����һ����ĸ���ӵ�tail_hash
		{
			(*pinyin_hash)[s_temp].next = tail_hash;
			(*pinyin_hash)[s_temp].isEnd = isEnd;
		}
		else							// �������һ����ĸ
		{
			pinyin_hash = (*pinyin_hash)[s_temp].next;	// pinyin_hash ת���½�����ĸ��λ��
		}
	}

	/*-------------------------*/
	// ʵ��ƴ������ĸ��ʶ��
	
	/*-------------------------*/

	return;
}

// ����ĸƴ��ʶ��
void put_head_pinyin_into_words_hash(string word_string_temp, int word_num,
	unordered_map<string, word>* head_hash, unordered_map<string, word>* tail_hash, bool isEnd)
{
	
}

/*--------------------------------------------------*/

// �ѵ������дʷ���words_hash�Թ�֮������ж�
void put_into_words_hash(string s_temp,int word_num)
{
	int i_temp = 0;
	int l_temp = s_temp.length();				// ���s_temp����
	unordered_map<string, word>* word_hash_temp = &words_hash;
	// word_hash_temp����ʱ�����дʱ�ڵ㣨ָ��ǰ�����дʵ�ĳ��λ�ã�
	// words_hash��ȫ�����дʹ�ϣ��

	while (i_temp < l_temp)	// ����s_temp��ÿ�δ���һ������/��ĸ
	{
		string word_string_temp = "";
		// ��ʱ�洢һ������/��ĸ

		if (s_temp[i_temp] & 0x80)	// ����һ������
		{
			word_string_temp = word_string_temp + s_temp[i_temp] + s_temp[i_temp + 1];
			i_temp += 2;
		}
		else						//����һ����ĸ
		{
			s_temp[i_temp] = (char)tolower(s_temp[i_temp]);	// �Դ������дʹ�ϣ�����ĸתСд
			word_string_temp += s_temp[i_temp];
			i_temp += 1;
		}

		if (!(*word_hash_temp).count(word_string_temp))	// �����ǰ����/��ĸ�ڱ�ͷ������
		{
			(*word_hash_temp)[word_string_temp] = *(new word(false, word_num));
			// �½��ú���/��ĸ

			if (word_string_temp[0] & 0x80)		// ����½��Ľ���Ǻ���
			{
				// Χ��������֣��ֱ��������¼���ʶ��:

				// ����ƴ��ʶ��
				put_full_pinyin_into_words_hash(word_string_temp, word_num, word_hash_temp, (*word_hash_temp)[word_string_temp].next, (i_temp >= l_temp));

				// ����ĸƴ��ʶ��
				put_head_pinyin_into_words_hash(word_string_temp, word_num, word_hash_temp, (*word_hash_temp)[word_string_temp].next, (i_temp >= l_temp));

				// ������ʶ��
				

				// ���ƫ��ʶ��

				// ���⣺ͬ����ʶ����Ҫ�����������ı������ж�
			}
		}

		if (i_temp >= l_temp)	// ����Ѿ������д����һ������/��ĸ
		{
			(*word_hash_temp)[word_string_temp].isEnd = true;
		}
		else					// ���򣬻���Ҫ��ӵĺ���/��ĸ
		{
			word_hash_temp = (*word_hash_temp)[word_string_temp].next;
			// �ƶ����ú���/��ĸ��λ��
		}
	}
}

/*--------------------------------------------------*/

// ������д��б�
unordered_map<string, word>* get_words()
{
	int word_num = 0;	// ������¼���д����
	string s_temp;		// ������ʱ��ȡÿһ�����д�
	while (getline(file_in_words,s_temp))	// ����һ�����д�
	{
		s_temp = UTF8ToGBK(s_temp.data());		// UTF8תGBK
		words_string.push_back(s_temp);	// �����дʴ������дʱ��Թ������word_num�Ǹ����дʵ��±�-----------------------------------��¼��׼���д�
		// Ҫ������ı�׼���д������ﱻ�����б���Сд����¼����˺��湹����ϣ���ʱ�򣬿���ȫ��ת��ΪСд
		
		put_into_words_hash(s_temp,word_num);	// �������дʺ����д���ţ��������words_hash�Թ�����

		word_num++;	// ���д���ż�1
	}
	return &words_hash;
}

/*--------------------------------------------------*/

// ��һ�仰�±�i��ʼ����Ƿ�Ϊ���д�
void have_words(int i,int l, unordered_map<string, word>* get_now_hash)	
{
	now_hash = get_now_hash;
	// i��ʾ��ǰλ�ã�l��ʾ�ܳ���
	while (i < l)		// ���±�i��ʼ������д�
	{
		//	(sentence[i] >= 161 && sentence[i] < 176 ) ||
		//	(sentence[i] >= 248 && sentence[i] <= 255) ||
		//	(sentence[i] >= 0 && sentence[i] <= 47) ||
		//	(sentence[i] >= 58 && sentence[i] <= 64) ||
		//	(sentence[i] >= 91 && sentence[i] <= 96) ||
		//	(sentence[i] >= 123 && sentence[i] <= 127) ||
		//	(sentence[i] >= 48 && sentence[i] <= 57)
		if (eng_sign(sentence[i]))	// ����һ���ַ�������Ӣ�ķ���/���ķ���
		{
			// �������������ף���������壬����Ϊ���дʣ�ֱ�ӷ���
			string c_to_s = "";
			// ���ģ����ַ��ָ�
			// Ӣ�ģ����ַ������ַָ�
			// ע�⣬��ʱ�ж������м�������Ҳ�����д�
			if (eng_sign(sentence[i]))		// ��Ӣ�ķ���
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

			word_stack.push(c_to_s);	// �ַ�������ĸ�����֡����е������ַ�����ֱ�ӷ���ջ
			// ���ǰ����ȫû�����дʣ��ǲ���Ҫ���������ѹ��ȥ��
		}
		else	// ����
		{
			string s_temp = "";		// s_temp������ʱ��¼һ������/��ĸ
			if (sentence[i] & 0x80)
			{
				s_temp = s_temp + sentence[i] + sentence[i + 1];	// ��Ϊ����
				is_chinese = true;
			}
			else
			{
				s_temp = s_temp + sentence[i];	// ��Ϊ��ĸ
				is_chinese = false;
			}

			string to_lower_string = int_to_char_to_string(tolower(s_temp[0]));
			if ((is_chinese && (*now_hash).count(s_temp)) || (!is_chinese && (*now_hash).count(to_lower_string)))
				// �������/��ĸ�����д��б���
				// ���ں��֣�ֱ���ж��Ƿ��ڱ���
				// ����Ӣ�ģ�תΪСд���ж��Ƿ��ڱ���
			{
				word_stack.push(s_temp);	// �������д�ջ�������Ǵ�дҲ������Сд--------------------------------------------------���������д�
				if (!is_chinese) s_temp = to_lower_string;
				// Ӣ��ת����Сд����������жϣ����������дʵ�ʱ�������Ҫ�Ǵ�д�������Ѿ�����ջ��������ν��
				if ((*now_hash)[s_temp].isEnd)		// �������һ�������֣��ҵ�һ�����������д�
				{
					// word_stack�е����ݱ������ans_string�У����Ҽ�¼��ans��
					string ans_string = "";
					while (!word_stack.empty())
					{
						ans_string = word_stack.top() + ans_string;
						word_stack.pop();
					}

					ans.push_back(*(new ans_word(sentence_num, (*now_hash)[s_temp].num, ans_string)));
					// Ѱ�ҳɹ���д���
					return;
				}
				else				// �������һ�������֣�����������
				{
					bool is_chinese_temp = is_chinese;
					now_hash = (*now_hash)[s_temp].next;	// now_hash�ƶ�����һλ
					if (sentence[i] & 0x80) have_words(i + 2, l, now_hash);	// ��Ϊ�Ǻ��֣�iҪ��2�ֽ�
					else have_words(i + 1, l, now_hash);	// ��Ϊ��Ӣ�ģ�i��1�ֽ�
					is_chinese = is_chinese_temp;	// is_chinese_temp���浱ǰ���Ƿ�Ϊ����
					return;
				}
			}
			else
			{
				return;		// ������ֲ������д��б��У�Ѱ��ʧ�ܣ�ֱ�ӽ���
			}
		}


		
	}

	return;
}

/*--------------------------------------------------*/

// ���ı�������д�
void deal_org(unordered_map<string, word>* now_hash)	// ���ı����
{
	// sentence�������浱ǰ�����һ�仰
	// sentence_num�������浱ǰ����Ļ��ı��
	// ����ȫ�ֲ�������
	sentence_num = 1;
	while (getline(file_in_org, sentence))	// ����һ�仰----------------------------------------------------------------����һ�仰
	{
		// sentence�õ���ǰ��һ�仰��������е����д�
		sentence = UTF8ToGBK(sentence.data());	// UTF8תGBK
		int i = 0, l = sentence.length();
		
		for (i = 0; i < l;)		// ��ÿ���ּ�����д�
		{
			now_hash = &words_hash;		// ��ʼ�������дʹ�ϣ����׷�����дʵ�ָ��
			while (!word_stack.empty()) word_stack.pop();	// ��մ洢�������дʵ�ջ
			is_chinese = false;
			
			have_words(i, l, now_hash);
			// �����������Ļ���Ӣ�ģ������λ
			if (is_chinese) i += 2;
			else i += 1;
		}
		sentence_num++;
	}
}
/*--------------------------------------------------*/

// �����
void write_ans()
{
	file_out_ans << "Total: " << ans.size() << endl;
	for (std::vector<ans_word>::iterator it = ans.begin(); it != ans.end(); ++it)
	{
		file_out_ans << "Line" << (*it).line_num << ": <" << words_string[(*it).type_num] << "> " << (*it).word_find << endl;
		// ע�⣺word_find���ҵ������д��������ʱ��Ҫ�����ָ���
	}
}

/*--------------------------------------------------*/
int main(int argc, char *argv[])
{
	open_file(argv, file_in_words, file_in_org, file_out_ans);	// ���ļ�

	deal_org(get_words());			
	// get_words������дʹ�ϣ��
	// deal_org������дʣ������

	write_ans();

	close_file(file_in_words, file_in_org, file_out_ans);		// �ر��ļ�

	return 0;
}



/*
Ŀǰ��չ��
	1.ʵ���� �������дʣ�����Ӣ�Ĵ�Сд�� ��ʶ��

	2.ʵ���� �����ַ������ֵ����д� ��ʶ����ʱ�޷��������ķ��ţ�

	3.ʵ���� ȫƴ���д� ��ʶ��

	4.

	

	���дʹ�ϣ���д洢�Ķ���Сд��Ӣ����ĸ����Ҫ����ı�׼���дʱ�����
	words_string��������дʱ�����wrod_stack�У���������д

	��дδͳһ UTF-8

	��ʱ�����ķ���Ҳ��������


	1.
	C++�ӳ���֮���ȫ�ֱ���ֵ��ͬ(now_hash��Ȼ��ȫ�ֱ�����������
	have_words��deal_org��ȡֵ��ͬ)��
	�ǿ�����ͬ�ģ�����ȫ�ֱ���int a;���ӳ�����Ҳ��������int a;������
	����������ĵ�Ҫ��������ֲ�������Ȼʹ��ͬȫ�ֱ�����ͬ���֣�������
	���⿪��ı������������������ǲ�ͬ��(�����������ӳ����н���Ӧ��
	���������������Ӧȫ�ֱ���)

	2.
	ͬһ��project�����Դ�ļ�֮��ĺ���һ��������ǿ��Ի�����õģ�ǰ��
	��Ҫ�����Լ�����������������


	���ݣ�

*/