#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <pwd.h>
#include <grp.h>
#include <stack>
#include <termios.h>
#include <unistd.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
using namespace std;
struct termios orig_termios;
namespace fs = std::filesystem;
int index = 0;
stack<string> pathStack;
stack<string> rightStack;
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
        cout << left << setw(25) << v[i].FileName << " ";
        cout << left << setw(10) << v[i].FileSize << "  ";
        cout << left << setw(10) << v[i].perm << " ";
        cout << left << setw(10) << v[i].user_name << " ";
        cout << left << setw(10) << v[i].group_name << " ";
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
void copy(string src_path, string dest_path)
{
    FILE *fp1;
    FILE *fp2;
    fp1 = fopen(src_path.c_str(), "r");
    if (fp1 == NULL)
    {
        cout << "cannot open the source file" << endl;
        return;
    }
    fp2 = fopen(dest_path.c_str(), "w");
    if (fp2 == NULL)
    {
        cout << "cannot open the destination file" << endl;
        return;
    }
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
void commandMode()
{
    while (true)
    {
        cout << "enter the command" << endl;
        string command;
        getline(cin, command);
        vector<string> commands = splitString(command);
        if (commands[0] == "quit")
            return;
        if (commands[0] == "copy")
        {
            vector<string> files;
            for (int i = 1; i <= commands.size() - 2; i++)
                files.push_back(commands[i]);
            string dest=commands.back();
            for(auto f: files)
            {
                string src=f;
                dest+="/"+src;
                copy(src,dest);
            }
        }
    }
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
    pathStack.push(path);
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
            if (pathStack.empty())
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
        else if (ch == 10)
        {

            path = (v[index]).filePath;
            if (fs::is_directory(path))
            {
                if (!pathStack.empty() and pathStack.top() != path)
                    pathStack.push(path);
                if (pathStack.empty())
                    pathStack.push(path);
                index = 0;
                showFilesandFolders(path);
                cout << path << endl;
            }
            else
            {
                // open the file
            }
        }
        else if (ch == 'D')
        {
            if (!pathStack.empty())
            {
                string temp_path = pathStack.top();
                rightStack.push(temp_path);
                pathStack.pop();
                if (!pathStack.empty())
                {
                    path = pathStack.top();
                    showFilesandFolders(path);
                    cout << path << endl;
                }
            }
            //}
        }
        else if (ch == 'C')
        {
            // right arrow
            if (!rightStack.empty())
            {
                path = rightStack.top();
                showFilesandFolders(path);
                cout << path << endl;
                rightStack.pop();
                pathStack.push(path);
            }
        }
        else if (ch == ':')
        {
            // command mode
            clear();
            disableRawMode();
            commandMode();
            enableRawMode();
        }
    }
}