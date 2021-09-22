import sys
from pypinyin import pinyin, lazy_pinyin, Style  # 拼音库
import changeCode  # 繁简体字库
from chai_zi import chai_zi

global file_in_words  # 读入的words文件
global file_in_org  # 读入的org文件
global file_out_ans  # 输出的ans文件
global words  # 敏感词表的内容
global org  # 要检测的文章的内容
global ans  # 要输出的答案

global dfa_tree  # 建成的敏感词树
global symbol  # 无效符号集合
global word_stack  # 词栈，用来保存检查过程中发现的一个敏感词
global words_map  # 映射表，保存敏感词各个单字的各种情况，注意是单字，末尾以原版敏感词单字为结尾
global flag  # 全局信号，判断某个敏感词是否走通
global homo  # 全局信号，判断是否有同音字在映射表中
global homo_now_map  # 全局信号，在出现同音字的时候，停在拼音的最后一个字母

# 基本思想：朴素的敏感词查询表 + 详细的敏感词映射表

# 半角，数字，全角符号
symbol = ['!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=',
          '|', '\\', '[', ']', '{', '}', ';', ':', "'", '"', ',', '.', '<', '>',
          '/', '?', '`', '~', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
          '！', '@', '#', '￥', '%', '……', '…', '&', '*', '（', '）', '——', '-', '+',
          '=', '【', '】', '{', '}', '|', '、', '；', '：', '‘', '“', '，', '。',
          '《', '》', '/', '？', '·', '~', ' ', '—']


# 模拟类
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


# DFA树结点
class DFA:
    def __init__(self, word_type=-1, is_end=False):
        self.dict = {}
        self.is_end = is_end
        self.word_type = word_type

    # 建立敏感词查询表、敏感词映射表
    def add_sensitive_word(self, word, word_type):
        if not word[0] in self.dict:  # 若该字不在dfa_tree中，则增加该字
            self.dict[word[0]] = DFA(word_type)  # 对 查询表 添加 基本敏感字 识别

        # n问映射表
        if word[0] not in words_map.dict:  # 映射表 是否有word[0]这一单字
            words_map.dict[word[0]] = DFA(word_type)  # 无则添加
        if word[0] not in words_map.dict[word[0]].dict:  # 映射表word[0]单字下，是否有终点word[0]
            words_map.dict[word[0]].dict[word[0]] = DFA(word_type, True)  # 无则添加
        # 添加映射：字本身 -> 字本身
        if is_chinese(word[0]):  # word[0] 是否为中文
            add_full_pinyin_to_words_map(words_map, word[0], word_type)  # 对 映射表 添加 全拼音识别
            # 添加映射：全拼音 -> 字本身
            add_title_pinyin_to_words_map(words_map, word[0], word_type)  # 对 映射表 添加 首拼音识别
            # 添加映射：首拼音 -> 字本身
            add_tradition_to_words_map(words_map, word[0], word_type)  # 对 映射表 添加 繁体字识别
            # 添加映射：繁体字 -> 字本身
            add_side_split_to_words_map(words_map, word[0], word_type)  # 对 映射表 添加 偏旁拆字识别
            # 添加映射：偏旁拆字 -> 字本身

        if len(word) >= 2:  # 不是最后一个字
            self.dict[word[0]].add_sensitive_word(word.lstrip(word[0]), word_type)
        else:  # 最后一个字
            self.dict[word[0]].is_end = True


# 是否为中文
def is_chinese(ch):
    if '\u4e00' <= ch <= '\u9fff':
        return True
    return False


# 在敏感词映射表中是否存在同音字
def homophonic(single_word, now_map):
    global homo
    global homo_now_map

    homo = False
    single_word_pinyin = lazy_pinyin(single_word)  # 转换全拼音
    for i in single_word_pinyin[0]:
        if i not in now_map.dict:
            return homo
        else:
            now_map = now_map.dict[i]

    for key in now_map.dict:
        if now_map.dict[key].is_end:
            homo = True
            homo_now_map = now_map
            return homo

    return homo


