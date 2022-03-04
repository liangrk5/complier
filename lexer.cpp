#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

map<string, int> dict;
vector<pair<string,int>> wordlist;
vector<int> errorline;

// 判断是否是字符
bool isLetter(char ch);

// 判断是否是数字
bool isDigit(char ch);

// 判断是否是关键字
bool isKey(string word);

// 错误处理（记录当前错误的行号，并且跳过当前字符）
void error();

// 判断是否是单个运算符或界符
bool isSingle(char ch);

// 添加单词并回退指针
void addWordBack(ifstream &file, string &word);

//添加单词但不回退指针
void addWord(string &word);

// 词法分析主程序
void scanDFA(string filename);

// 初始化字类型对应表
void initMAP();

int main()
{
    initMAP();
    scanDFA("1.c");
    for(int i = 0;i < wordlist.size();i++)
    {
        cout << wordlist[i].first << " " << wordlist[i].second << endl;
    }
    return 0;
}

bool isLetter(char ch)
{
    if((ch >= 'a'&&ch <= 'z')||(ch >= 'A' &&ch <= 'Z'))
    {
        return true;
    }
    return false;
}

bool isDigit(char ch)
{
    if(ch >= '0' &&ch <= '9')
    {
        return true;
    }
    return false;
}

bool isSingle(char ch)
{
    string single = "?:^%()[]{},;~";
    for(int i = 0;i < single.size();i++)
    {
        if(ch == single[i])
            return true;
    }
    return false;
}

bool isKey(string word)
{
    if(dict.count(word) == 0)
    {
        return 0;
    }
    else
    {
        return dict[word];
    }
}

void addWordBack(ifstream &file, string &word)
{
    file.seekg(-1, ios::cur);
    if(isKey(word))
    {
        wordlist.push_back(make_pair(word, dict[word]));
    }
    else if(isDigit(word[0]))
    {
        wordlist.push_back(make_pair(word, 3));
    }
    else
    {
        wordlist.push_back(make_pair(word, 0));
    }
    word = "";
}

void addWord(string &word)
{
    if(word[0] == '"')
    {
        wordlist.push_back(make_pair(word, 2));
    }
    else if(word[0] == '\'')
    {
        wordlist.push_back(make_pair(word, 1));
    }
    else
        wordlist.push_back(make_pair(word, dict[word]));
    word = "";
}

