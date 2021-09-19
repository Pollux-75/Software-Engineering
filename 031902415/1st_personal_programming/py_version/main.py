import sys
from pypinyin import pinyin, lazy_pinyin, Style

global file_in_words  # 读入的words文件
global file_in_org  # 读入的org文件
global file_out_ans  # 输出的ans文件
global words  # 敏感词表的内容
global org  # 要检测的文章的内容
global ans  # 要输出的答案

global dfa_tree  # 建成的敏感词树
global symbol  # 无效符号集合
global word_stack  # 词栈，用来保存检查过程中发现的一个敏感词

symbol = ['!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=',
          '|', '\\', '[', ']', '{', '}', ';', ':', "'", '"', ',', '.', '<', '>',
          '/', '?', '`', '~', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
          '！', '@', '#', '￥', '%', '……', '…', '&', '*', '（', '）', '——', '-', '+',
          '=', '【', '】', '{', '}', '|', '、', '；', '：', '‘', '“', '，', '。',
          '《', '》', '/', '？', '·', '~', ' ']


# 半角，数字，全角符号


class Stack:  # 惊了，stack竟然不是python内置的数据结构
    def __init__(self):
        self.items = []

    # I have changed method name isEmpty to is_empty
    # because in your code you have used is_empty
    def is_empty(self):
        return self.items == []

    def push(self, item):
        self.items.append(item)

    def pop(self):
        return self.items.pop()

    def peek(self):
        return self.items[len(self.items) - 1]

    def size(self):
        return len(self.items)


class DFA:
    def __init__(self, word_type):
        self.dict = {}
        self.is_end = False
        self.word_type = word_type

    def add_sensitive_word(self, word, word_type):
        if not word[0] in self.dict:  # 若该字不在dfa_tree中，则增加该字
            self.dict[word[0]] = DFA(word_type)  # 添加 基本敏感字 识别

        if len(word) >= 2:  # 不是最后一个字
            self.dict[word[0]].add_sensitive_word(word.lstrip(word[0]), word_type)
        else:  # 最后一个字
            self.dict[word[0]].is_end = True


def is_chinese(ch):
    if '\u4e00' <= ch <= '\u9fff':
        return True
    return False


def read_file():  # 读入文件
    global file_in_words
    global file_in_org
    global file_out_ans
    global words
    global org

    # file_in_words = open(sys.argv[0], 'r', encoding='utf-8')
    # file_in_org = open(sys.argv[1], 'r', encoding='utf-8')
    # file_out_ans = open(sys.argv[2], 'w', encoding='utf-8')

    s1 = "words.txt"
    s2 = "org.txt"
    s3 = "ans.txt"
    file_in_words = open(s1, 'r', encoding='utf-8')
    file_in_org = open(s2, 'r', encoding='utf-8')
    file_out_ans = open(s3, 'w', encoding='utf-8')

    words = file_in_words.readlines()  # 得到敏感词表
    org = file_in_org.readlines()  # 得到要检测的文章


def write_file():  # 输出文件
    ans.insert(0, "Total: %d" % len(ans))
    for i in ans:
        file_out_ans.write(i + '\n')

    file_in_words.close()
    file_in_org.close()
    file_out_ans.close()


def deal_words():  # 处理敏感词表，建立敏感词查询表
    global dfa_tree

    dfa_tree = DFA(-1)  # 初始化敏感词树

    # 一个词一个词的放入
    for word_type, word in enumerate(words):  # word是一个敏感词
        words[word_type] = word = word.strip()  # 对word去除首尾空格和换行
        word = word.lower()  # 进入命案此查询表前，全部转小写
        dfa_tree.add_sensitive_word(word, word_type)  # 把word放入敏感词查询表




def find_word(sentence, i, now_tree, line_num):
    global word_stack

    while i < len(sentence) and sentence[i] in symbol:  # 无效符号放入词栈
        word_stack.push(sentence[i])
        i += 1

    if i < len(sentence) and sentence[i].lower() in now_tree.dict:  # 在敏感词查询表中
        lower_word = sentence[i].lower()  # 用小写形式查询，即sentence[i]全部改为lower_word
        word_stack.push(sentence[i])  # 放入词栈，需要是没有改变大小写前的字
        if now_tree.dict[lower_word].is_end:  # 查到完整敏感词
            word_size = word_stack.size()  # 保存找到的敏感词的大小
            s_temp = ""  # s_temp用来保存找到的敏感词，初始化
            while not word_stack.is_empty():
                s_temp = word_stack.pop() + s_temp
            ans.append("Line%d: <%s> %s" %
                       (line_num + 1, words[now_tree.dict[lower_word].word_type], s_temp))
            return word_size  # 返回找到的敏感词的大小
        else:  # 在敏感词查询表中，但没查到完整敏感词
            now_tree = now_tree.dict[lower_word]
            i += 1
            return find_word(sentence, i, now_tree, line_num)
    else:  # 不在敏感词查询表中，且不是无效符号
        return 1


def deal_org():  # 处理文本，得到答案
    global word_stack

    # 一句话一句话地检查
    for line_num, sentence in enumerate(org):  # 同时得到下标和元素
        sentence = sentence.strip()  # 删掉句子首尾空格

        i = 0
        while i < len(sentence):
            word_stack = Stack()
            if sentence[i].lower() in dfa_tree.dict:
                i += find_word(sentence, i, dfa_tree, line_num)  # 从下标i开始找敏感词
            else:
                i += 1  # 无效符号，下一个


ans = []

if __name__ == '__main__':
    read_file()  # 输入

    deal_words()  # 处理敏感词：建立敏感词表

    deal_org()  # 处理文本：检查敏感词

    write_file()  # 输出
