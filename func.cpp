#include <algorithm>
#include <sstream>
#include <vector>
#include <bitset>
#include <string>
#include <set>
#include "func.h"

using namespace std;

bool startsWith(const string &str, const string &prefix)
{
    if (str.length() < prefix.length())
        return false;
    return str.compare(0, prefix.length(), prefix) == 0;
}

string decimalToBinary(int decimalNumber, int numBits)
{
    bitset<64> bits(decimalNumber);
    // Extract the rightmost n bits. 64 - numBits is the starting position
    string binaryString = bits.to_string().substr(64 - numBits);
    return binaryString;
}

int countOnes(const string &binaryString)
{
    int count = 0;
    for (char c : binaryString)
    {
        if (c == '1')
        {
            count++;
        }
    }
    return count;
}

void sortBinaryStrings(vector<string> &binaryStrings)
{
    sort(binaryStrings.begin(), binaryStrings.end(), [](const string &a, const string &b)
         {
             return countOnes(a) < countOnes(b); // Lambda function
         });
}

void check_PushBack(vector<string> &v, string s)
{
    auto it = find(v.begin(), v.end(), s);
    if (it == v.end())
        v.push_back(s); // If s not found, then push_back into
}

int canReduce(string s, string t, int NOI, vector<int> &pos)
{
    /*
    NOI is Number Of Inputs
    pos.size() is the number of DCs

    string s searching for a t

    If c is one, that means there is exactly
    one-bit difference between the two strings
    Return value 1 means the two strings can be combined
    */

    // Check if DC positions match first
    for (int i = 0; i < pos.size(); i++)
    {
        if (s[pos[i]] == t[pos[i]]) continue;
        else return 0;
    }
    int c = 0;
    int position = 0;
    for (int i = 0; i < NOI; i++)
    {
        if (s[i] == '-')
            continue;
        if (s[i] != t[i])
        {
            c++;
            position = i + 1; // ith position of the number(starting from 1)
        }
    }
    if (c == 1)
    {
        return position;
    }
    return 0;
}

bool canAlign(string s1, string s2)
{
    /*
    s1 is a minterm (not DC)
    s2 is in a reduced form
    see if the two numbers can align (0000 -0-0 okay) (0000 -01- NG)
    */
    if (s1.length() != s2.length())
        return false;
    for (int i = 0; i < s1.length(); i++)
    {
        if (s2[i] == '-')
        {
            continue;
        }
        else
        {
            if (s1[i] != s2[i])
                return false;
        }
    }
    return true;
}

string mul(string t1, string t2)
{
    /*
    Designed for two-term multiplication
    (A+B)(C+D+E) = AC + AD + AE + BC + BD + BE
    (A+B)(A+CD) = A + AB + ACD + BCD
    ABCD are strings (not of specific length)
    */

    if (t1 == "")
    {
        return t2;
    }
    else if (t2 == "")
    {
        return t1;
    }

    vector<string> one;
    vector<string> two;

    // Extract
    string s = "";
    for (int i = 0; i < t1.length(); i++)
    {
        if (t1[i] == '+')
        {
            one.push_back(s);
            s.clear();
        }
        else
        {
            s += t1[i];
            if (i == t1.length() - 1)
            {
                one.push_back(s);
                s.clear();
            }
        }
    }
    for (int i = 0; i < t2.length(); i++)
    {
        string s = "";
        if (t2[i] == '+')
        {
            continue;
        }
        s += t2[i];
        two.push_back(s);
    }

    string res = "";
    for (int i = 0; i < one.size(); i++)
    {
        for (int j = 0; j < two.size(); j++)
        {
            string combined = one[i] + two[j];
            combined = rmDupChar(combined);
            res = add(res, combined);
        }
    }
    return res;
}

string add(string s1, string s2)
{
    if (s1 == "")
    {
        return s2;
    }
    string s = s1 + "+" + s2;
    return s;
}

string rmDupChar(const string &inputStr)
{
    set<char> temp(inputStr.begin(), inputStr.end());
    string res(temp.begin(), temp.end());

    return res;
}

string rmDupStr(const string &inputStr)
{
    set<string> strSet;

    istringstream iss(inputStr);
    string token;

    while (getline(iss, token, '+'))
    {
        strSet.insert(token);
    }

    token.clear();
    for (auto it = strSet.begin(); it != strSet.end(); ++it)
    {
        token = add(token, *it);
    }
    return token;
}

vector<string> findTerm_LeastLtrls(const string &inputStr)
{
    istringstream iss(inputStr);
    string s;
    vector<string> t;
    int leastltrl = 10;

    while (getline(iss, s, '+'))
    {
        t.push_back(s);
        if (s.length() < leastltrl)
            leastltrl = s.length();
    }

    vector<string> res;
    for (int i = 0; i < t.size(); i++)
    {
        if (t[i].length() == leastltrl)
        {
            res.push_back(t[i]);
        }
    }

    return res;
}

bool noDuplicate(vector<strWithIdx> v, string s)
{
    for (int i = 0; i < v.size(); i++)
    {
        if (s == v[i].T)
            return false;
    }
    return true;
}