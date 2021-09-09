#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[])
{
	// -------------------------打开文件------------------------- //

//	*argv[1]:敏感词词汇文件
//	*argv[2]:待检测文件
//	*argv[3]:答案文件
	ifstream file_in_words(argv[1]);
	ifstream file_in_org(argv[2]);
	ofstream file_out_ans(argv[3]);

	if (!file_in_words.is_open())	{ cout << "文件打开失败：敏感词词汇文件" << endl; goto finish; }
	if (!file_in_org.is_open())		{ cout << "文件打开失败：待检测文件" << endl; goto finish; }
	if (!file_out_ans.is_open())	{ cout << "文件打开失败：答案文件" << endl; goto finish; }

	// --------------------------------------------------------------------------- //

	string s_temp
	vector<string> words;
	while (!file_in_words.eof())
	{
		file_in_words >> s_temp;
	}


	// -------------------------程序结束，善后处理------------------------- //

finish:
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "程序结束";
	return 0;

	// --------------------------------------------------------------------------- //
}
