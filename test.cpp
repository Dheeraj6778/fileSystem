#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <dirent.h>
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
        c = fgetc(fp1);
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
        if (s[i] == ' ')
        {

            temp.push_back(tempString);
            tempString = "";
        }
        else
            tempString += s[i];
    }
    temp.push_back(tempString);
    return temp;
}
int isFile(string path)
{
    DIR *directory = opendir(path.c_str());

    if (directory != NULL)
    {
        closedir(directory);
        return 0;
    }

    if (errno == ENOTDIR)
    {
        return 1;
    }

    return -1;
}
void fun(string basePath)
{
    string tempPath = "";
    struct dirent *dp;
    DIR *dir = opendir(basePath.c_str());
    if (dir == NULL)
        return;
    while ((dp = readdir(dir)) != NULL)
    {
        string st1 = dp->d_name;
        if (st1 != "." and st1 != "..")
        {
            cout<<st1<<endl;
            tempPath = basePath;
            tempPath += "/" + st1;
            fun(tempPath);
        }
    }
    closedir(dir);
}
void deldir(string basePath)
{
    struct dirent *re;
    DIR *dir = opendir(basePath.c_str());
    if (dir == NULL)
        return;
    string tempPath = "";
    while ((re = readdir(dir)) != NULL)
    {
        string st1 = re->d_name;
        if (st1 != "." and st1 != "..")
        {
            tempPath = basePath;
            tempPath += "/" + st1;
            remove(tempPath.c_str());
            deldir(tempPath);
            //remove(tem)
        }
    }
    remove(basePath.c_str());
}
int main()
{
    string path = "./testt";
    cout << path << endl;
    deldir(path);
    return 0;
}