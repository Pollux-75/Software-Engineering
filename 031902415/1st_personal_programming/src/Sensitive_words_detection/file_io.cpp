#include <iostream>
#include <fstream>
using namespace std;

// ���ļ�
void open_file(char* argv[], ifstream& file_in_words, ifstream& file_in_org,
	ofstream& file_out_ans)
{
	// ���ļ�
	// *argv[1]:���дʴʻ��ļ�
	// *argv[2]:������ļ�
	// *argv[3]:���ļ�

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

// �ر��ļ�
void close_file(ifstream& file_in_words, ifstream& file_in_org, 
	ofstream& file_out_ans)
{
	// �ر��ļ�
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "�����������";
}