# 添加 全拼音识别 到 敏感词映射表
def add_full_pinyin_to_words_map(now_map, single_word, word_type):
    single_word_pinyin = lazy_pinyin(single_word)  # 转换全拼音
    for i in single_word_pinyin[0]:  # 每个拼音字母加入words_map
        if i not in now_map.dict:
            now_map.dict[i] = DFA(word_type)
        now_map = now_map.dict[i]

    if single_word not in now_map.dict:
        now_map.dict[single_word] = DFA(word_type)
        now_map.dict[single_word].is_end = True


# 添加 首拼音识别 到 敏感词映射表
def add_title_pinyin_to_words_map(now_map, single_word, word_type):
    single_word_pinyin = pinyin(single_word, style=Style.FIRST_LETTER)  # 转换首拼音
    for i in single_word_pinyin[0]:  # 每个拼音字母加入words_map
        if i not in now_map.dict:
            now_map.dict[i] = DFA(word_type)
        now_map = now_map.dict[i]

    if single_word not in now_map.dict:
        now_map.dict[single_word] = DFA(word_type)
        now_map.dict[single_word].is_end = True


# 添加 繁体字识别 到 敏感词映射表
def add_tradition_to_words_map(now_map, single_word, word_type):
    tradition_single_word = changeCode.toTraditionString(single_word)  # 转换繁体字
    for i in tradition_single_word[0]:  # 每个繁体字加入words_map
        if i not in now_map.dict:
            now_map.dict[i] = DFA(word_type)
        now_map = now_map.dict[i]

    if single_word not in now_map.dict:
        now_map.dict[single_word] = DFA(word_type)
        now_map.dict[single_word].is_end = True


# 添加 拆分字识别 到 敏感词映射表
def add_side_split_to_words_map(now_map, single_word, word_type):
    side_split_single_word = chai_zi[single_word]  # 偏旁拆字
    for i in side_split_single_word:  # 每个偏旁拆字加入words_map
        if i not in now_map.dict:
            now_map.dict[i] = DFA(word_type)
        now_map = now_map.dict[i]

    if single_word not in now_map.dict:
        now_map.dict[single_word] = DFA(word_type)
        now_map.dict[single_word].is_end = True


# 搞点初始化
def do_some_initial():
    global ans
    global dfa_tree
    global words_map

    ans = []
    dfa_tree = DFA()  # 初始化敏感词树
    words_map = DFA()  # 初始化映射表


# 读文件
def read_file():
    global file_in_words
    global file_in_org
    global file_out_ans
    global words
    global org

    file_in_words = open(sys.argv[0], 'r', encoding='utf-8')
    file_in_org = open(sys.argv[1], 'r', encoding='utf-8')
    file_out_ans = open(sys.argv[2], 'w', encoding='utf-8')

    # s1 = "words.txt"
    # s2 = "org.txt"
    # s3 = "ans.txt"
    # file_in_words = open(s1, 'r', encoding='utf-8')
    # file_in_org = open(s2, 'r', encoding='utf-8')
    # file_out_ans = open(s3, 'w', encoding='utf-8')

    words = file_in_words.readlines()  # 得到敏感词表
    org = file_in_org.readlines()  # 得到要检测的文章


# 写文件
def write_file():
    ans.insert(0, "Total: %d" % len(ans))
    for i in ans:
        file_out_ans.write(i + '\n')

    file_in_words.close()
    file_in_org.close()
    file_out_ans.close()


# 处理敏感词列表，得到 敏感词查询表 + 敏感词映射表
def deal_words():  # 处理敏感词表，建立敏感词查询表
    global dfa_tree

    # 一个词一个词的放入
    for word_type, word in enumerate(words):  # word是一个敏感词
        words[word_type] = word = word.strip()  # 对word去除首尾空格和换行
        word = word.lower()  # 进入敏感词查询表前，全部转小写
        dfa_tree.add_sensitive_word(word, word_type)  # 把word放入敏感词查询表


