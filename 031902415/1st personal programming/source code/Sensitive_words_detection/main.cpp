#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[])
{
	// -------------------------���ļ�------------------------- //

//	*argv[1]:���дʴʻ��ļ�
//	*argv[2]:������ļ�
//	*argv[3]:���ļ�
	ifstream file_in_words(argv[1]);
	ifstream file_in_org(argv[2]);
	ofstream file_out_ans(argv[3]);

	if (!file_in_words.is_open())	{ cout << "�ļ���ʧ�ܣ����дʴʻ��ļ�" << endl; goto finish; }
	if (!file_in_org.is_open())		{ cout << "�ļ���ʧ�ܣ�������ļ�" << endl; goto finish; }
	if (!file_out_ans.is_open())	{ cout << "�ļ���ʧ�ܣ����ļ�" << endl; goto finish; }

	// --------------------------------------------------------------------------- //

	string s_temp
	vector<string> words;
	while (!file_in_words.eof())
	{
		file_in_words >> s_temp;
	}


	// -------------------------����������ƺ���------------------------- //

finish:
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "�������";
	return 0;

	// --------------------------------------------------------------------------- //
}
