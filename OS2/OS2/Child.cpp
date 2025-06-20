#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "RU");

   // cout << "HI!" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));

    if (argc < 2)
    {
        cout << "Ошибка: недостаточно аргументов командной строки." << endl;
        return 1;
    }

    int size = stoi(argv[1]);

    cout << size << endl;
    this_thread::sleep_for(chrono::milliseconds(500));
    vector<char> symbols;
    for (int i = 2; i < argc; ++i)
    {
        for (char c : string(argv[i]))
        {
            symbols.push_back(c);
            /*cout << argv[i] << endl;
            this_thread::sleep_for(chrono::milliseconds(500));*/
        }
        symbols.push_back(' ');
    }
    symbols.push_back('1');
    
    vector<string> lexemes;
    string current;
    for (char c : symbols)
    {
        if (isdigit(c))  
        {
            if (!current.empty())
            {
                cout << current << " ";
                lexemes.push_back(current);
                current.clear();
            }
        }
        else
        {
            current.push_back(c);
        }
    }
    if (!current.empty())
    {
        lexemes.push_back(current);
    }

    lexemes.push_back(current);

    /*vector<char> output(size, '0');
    int pos = 0;
    for (const string& lex : lexemes)
    {
        for (char c : lex)
        {
            if (pos < size)
                output[pos++] = c;
        }

        if (pos < size)
            output[pos++] = ' ';
    }


    cout << "Результат: ";
    for (char c : output)
    {
        cout << c;
    }
    cout << endl;*/

   // this_thread::sleep_for(chrono::milliseconds(1000));
    cin >> size;
    return 0;
}
