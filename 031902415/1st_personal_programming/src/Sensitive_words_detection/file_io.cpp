#include <iostream>
#include <fstream>
using namespace std;

// 打开文件
void open_file(char* argv[], ifstream& file_in_words, ifstream& file_in_org,
	ofstream& file_out_ans)
{
	// 打开文件
	// *argv[1]:敏感词词汇文件
	// *argv[2]:待检测文件
	// *argv[3]:答案文件

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

// 关闭文件
void close_file(ifstream& file_in_words, ifstream& file_in_org, 
	ofstream& file_out_ans)
{
	// 关闭文件
	file_in_words.close();
	file_in_org.close();
	file_out_ans.close();
	cout << "程序运行完成";
}