# 对句子寻找敏感词
def find_word(sentence, i, line_num, now_dfa_tree):  # 从sentence下标i开始，目标是找到整个敏感词
    global word_stack
    global flag
    global homo
    global homo_now_map

    flag = False
    # 每一次找新的敏感词都要复原为False，只需要走通一次，find_word函数的功能就算实现了

    while i < len(sentence):
        # 找下一个单字前，拉入干扰符号
        while i < len(sentence) and sentence[i] in symbol:  # 从整个敏感词的层面看，无效符号放入词栈
            word_stack.push(sentence[i])
            i += 1

        # 直到不是干扰符号，开始找单字
        j = i
        now_map = words_map

        # 第一个不是干扰符号的字，不在映射表中，则表示敏感词被“非干扰符号”分割了
        if j >= len(sentence) or (sentence[j].lower() not in now_map.dict \
                and (is_chinese(sentence[j]) and not homophonic(sentence[j], now_map))):
            return 1  # 敏感词被打断，单步前进
        else:
            # 这里认为单字不能被干扰字符打断，如果有单字，应当是完整的，跟着映射表走直到（找到源字）或者（被打断）

            j_saved = j
            now_map_saved = now_map
            # 要么这个字本身就在映射表里（可能是错的）
            self_in_map = False
            while j < len(sentence) and sentence[j].lower() in now_map.dict:
                self_in_map = True
                now_map = now_map.dict[sentence[j].lower()]
                j += 1
            if self_in_map:
                # 走不通了，当前j落在走不通的位置，now_map是最后走的通的位置
                find_origin_word = False  # 是否找到源字
                origin_word_is_end = False  # 源字是否为某个敏感词的最后一个字
                origin_word_type = -1  # 源字属于哪个敏感词
                now_dfa_tree_saved = now_dfa_tree  # 备份now_dfa_tree
                for key in now_map.dict:
                    # 如果当前now_map的dict中存在终点（源字）（就是这里的key），
                    # 则认为sentence从i到j-1的字符的源字 可能 为该字
                    # 即找到一个不确定是否为当前字符源字的源字
                    if now_map.dict[key].is_end:
                        if key in now_dfa_tree.dict:
                            find_origin_word = True
                            origin_word_is_end = now_dfa_tree.dict[key].is_end
                            origin_word_type = now_dfa_tree.dict[key].word_type
                            now_dfa_tree = now_dfa_tree.dict[key]
                            # 找到了源字，并且dfa_tree相应位置上有对应字，但不确定是否为敏感词的正确源字
                        else:
                            # 找到了源字但是源字不在相应的dfa_tree位置上，看看其它源字
                            continue
                    else:
                        # 不是源字，看看下一个
                        continue

                    # 找到了源字，并且dfa_tree相应位置上有对应字，但不确定是否为敏感词的正确源字
                    # 总之先把i到j的字放入word_stack
                    for i_temp in range(i, j):
                        word_stack.push(sentence[i_temp])
                    if origin_word_is_end:
                        # 找到源字，并且这个源字是某个敏感词的最后一个字，也就是说找到完整敏感词了，记录答案
                        s_temp = ""
                        while not word_stack.is_empty():
                            s_temp = word_stack.pop() + s_temp
                        ans.append("Line%d: <%s> %s" %
                                   (line_num + 1, words[origin_word_type], s_temp))
                        flag = True  # 走通敏感词，发出全局信号
                        # j - i是最后一个源字对应的原文本长度
                        return j - i
                    else:
                        # 找到源字，但不是敏感词最后一个字，继续找下一个
                        # 问：如何判断走通了，如何判断没走通，并尝试下一个源字？
                        # 答：全局信号signal
                        l = find_word(sentence, j, line_num, now_dfa_tree)
                        if flag:
                            # 走通了，直接跳出程序
                            return j - i + l
                        else:
                            # 使用该源字没走通，恢复word_stack状态，并寻找下一个源字
                            while word_stack.peek() in symbol:
                                word_stack.pop()
                            for j_temp in range(i, j):
                                word_stack.pop()
                            find_origin_word = False
                            origin_word_is_end = False
                            origin_word_type = -1
                            now_dfa_tree = now_dfa_tree_saved
                            # 恢复到选择源字前的状态
                            continue

