#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include "func.h"

using namespace std;

int main(int argc, char *argv[])
{
    string fileIn = argv[1];
    string fileOut = argv[2];

    // Parse file
    int NumOfIn;
    int NumOfOut;
    int NumOfTerms;
    int NumOfLtrls = 0;
    char funcName = 'F'; // F as default function Name
    vector<char> Ins;    // From .p
    vector<Term> Terms;  // This is read from the file

    // Processing
    vector<string> Minterms; // Processed terms, including DCs
    // Reduction process
    vector<string> GroupedTerms; // previously combined
    vector<string> NextGT;       // Put the freshly combined here
    vector<string> saved;        // PIs
    vector<strWithSym> savedWSym;

    vector<string> DCMinterms; // Used to check the DCs in Minterms
    vector<vector<int>> Table;
    vector<string> MintWoDC;
    vector<strWithIdx> EPI;
    vector<vector<int>> Table_final;
    vector<string> POS; // Like product-of-sum
    vector<string> candidates;

    set<int> idx;
    int progress_count = 0;

    ifstream pla(fileIn);
    if (pla.is_open())
    {
        string line;

        // Parse the file by headers
        while (getline(pla, line))
        {
            if (startsWith(line, ".i") && !startsWith(line, ".ilb"))
            { // Number of inputs
                if (sscanf(line.c_str(), "%*s %d", &NumOfIn) != 1)
                {
                    printf("Format error. => .i (int)\n");
                    return 1;
                }
            }
            else if (startsWith(line, ".o") && !startsWith(line, ".ob"))
            { // Number of outputs
                if (sscanf(line.c_str(), "%*s %d", &NumOfOut) != 1)
                {
                    printf("Format error. => .o (int)\n");
                    return 1;
                }
            }
            else if (startsWith(line, ".ilb"))
            { // Input variable names
                for (int i = 5; i < line.length(); i++)
                {
                    if (line[i] == '\n' || line[i] == '\r')
                    {
                        break;
                    }
                    else if (line[i] == ' ' || line[i] == '\t')
                    {
                        continue;
                    }
                    else
                    {
                        Ins.push_back(line[i]);
                    }
                }
                if (Ins.size() != NumOfIn)
                {
                    printf("Inconsistent input quantities!\n");
                    return 1;
                }
            }
            else if (startsWith(line, ".ob"))
            { // Output variable name (A function)
                if (sscanf(line.c_str(), "%*s %c", &funcName) != NumOfOut)
                {
                    printf("Inconsistent output quantities!\n");
                    return 1;
                }
            }
            else if (startsWith(line, ".p"))
            {   // The terms
                while (getline(pla, line))
                {
                    if (startsWith(line, ".e"))
                        break;
                    else if (startsWith(line, "\r") || startsWith(line, "\n"))
                        continue;
                    Term t;
                    t.T = line.substr(0, NumOfIn);
                    t.type = line.substr(NumOfIn + 1, 1);
                    Terms.push_back(t);
                }
                break;
            }
            else if (startsWith(line, ".e"))
            {
                break;
            }
            else
            { // No any header
                Term t;
                t.T = line.substr(0, NumOfIn);
                t.type = line.substr(NumOfIn + 1, 1);
                Terms.push_back(t);
                while (getline(pla, line))
                {
                    if (line == ".e")
                        break;
                    else if (startsWith(line, "\r") || startsWith(line, "\n"))
                        continue;
                    t.T = line.substr(0, line.size() - 2);
                    t.type = line.substr(NumOfIn + 1, 1);
                    Terms.push_back(t);
                }
                break;
            }
        }
        pla.close();
    }
    else
    {
        cerr << "Failed to open the .pla file." << endl;
    }

    // Generating minterms and DCs
    for (int i = 0; i < Terms.size(); i++)
    {                      // Go through every term
        string temp;       // It's a 2D vector (char** type)
        vector<int> DCidx; // The indices of the Don't Care variables in a term
        for (int j = 0; j < NumOfIn; j++)
        {
            temp += Terms[i].T[j];
            if (temp.back() == '-')
            {
                DCidx.push_back(j); // Keep the index of the DC
            }
        }
        int twoToTheDC = static_cast<int>(round(pow(2, DCidx.size()))); // 2^NumOfIn combinations
        if (twoToTheDC == 2)
        { // Only one DC
            // Assign a 0 or 1 to the DC (the first) position and save it (2D char vector <- 1D char vector)
            if (Terms[i].type == "-")
            {
                temp[DCidx[0]] = '0';
                check_PushBack(Minterms, temp);
                check_PushBack(DCMinterms, temp);
                temp[DCidx[0]] = '1';
                check_PushBack(Minterms, temp);
                check_PushBack(DCMinterms, temp);
            }
            else if (Terms[i].type == "1")
            {
                temp[DCidx[0]] = '0';
                check_PushBack(Minterms, temp);
                temp[DCidx[0]] = '1';
                check_PushBack(Minterms, temp);
            }
        }
        else if (twoToTheDC > 2)
        { // More than one DC
            for (int k = 0; k < twoToTheDC; k++)
            {                                                      // twoToTheDC = 4
                string bstring = decimalToBinary(k, DCidx.size()); // One of the possibilities
                for (int t = 0; t < DCidx.size(); t++)
                {                                // Assign a 0 or 1 to a DC position of a term
                    temp[DCidx[t]] = bstring[t]; // bstring should be DCidx.size() long
                }
                check_PushBack(Minterms, temp);
                if (Terms[i].type == "-")
                {
                    check_PushBack(DCMinterms, temp);
                }
            }
        }
        else if (DCidx.size() == 0)
        { // twoToTheDC == 1 (2^0 = 1, no DC)
            check_PushBack(Minterms, temp);
            if (Terms[i].type == "-")
            {
                check_PushBack(DCMinterms, temp);
            }
        }
    }

    // Sort the strings zero 1, one 1, two 1s ...
    sortBinaryStrings(Minterms); // This can enhance efficiency

    /*
    Combine minterms - starting from here
    */
    for (int i = 0; i < Minterms.size() - 1; i++)
    {
        vector<int> pos;
        for (int j = 0; j < Minterms[i].length(); j++)
        {
            if (Minterms[i][j] == '-')
            {
                pos.push_back(j);
            }
        }
        for (int j = i + 1; j < Minterms.size(); j++)
        {
            int k;
            if (k = canReduce(Minterms[i], Minterms[j], NumOfIn, pos))
            {
                idx.insert(i);
                idx.insert(j);
                string temp = Minterms[i];
                temp[k - 1] = '-';
                check_PushBack(GroupedTerms, temp);
            }
        }
    }
    for (int i = 0; i < Minterms.size(); i++)
    {
        if (idx.find(i) == idx.end())
        { // Not in the set (Cannot be combined with others, PI)
            saved.push_back(Minterms[i]);
        }
    }
    idx.clear();
    progress_count++;

    /*
    Protected contents
    */

    /*
    Combine minterms - end here
    */

    // Essential PI Identification
    // Initialize the first column
    Table.resize(saved.size() + 1);
    int alphabets = 'Z';
    Table[0].push_back(0);
    for (int i = 1; i <= saved.size(); i++)
    {
        Table[i].push_back(alphabets);
        strWithSym s;
        s.T = saved[i - 1];
        s.sym = alphabets;
        savedWSym.push_back(s);
        alphabets--;
    }
    // Initialize the first row
    for (int j = 0; j < Minterms.size(); j++)
    {
        auto it = find(DCMinterms.begin(), DCMinterms.end(), Minterms[j]);
        if (it == DCMinterms.end())
        { // If Minterms[i] is not DC, start identifying
            Table[0].push_back(stoi(Minterms[j], 0, 2)); // , Number of base 2
            MintWoDC.push_back(Minterms[j]);
        }
    }

    // Building table
    for (int i = 0; i < saved.size(); i++)
    {
        for (int j = 0; j < MintWoDC.size(); j++)
        {
            if (canAlign(MintWoDC[j], saved[i]))
            {
                Table[i + 1].push_back(1);
            }
            else
            {
                Table[i + 1].push_back(0);
            }
        }
    }

    // Searching EPI
    for (int i = 1; i <= MintWoDC.size(); i++)
    { // Check each term
        int sum = 0;
        int EPIidx = 0;
        for (int j = 1; j <= saved.size(); j++)
        { // Check if there is an EPI by looking at a column once
            if (Table[j][i] == 1)
            {
                sum++;
                EPIidx = j - 1;
            }
        }
        if (sum == 1)
        {
            if (noDuplicate(EPI, saved[EPIidx]))
            {
                strWithIdx temp;
                temp.T = saved[EPIidx];
                temp.idx = EPIidx + 1;
                EPI.push_back(temp);
            }
        }
    }

    // Creating the final version of Table
    // First delete the columns the EPIs have covered
    // Then delete the rows taken by EPIs
    for (int i = 0; i < EPI.size(); i++)
    { // Loop through each EPI column and row
        for (int j = 1; j < Table.size(); j++)
        { //
            if (j == EPI[i].idx)
            {                     // EPI found
                Table[j][0] = -1; // delete symbol
                for (int k = 0; k < Table[j].size(); k++)
                {
                    if (Table[j][k] == 1)
                    {
                        for (int r = 0; r < Table.size(); r++)
                        {
                            Table[r][k] = -1;
                        }
                    }
                    else if (Table[j][k] == 0)
                    {
                        Table[j][k] = -1;
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }

    // Generate possible combinations
    // Each column
    for (int i = 1; i <= MintWoDC.size(); i++)
    {
        if (Table[0][i] == -1)
            continue;
        string temp;
        for (int j = 1; j <= saved.size(); j++)
        {
            if (Table[j][i] == 1)
            {
                string t = "";
                t += (char)Table[j][0];
                temp = add(temp, t);
            }
        }
        POS.push_back(temp);
    }

    // Do multiplication
    string expanded = "";
    for (int i = 0; i < POS.size(); i++)
    {
        expanded = mul(expanded, POS[i]);
    }
    expanded = rmDupStr(expanded);

    // Pick shortest ones and find the term
    candidates = findTerm_LeastLtrls(expanded);

    if (candidates.size() > 0)
    {
        for (int i = 0; i < candidates[0].length(); i++)
        { // Use the first one
            for (int j = 0; j < savedWSym.size(); j++)
            {
                if (savedWSym[j].sym == candidates[0][i])
                {
                    strWithIdx temp;
                    temp.T = savedWSym[j].T;
                    temp.idx = -1; // To mark the candidates to make a differentiation
                    EPI.push_back(temp);
                }
            }
        }
    }

    NumOfTerms = EPI.size();
    for (int i = 0; i < EPI.size(); i++)
    {
        for (int j = 0; j < EPI[i].T.length(); j++)
        {
            if (EPI[i].T[j] == '1' || EPI[i].T[j] == '0')
            {
                NumOfLtrls++;
            }
        }
    }
    cout << "Total number of terms: " << NumOfTerms << endl;
    cout << "Total number of literals: " << NumOfLtrls << endl;

    ofstream outFile(fileOut);
    if (!outFile.is_open())
    {
        cerr << "Failed to open the .dot file." << endl;
        return 1;
    }

    // Write a simplified pla file
    outFile << ".i " << NumOfIn << endl;
    outFile << ".o " << NumOfOut << endl;
    outFile << ".ilb ";
    for (int i = 0; i < Ins.size(); i++)
    {
        outFile << Ins[i] << " ";
    }
    outFile << endl;
    outFile << ".ob " << funcName << endl;
    outFile << ".p " << EPI.size() << endl;
    for (int i = 0; i < EPI.size(); i++)
    {
        outFile << EPI[i].T << " 1" << endl;
    }
    outFile << ".e" << endl;
    outFile.close();

    return 0;
}