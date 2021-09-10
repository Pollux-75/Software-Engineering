#include <bits/stdc++.h>
#include <unordered_map>
#include "windows.h"
using namespace std;

/*--------------------------------------------------*/
class word
{
public:
	bool isEnd;		// �Ƿ�Ϊ���д����һ����
	int num;				// Ϊ�ڼ������д�
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

vector<string> words_string;			// ���дʱ�������ȡ��
unordered_map<string, word> words_hash;	// ���дʹ�ϣ�����ڲ��ң�

/*--------------------------------------------------*/

void open_file(char* argv[])	// ���ļ�
{
	// ���ļ�
	// *argv[1]:���дʴʻ��ļ�
	// *argv[2]:������ļ�
	// *argv[3]:���ļ�

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

	// ֱ�Ӵ�slnͬ��Ŀ¼�¶��ļ�������debug���������ս׶���Ҫ�ĳ��ڿ���̨��
	file_in_words.open("words.txt");
	file_in_org.open("org.txt");
	file_out_ans.open("ans.txt");


	// �ļ���ʧ�ܣ�˵������������
	if (!(file_in_words.is_open() && file_in_org.is_open() && file_out_ans.is_open()))
	{
		if (!file_in_words.is_open())	cout << "���дʴʻ��ļ���ʧ��" << endl;
		if (!file_in_org.is_open())		cout << "������ļ���ʧ��" << endl;
		if (!file_out_ans.is_open())	cout << "���ļ���ʧ��" << endl;
		exit(0);
	}
}

/*--------------------------------------------------*/

void close_file()	// �ر��ļ�
{
	// �ر��ļ�
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "�����������";
}

/*--------------------------------------------------*/

static string UTF8ToGBK(const char* strUTF8)	// UTF8תGBK
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

void get_words()	// ������д��б�
{
	int word_num = 0;	// ������¼���д����
	string s_temp;	// ������ʱ��ȡÿһ�����д�
	while (getline(file_in_words,s_temp))	// ����һ�����д�
	{
		int i_temp=0, l_temp;
		unordered_map<string, word> *word_hash_temp = &words_hash;
		// word_hash_temp����ʱ�����дʱ�ڵ㣨ָ��ǰ�����дʵ�ĳ��λ�ã�
		// words_hash��ȫ�����дʹ�ϣ��

		s_temp = UTF8ToGBK(s_temp.data());		// UTF8תGBK
		l_temp = s_temp.length();				// ���s_temp����

		words_string.push_back(s_temp);	// �����дʴ������дʱ�word_num�Ǹ����дʵ��±�
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
				word_string_temp += s_temp[i_temp];
				i_temp += 1;
			}

			if (!(*word_hash_temp).count(word_string_temp))	// �����ǰ����/��ĸ�ڱ�ͷ������										// �����ǰ����/��ĸ�ڱ�ͷ
			{
				(*word_hash_temp)[word_string_temp] = *(new word(false, word_num));
				// �½��ú���/��ĸ
			}

			if (i_temp >= l_temp)	// ����Ѿ������дʱ����һ������/��ĸ
			{
				(*word_hash_temp)[word_string_temp].isEnd = true;
			}
			else					// ���򣬻���Ҫ��ӵĺ���/��ĸ
			{
				word_hash_temp = &(*word_hash_temp)[word_string_temp].next;
				// �ƶ����ú���/��ĸ��λ��
			}
		}
		word_num++;	// ���д���ż�1
	}
}
/*--------------------------------------------------*/
void deal_org()		// ������д�
{

}
/*--------------------------------------------------*/
void write_ans()	// �����
{

}
/*--------------------------------------------------*/
int main(int argc, char *argv[])
{
	open_file(argv);	// ���ļ�

	get_words();		// ������д��б�

	deal_org();			// ������д�

	write_ans();		// �����

	close_file();		// �ر��ļ�

	return 0;
}
