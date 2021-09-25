/*
\\(1）可以识别出用C语言编写的源程序中的每个单词符号,并以记号的形式输出每个单词符号。
\\(2）可以识别并跳过源程序中的注释。
\\(3）可以统计源程序中的语句行数、各类单词的个数、以及字符总数，并输出统计结果。
(4）检查源程序中存在的词法错误,并报告错误所在的位置。
(5）对源程序中出现的错误进行适当的恢复，使词法分析可以继续进行，对源程序进行一次扫描,即可检查并报告源程序中存在的所有词法错误。
*/

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cctype>

using namespace std;

#define reservedWord 1
#define operator 2
#define delimiter 3
#define identifier 4
string typeOfWords[5] = {"null", "reservedWord", "operator", "delimiter", "identifier"};

string resource[1000];
string buffer[100];
int line = 0;
int ptr = -1;

class Words {
public:
    string letter;
    int type;
    int times = 0;
};
class typeWords {
public:
    int num = 0;
    Words words[100];
};
unordered_map<string, Words> wordsMap;
unordered_map<string, typeWords> typeMap;

void readResource();
void readCharacter();
void scanner();
void readCharacter() {
    ifstream infile;
    infile.open("/Users/whjandhisstars/CLionProjects/Analysis_whj/character.txt", ios::in);
    if(!infile.is_open()) {
        cout << "can not open this file to read character, please check your pathand name character.txt" << endl;
        exit(0);
    }
    string buf;
    int type = 1;
    while(getline(infile, buf)) {
        if(std::equal(buf.begin(), buf.end(), "???")) {
            type++;
        }
        else {
            Words tem;
            tem.letter = buf;
            tem.type = type;
            wordsMap[buf] = tem;
        }
    }
}

void readResource() {
    ifstream infile;
    infile.open("/Users/whjandhisstars/CLionProjects/Analysis_whj/test1.cpp", ios::in);
    if(!infile.is_open()) {
        cout << "can not open this file to read character, please check your pathand name test1.cpp" << endl;
        exit(0);
    }
    int i = 0;
    while(getline(infile, buffer[i])) {
        i++;
    }
    line = i;
}

/*
 *可以识别并跳过源程序中的注释。
 * 读取保留字、界符、运算符
 * 读取源文件并进行预处理
 */
