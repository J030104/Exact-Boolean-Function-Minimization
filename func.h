#ifndef FUNC_H
#define FUNC_H

#include <string>
#include <vector>
using namespace std;

struct Term
{
    string T;
    string type;
};

struct strWithSym
{
    string T;
    char sym;
};

struct strWithIdx
{
    string T;
    int idx;
};

bool startsWith(const string &str, const string &prefix);
string decimalToBinary(int decimalNumber, int numBits);
int countOnes(const string &binaryString);
void sortBinaryStrings(vector<string> &binaryStrings);
void check_PushBack(vector<string> &v, string s);
int canReduce(string s, string t, int NOI, vector<int> &pos);
bool canAlign(string s1, string s2);
string mul(string t1, string t2);
string add(string s1, string s2);
string rmDupChar(const string &inputStr);
string rmDupStr(const string &inputStr);
vector<string> findTerm_LeastLtrls(const string &inputStr);
bool noDuplicate(vector<strWithIdx> v, string s);

#endif // FUNC_H