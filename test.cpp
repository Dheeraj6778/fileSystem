#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <filesystem>
using namespace std;
struct termios orig_termios;
namespace fs = std::filesystem;
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void editorRefreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
class Node
{
public:
    string FileName;
    int FileSize;
    string perm;
    string user_name;
    string group_name;
    string lastModified;
    Node(string Fname, int Fsize, string perm, string uName, string gName, string lmodified)
    {
        this->FileName = Fname;
        this->FileSize = Fsize;
        this->perm = perm;
        this->user_name = uName;
        this->group_name = gName;
        this->lastModified = lmodified;
    }
};
string extractFileName(string st)
{
    string temp = "";
    for (int i = st.size() - 1; i >= 0; i--)
    {
        if (st[i] == '/')
            break;
        else
            temp += st[i];
    }
    reverse(temp.begin(), temp.end());
    return temp;
}
int main()
{
    string path = "/mnt/c/Users/Anonymous/Desktop/AOS";
    for (auto &file : fs::directory_iterator(path))
    {
        struct stat info;
        string fp = file.path();
        stat(fp.c_str(), &info);
        
        if(fs::is_directory(fp)){
            cout << "directory " << info.st_size << endl;
        }
        else
            cout<<"file "<<info.st_size<<endl;
        
    }
    return 0;
}