void scanner() {
    bool skip = false;
    string buf;
    for(int i = 0; i < line; i++) {
        bool flag = true;
        if(buffer[i].find("//") && buffer[i].find("*/") && buffer[i].find("/*") && !skip) {
            ptr++;
        }
        for(int j = 0; j < buffer[i].size() && flag; j++) {
            char tem1 = buffer[i][j], tem2 = buffer[i][j+1];
            //用于判断出现/*是否遇到*/，没遇到之前就skip跳过该行
            if(skip && tem1 != '*' && tem2 != '/') {
                flag = false;
            }
            //出现//则跳过该行
            else if(tem1 == '/' && tem2 == '/') {
                flag = false;
            }
            //出现/*则跳过该行并将skip置为true
            else if(tem1 == '/' && tem2 == '*') {
                skip = true;
                flag = false;
            }
            //遇到*/，skip置false
            else if(tem1 == '*' && tem2 == '/') {
                skip = false;
                flag = false;
            }
            //非定界符
            else {
                //若不是空格则加入缓存字符串
                if(tem1 != ' ' && tem1 != '\n') {
                    //如果是字母开头
                    if(isalpha(tem1) == 1) {
                        while(isalpha(buffer[i][j]) || isdigit(buffer[i][j])) {
                            //后面跟着字母或数字
                            buf += buffer[i][j];    //加入
                            j++;    //后移
                        }
                        if(wordsMap.find(buf) != wordsMap.end()) {  //该字符串出现过
                            wordsMap[buf].times++;
                        } else {    //为标识符
                            Words tem;
                            tem.type = identifier;
                            tem.times++;
                            tem.letter = buf;
                            wordsMap[buf] = tem;
                        }
                        resource[ptr] += buf;
                        buf.clear();    //清除缓存字符串
                        j--;
                    }
                    else if(isdigit(tem1)){  //如果是数字开头
                        while(isdigit(buffer[i][j])) {
                            buf += buffer[i][j];
                            j++;
                        }
                        if(wordsMap.find(buf) != wordsMap.end()) {  //  该字符串出现过
                            wordsMap[buf].times++;
                        } else {    //为标识符
                            Words tem;
                            tem.type = identifier;
                            tem.times++;
                            tem.letter = buf;
                            wordsMap[buf] = tem;
                        }
                        resource[ptr] += buf;
                        buf.clear();    //清除缓存字符串
                        j--;
                    }
                    else {
                        string t;
                        t += tem1;
                        if(wordsMap.find(t) != wordsMap.end() && wordsMap[t].type <= 3)
                        //首字符为符号
                        switch (tem1) {
                            case '!' : {
                                if(tem2 == '=') {   //识别为 !=
                                    wordsMap["!="].times++;
                                    resource[ptr] += "!=";
                                    j++;
                                }
                                else {
                                    resource[ptr] += "!";
                                    wordsMap["!"].times++;
                                }
                            }break;
                            case '<' : {
                                if(tem2 == '=') {
                                    resource[ptr] += "<=";
                                    wordsMap["<="].times++;
                                    j++;
                                }
                                else if(tem2 == '<') {
                                    resource[ptr] += "<<";
                                    wordsMap["<<"].times++;
                                    j++;
                                }
                                else {
                                    resource[ptr] += "<";
                                    wordsMap["<"].times++;
                                }
                            }break;
                            case '>' : {
                                if(tem2 == '=') {
                                    resource[ptr] += ">=";
                                    wordsMap[">="].times++;
                                    j++;
                                }
                                else if(tem2 == '>') {
                                    resource[ptr] += ">>";
                                    wordsMap[">>"].times++;
                                    j++;
                                }
                                else {
                                    wordsMap[">"].times++;
                                    resource[ptr] += ">";
                                }
                            }break;
                            case '&' : {
                                if(tem2 == '&') {
                                    resource[ptr] += "&&";
                                    wordsMap["&&"].times++;
                                    j++;
                                }
                                else {
                                    resource[ptr] += "&";
                                    wordsMap["&"].times++;
                                }
                            }break;
                            case '|' : {
                                if(tem2 == '|') {
                                    resource[ptr] += "||";
                                    wordsMap["||"].times++;
                                    j++;
                                }
                                else {
                                    resource[ptr] += "|";
                                    wordsMap["|"].times++;
                                }
                            }break;
                            case '=' : {
                                if(tem2 == '=') {
                                    resource[ptr] += "==";
                                    wordsMap["=="].times++;
                                    j++;
                                }
                                else {
                                    resource[ptr] += "=";
                                    wordsMap["="].times++;
                                }
                            }break;
                        }
                        if(wordsMap.find(t) != wordsMap.end()) {
                            wordsMap[t].times++;
                            resource[ptr] += t;
                        }
                        else {
                            cout << "error: illegal characters" << endl
                            << buffer[i] << endl;
                            for(int k = 0; k < j; k++)
                                cout << ' ';
                            cout << '^' << endl;
                        }
                    }
                }
                //如果首字符为空格、换行符
                else {
                    if(tem1 == ' ')
                        resource[ptr] += tem1;
                }
            }
        }
    }
}


int main() {
    readCharacter();
    readResource();
    scanner();

    for(auto i = wordsMap.begin(); i != wordsMap.end(); i++) {
        if(i->second.times > 0) {
            typeMap[typeOfWords[i->second.type]].words[typeMap[typeOfWords[i->second.type]].num] = i->second;
            typeMap[typeOfWords[i->second.type]].num++;
            cout << '<'
                 << typeOfWords[i->second.type] << ' '
                 << i->second.letter << ' '
                 << i->second.times
                 << '>'
                 << endl;
        }

    }
    cout << endl;
    for(auto i = 1; i < 5; i++) {
        if(typeMap[typeOfWords[i]].num != 0)
            cout << typeOfWords[i] << ' ' << typeMap[typeOfWords[i]].num << endl;
        for(int j = 0; j < typeMap[typeOfWords[i]].num; j++)
        cout << typeMap[typeOfWords[i]].words[j].letter << endl;
    }
    for(int i = 0; i <= ptr; i++)
        cout << resource[i] << endl;
    return 0;
}