void scanDFA(string filename)
{
    ifstream file(filename);
    if(!file.is_open())
    {
        cerr << "No such file!" << endl;
    }
    char buff;
    int state = 0;
    string word = "";
    int line = 0;
    do{
        switch(state)
        {
            case 0:
            {
                buff = file.get();
                if(isLetter(buff)||buff == '_')
                {
                    state = 1;
                    word += buff;
                    break;
                }
                else if(isDigit(buff))
                {
                    state = 3;
                    word += buff;
                    break;
                }
                else if(buff == '<')
                {
                    state = 11;
                    word += buff;
                    break;
                }
                else if(buff == '>')
                {
                    state = 14;
                    word += buff;
                    break;
                }
                else if(isSingle(buff))
                {
                    state = 0;
                    word += buff;
                    wordlist.push_back(make_pair(word,dict[word]));
                    word = "";
                    break;
                }
                else if(buff == '=')
                {
                    state = 18;
                    word += buff;
                    break;
                }
                else if(buff == '+')
                {
                    state = 21;
                    word += buff;
                    break;
                }
                else if(buff == '-')
                {
                    state = 24;
                    word += buff;
                    break;
                }
                else if(buff == '*')
                {
                    state = 26;
                    word += buff;
                    break;
                }
                else if(buff == '/')
                {
                    state = 29;
                    word += buff;
                    break;
                }
                else if(buff == '!')
                {
                    state = 37;
                    word += buff;
                    break;
                }
                else if(buff == '\n')
                {
                    state = 0;
                    word = "";
                    line++;
                    break;
                }
                else if(buff == '"')
                {
                    state = 41;
                    word += buff;
                    break;
                }
                else if(buff == '&')
                {
                    state = 43;
                    word += buff;
                    break;
                }
                else if(buff == '|')
                {
                    state = 46;
                    word += buff;
                    break;
                }
                else if(buff == '#')
                {
                    state = 49;
                    word += buff;
                    break;
                }
                else if(buff == '\'')
                {
                    state = 51;
                    word += buff;
                    break;
                }
                else
                {
                    break;
                }
            }
            case 1:
            {
                buff = file.get();
                if(isLetter(buff)||isDigit(buff)||buff == '_')
                {
                    state = 1;
                    word += buff;
                    break;
                }
                else
                {
                    state = 0;
                    addWordBack(file, word);
                    break;
                }
            }
            case 3:
            {
                buff = file.get();
                if(isDigit(buff))
                {
                    state = 3;
                    word += buff;
                    break;
                }
                else if(buff == '.')
                {
                    state = 5;
                    word += buff;
                    break;
                }
                else if(buff == 'E' || buff == 'e')
                {
                    state = 7;
                    word += buff;
                    break;
                }
                else
                {
                    state = 0;
                    addWordBack(file, word);
                    break;
                }
            }
            case 5:
            {
                buff = file.get();
                if(isDigit(buff))
                {
                    state = 6;
                    word += buff;
                    break;
                }
                else
                {
                    state = 100;    //错误处理
                    break;
                }
            }
            case 6:
            {
                buff = file.get();
                if(isDigit(buff))
                {
                    state = 6;
                    word += buff;
                    break;
                }
                else if(buff == 'E' || buff == 'e')
                {
                    state = 7;
                    word += buff;
                    break;
                }
                else
                {
                    state = 0;
                    addWordBack(file, word);
                    break;
                }
            }
            case 7:
            {
                buff = file.get();
                if(buff == '+' || buff == '-')
                {
                    state = 8;
                    word += buff;
                    break;
                }
                else if(isDigit(buff))
                {
                    state = 9;
                    word += buff;
                    break;
                }
                else
                {
                    state = 100;
                    break;
                }
            }
            case 8:
            {
                buff = file.get();
                if(isDigit(buff))
                {
                    state = 9;
                    word += buff;
                    break;
                }
                else
                {
                    state = 100;
                    break;
                }
            }
            case 9:
            {
                buff = file.get();
                if(isDigit(buff))
                {
                    state = 9;
                    word += buff;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 11:
            {
                buff = file.get();
                if(buff == '=' || buff == '<')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 14:
            {
                buff = file.get();
                if(buff == '=' || buff == '>')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 18:
            {
                buff = file.get();
                if(buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 21:
            {
                buff = file.get();
                if(buff == '+' || buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 24:
            {
                buff = file.get();
                if(buff == '-'||buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 26:
            {
                buff = file.get();
                if(buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 29:
            {
                buff = file.get();
                if(buff == '*')
                {
                    state = 30;
                    break;
                }
                else if(buff == '/')
                {
                    state = 33;
                    break;
                }
                else if(buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 30:
            {
                buff = file.get();
                if(buff == '*')
                {
                    state = 31;
                    break;
                }
                else
                {
                    state = 30;
                    break;
                }
            }
            case 31:
            {
                buff = file.get();
                if(buff == '/')
                {
                    state = 0;
                    break;
                }
                else
                {
                    state = 30;
                    break;
                }
            }
            case 33:
            {
                buff = file.get();
                if(buff == '\n')
                {
                    state = 0;
                    break;
                }
                else
                {
                    state = 33;
                    break;
                }
            }
            case 37:
            {
                buff = file.get();
                if(buff == '=')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 41:
            {
                buff = file.get();
                if(buff == '"')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else if(buff == '\n')
                {
                    state = 100;
                    break;
                }
                else
                {
                    state = 41;
                    word += buff;
                    break;
                }
            }
            case 43:
            {
                buff = file.get();
                if(buff == '&')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 46:
            {
                buff = file.get();
                if(buff == '|')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    addWordBack(file, word);
                    state = 0;
                    break;
                }
            }
            case 49:
            {
                buff = file.get();
                if(buff == 'i')
                {
                    string tmp = "";
                    while((buff=file.get()) != '<')
                    {
                    }
                    while((buff=file.get()) != '>')
                    {
                        tmp += buff;
                    }
                    wordlist.push_back(make_pair(tmp, 65));
                    word = "";
                    state = 0;
                    break;
                }
            }
            case 51:
            {
                buff = file.get();
                word += buff;
                state = 52;
                break;
            }
            case 52:
            {
                buff = file.get();
                if(buff == '\'')
                {
                    word += buff;
                    addWord(word);
                    state = 0;
                    break;
                }
                else
                {
                    state = 100;
                    break;
                }
            }
            case 100:
            {
                file.seekg(-1, ios::cur);
                errorline.push_back(line);
                state = 0;
                break;
            }
        }
    }while(buff != EOF);
    file.close();
}

void initMAP()
{
    ifstream file("word.txt");
    if(!file.is_open())
    {
        cerr << "No word txt!" << endl;
    }
    for(int i = 0;i < 64;i++)
    {
        string name;
        int tmp;
        file >> name >> tmp;
        dict[name] = tmp;
    }
    file.close();
}