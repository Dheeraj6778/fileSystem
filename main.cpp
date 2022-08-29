#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <pwd.h>
#include <grp.h>
#include<stack>
#include <termios.h>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <ctime>
using namespace std;
struct termios orig_termios;
namespace fs = std::filesystem;
int index = 0;
stack<string> pathStack;
class Node
{
public:
    string FileName;
    int FileSize;
    string perm;
    string user_name;
    string group_name;
    string lastModified;
    string filePath;
    Node(string Fname, int Fsize, string perm, string uName, string gName, string lmodified, string pa)
    {
        this->FileName = Fname;
        this->FileSize = Fsize;
        this->perm = perm;
        this->user_name = uName;
        this->group_name = gName;
        this->lastModified = lmodified;
        this->filePath = pa;
    }
};
vector<Node> v;
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
void clear()
{
    cout << "\033[H\033[2J\033[3J";
}
bool cmp(const Node &a, const Node &b)
{
    return a.FileName <= b.FileName;
}
void display(vector<Node> v)
{
    clear();
    sort(v.begin(), v.end(), cmp);
    for (int i = 0; i < v.size(); i++)
    {
        if (i == index)
        {
            cout << left << setw(5) << "-->";
        }
        else
            cout << left << setw(5) << "     ";
        cout << left << setw(30) << v[i].FileName << " ";
        cout << left << setw(10) << v[i].FileSize << "  ";
        cout << left << setw(20) << v[i].perm << " ";
        cout << left << setw(20) << v[i].user_name << " ";
        cout << left << setw(20) << v[i].group_name << " ";
        cout << left << setw(20) << v[i].lastModified << endl;
    }
}
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
pair<string, string> printPermissions(string path)
{
    const char *fname;
    fname = path.c_str();
    struct stat fs;
    stat(fname, &fs);
    // checking for user permissions
    string temp = "";
    if (fs.st_mode & S_IRUSR)
        temp += "r";
    else
        temp += "-";
    if (fs.st_mode & S_IWUSR)
        temp += "w";
    else
        temp += "-";
    if (fs.st_mode & S_IXUSR)
        temp += "x";
    else
        temp += "-";
    // checking for group permissions
    if (fs.st_mode & S_IRGRP)
        temp += "r";
    else
        temp += "-";
    if (fs.st_mode & S_IWGRP)
        temp += "w";
    else
        temp += "-";
    if (fs.st_mode & S_IXGRP)
        temp += "x";
    else
        temp += "-";
    // checking for others permissions
    if (fs.st_mode & S_IROTH)
        temp += "r";
    else
        temp += "-";
    if (fs.st_mode & S_IWOTH)
        temp += "w";
    else
        temp += "-";
    if (fs.st_mode & S_IXOTH)
        temp += "x";
    else
        temp += "-";
    // cout << left << setw(20) << temp << " ";
    string lmod = ctime(&fs.st_mtime);

    // cout << left << setw(20) << lmod << endl;
    return {temp, lmod};
}
void showFilesandFolders(string path)
{
    vector<Node> ve;
    for (auto &file : fs::directory_iterator(path))
    {
        string filePath = file.path();
        string FileName = extractFileName(filePath);
        struct stat info;
        stat(filePath.c_str(), &info);
        int FileSize = info.st_size;
        auto p = printPermissions(filePath);
        struct passwd *pw = getpwuid(info.st_uid);
        struct group *gr = getgrgid(info.st_gid);
        Node n(FileName, FileSize, p.first, pw->pw_name, gr->gr_name, p.second, filePath);
        ve.push_back(n);
    }
    // sort the files
    v = ve;
    display(v);
}
void nav(string path)
{
    for (auto &file : fs::recursive_directory_iterator(path))
    {
        string filePath = file.path();
        string FileName = extractFileName(filePath);
        struct stat info;
        stat(filePath.c_str(), &info);
        int FileSize = info.st_size;
        auto p = printPermissions(filePath);
        struct passwd *pw = getpwuid(info.st_uid);
        struct group *gr = getgrgid(info.st_gid);
        Node n(FileName, FileSize, p.first, pw->pw_name, gr->gr_name, p.second, filePath);
        v.push_back(n);
    }
}
int getBackSlashIndex(string s)
{
    int ind = 0;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        if (s[i] == '/')
        {
            ind = i;
            break;
        }
    }
    return ind;
}
void emptyStack()
{
    while (!pathStack.empty())
        pathStack.pop();
}
int main()
{
    string path = "/mnt/c/Users/Anonymous/Desktop/AOS";
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    string home = pw->pw_dir;
    showFilesandFolders(path);
    display(v);
    cout << path << endl;
    enableRawMode();
    while (1)
    {
        char ch = cin.get();
        if (ch == 'q')
            break;
        if (ch == 'A')
        {
            // up
            if (!pathStack.empty() and pathStack.top() != path)
                pathStack.push(path);
            if(pathStack.empty())
                pathStack.push(path);
            if (index - 1 >= 0)
            {
                index--;
                display(v);
                cout << path << endl;
            }
        }
        else if (ch == 'B')
        {
            // down
            if (!pathStack.empty() and pathStack.top() != path)
                pathStack.push(path);
            if (pathStack.empty())
                pathStack.push(path);
            if (index + 1 < v.size())
            {
                index++;
                display(v);
                cout << path << endl;
            }
        }
        else if (ch == 127)
        {
            // backspace implementation
            int backSlashIndex = getBackSlashIndex(path);
            if (backSlashIndex != 0)
            {
                path = path.substr(0, backSlashIndex);
                if (!pathStack.empty() and pathStack.top() != path)
                    pathStack.push(path);
                if (pathStack.empty())
                    pathStack.push(path);
                index = 0;
                showFilesandFolders(path);
                cout << path << endl;
            }
        }
        else if (ch == 'h')
        {
            // home
            path = home;
            if (!pathStack.empty() and pathStack.top() != path)
                pathStack.push(path);
            if (pathStack.empty())
                pathStack.push(path);
            index = 0;
            showFilesandFolders(path);
            cout << path << endl;
        }
        else if(ch==10)
        {
            path=(v[index]).filePath;
            if (!pathStack.empty() and pathStack.top() != path)
                pathStack.push(path);
            if (pathStack.empty())
                pathStack.push(path);
            index = 0;
            showFilesandFolders(path);
            cout << path << endl;
        }

    }
}