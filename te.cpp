#include <bits/stdc++.h>
#include <iostream>
using namespace std;
vector<string> ve = {"abc", "def", "ghi", "jkl", "lmn"};
vector<string> ar = {"-->", "", "", "", ""};
void clear()
{
    cout << "\033[H\033[2J\033[3J";
}
void cursor(int x, int y)
{
    cout << "\033[" << x << ";" << y << "H";
}
void display(vector<string> v)
{
    clear();
    for(int i=0;i<ve.size();i++){
        cout<<ar[i]<<"\t"<<ve[i]<<endl;
    }
}
int main()
{
    int x;
    cin >> x;
    cout << x * 2 << endl;
    
    display(ve);
    int index = 0;
    while (1)
    {
        char ch = cin.get();
        if (ch == 'q')
            break;
        switch (ch)
        {
        case 'a':
            // up
            if (index - 1 >= 0)
            {
                ar[index] = "";
                index--;
                ar[index] = "-->";
                
                //cursor(0, 1);
                display(ve);
                //cursor(0, 14);
                cout<<"  ";
                //display(ar);
            }
            break;
        case 'b':
            // down
            if (index + 1 < ve.size())
            {
                ar[index] = "";
                index++;
                ar[index] = "-->";
                //clear();
               // cursor(0, 1);
                display(ve);
                //cursor(1, 14);
                //display(ar);
            }
            break;
        default:
            break;
        }
    }

    return 0;
}