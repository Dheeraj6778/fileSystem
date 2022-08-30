#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
using namespace std;
struct termios orig_termios;
void copy(string src_path, string dest_path)
{
    FILE *fp1;
    FILE *fp2;
    fp1 = fopen(src_path.c_str(), "r");
    fp2 = fopen(dest_path.c_str(), "w");
    char c = fgetc(fp1);
    while (c != EOF)
    {
        fputc(c, fp2);
        c=fgetc(fp1);
    }
    fclose(fp1);
    fclose(fp2);
}
vector<string> splitString(string s)
{
    vector<string> temp;
    string tempString = "";
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == ' '){
            
            temp.push_back(tempString);
            tempString = "";
        }
        else
            tempString += s[i];
    }
    temp.push_back(tempString);
    return temp;
}
int main()
{
    string path = "te.cpp";
    string dest = "./testt/copied.txt";
    copy(path,dest);
}