# ---------------------------------------------------------------- #

            j = j_saved
            now_map = now_map_saved
            # 要么这个字的谐音在映射表里
            homo_in_map = False
            while j < len(sentence) and is_chinese(sentence[j]) and homophonic(sentence[j], now_map):
                homo_in_map = True
                now_map = homo_now_map
                homo = False
                j += 1
            if homo_in_map:
                # 走不通了，当前j落在走不通的位置，now_map是最后走的通的位置
                find_origin_word = False  # 是否找到源字
                origin_word_is_end = False  # 源字是否为某个敏感词的最后一个字
                origin_word_type = -1  # 源字属于哪个敏感词
                now_dfa_tree_saved = now_dfa_tree  # 备份now_dfa_tree
                for key in now_map.dict:
                    # 如果当前now_map的dict中存在终点（源字）（就是这里的key），
                    # 则认为sentence从i到j-1的字符的源字 可能 为该字
                    # 即找到一个不确定是否为当前字符源字的源字
                    if now_map.dict[key].is_end:
                        if key in now_dfa_tree.dict:
                            find_origin_word = True
                            origin_word_is_end = now_dfa_tree.dict[key].is_end
                            origin_word_type = now_dfa_tree.dict[key].word_type
                            now_dfa_tree = now_dfa_tree.dict[key]
                            # 找到了源字，并且dfa_tree相应位置上有对应字，但不确定是否为敏感词的正确源字
                        else:
                            # 找到了源字但是源字不在相应的dfa_tree位置上，看看其它源字
                            continue
                    else:
                        # 不是源字，看看下一个
                        continue

                    # 找到了源字，并且dfa_tree相应位置上有对应字，但不确定是否为敏感词的正确源字
                    # 总之先把i到j的字放入word_stack
                    for i_temp in range(i, j):
                        word_stack.push(sentence[i_temp])
                    if origin_word_is_end:
                        # 找到源字，并且这个源字是某个敏感词的最后一个字，也就是说找到完整敏感词了，记录答案
                        s_temp = ""
                        while not word_stack.is_empty():
                            s_temp = word_stack.pop() + s_temp
                        ans.append("Line%d: <%s> %s" %
                                   (line_num + 1, words[origin_word_type], s_temp))
                        flag = True  # 走通敏感词，发出全局信号
                        # j - i是最后一个源字对应的原文本长度
                        return j - i
                    else:
                        # 找到源字，但不是敏感词最后一个字，继续找下一个
                        # 问：如何判断走通了，如何判断没走通，并尝试下一个源字？
                        # 答：全局信号signal
                        l = find_word(sentence, j, line_num, now_dfa_tree)
                        if flag:
                            # 走通了，直接跳出程序
                            return j - i + l
                        else:
                            # 使用该源字没走通，恢复word_stack状态，并寻找下一个源字
                            while word_stack.peek() in symbol:
                                word_stack.pop()
                            for j_temp in range(i, j):
                                word_stack.pop()
                            find_origin_word = False
                            origin_word_is_end = False
                            origin_word_type = -1
                            now_dfa_tree = now_dfa_tree_saved
                            # 恢复到选择源字前的状态
                            continue

            return 1  # 没有找到源字可以在dfa_tree上匹配，单步前进


# 对文章寻找敏感词
def deal_org():  # 处理文本，得到答案
    global word_stack

    # 一句话一句话地检查
    for line_num, sentence in enumerate(org):  # 同时得到下标和元素
        sentence = sentence.strip()  # 删掉句子首尾空格

        i = 0
        while i < len(sentence):
            if sentence[i].lower() in words_map.dict or homophonic(sentence[i], words_map):  # 单字sentence[i]是敏感词某种情况的开头
                word_stack = Stack()
                i += find_word(sentence, i, line_num, dfa_tree)
                # 从下标i开始找敏感词，传输：句子，下标，映射表，行号
            else:
                i += 1  # 无效符号，下一个


if __name__ == '__main__':
    do_some_initial()  # 初始化

    read_file()  # 输入

    deal_words()  # 处理敏感词：建立 敏感词查询表 + 敏感词映射表

    deal_org()  # 处理文本：检查敏感词

    write_file()  # 输出
