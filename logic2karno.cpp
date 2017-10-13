#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <utility>

using namespace std;

// Получить бит в позиции числа
int getBit(int bitpos, int num)
{
    int zerofirstbit = (num >> bitpos) << bitpos;
    return (num - zerofirstbit) >> (bitpos - 1);
}

// Переводит число в двоичное, разделяя старшие и младшие разряды в соответствии с dem
// first - старшие разряды числа
// second - младшие разряды числа
//
// Не используется.
// В планах заменить явно определенные массивы greyX и greyY на их вычисление.
pair<string,string> inttobin(pair<int,int> dem, int num)
{
    string first;
    string second;

    for (int i = dem.first + dem.second; i >= 1 ; i--){
        if (i >= dem.first)
            first += 0x30+getBit(i, num);
        else
            second += 0x30+getBit(i, num);
    }
    
    return make_pair(first, second);
}


// Необходимо переделать функцию вывода не привязываясь к жеским размерам таблизы Мура 4 на 8
void printTableKarno(string header, vector<vector<int>> &tablekarno)
{
    const char *red = "\033[1;31m";
    const char *white = "\033[1;37m";
    const char *purple = "\033[1;33;45m";
    const char *blue = "\033[1;33;44m";
    const char *nocol = "\033[0m";
    const char *blank = "";

    if (header == "  ")
        nocol = blue = purple = white = red = blank;

    vector<string> greyX = {"000", "001", "011", "010", "110", "111", "101", "100"};
    vector<string> greyY = {"00", "01", "11", "10"};

    printf("%s%s | ", white, header.c_str());
    for (auto &grey: greyX)
        printf(" %s ", grey.c_str());
    printf("\n---+-----------%s[---X4----%s[=X5==X4=]%s----X5---]%s\n", blue, purple, blue, nocol);

    // Из столбца table[y] выводим биты в соответстии с порядком определенном в seqout2x3
    for (int i = 0; i < greyY.size(); i++) {
        printf("%s%s |%s ", white, greyY[i].c_str(), nocol);
        for (int j = 0; j < greyX.size(); j++) {
            int out = tablekarno[i][j];
            if (out == 1) {
                printf("  %s%d%s  ", red, out, nocol);
            } else if (out == 0) {
                printf("  %d  ", out);
            } else {
                printf("  %sX%s  ", red, nocol);
            }
        }
        switch(i) {
            case 1: printf("%sX1  %s", blue, nocol); break;
            case 2: printf("%sX1X2%s", purple, nocol); break;
            case 3: printf("%s  X2%s", blue, nocol); break;
            default: printf(""); break;
        }
        printf("\n");
    }
    printf("---+------%s[---X3---]%s----------%s[---X3---]%s-----\n", blue, nocol, blue, nocol);
}


void karno(string header, int y, vector<vector<bool>> &table)
{
    vector<vector<int>> seqout3x2 = {{0, 4, 12,  8, 24, 28, 20, 16},
                                     {1, 5, 13,  9, 25, 29, 21, 17},
                                     {3, 7, 15, 11, 27, 31, 23, 19},
                                     {2, 6, 14, 10, 26, 30, 22, 18}};

    vector<vector<int>> tablekarno;

    for (auto &l: seqout3x2) {
        vector<int> line;
        for (auto &cel: l) {
            int out;
            try {
                out = table.at(cel).at(y);
            }catch(...) {
                out = -1;
            }
            line.push_back(out);
        }
        tablekarno.push_back(line);
        line.clear();
    }

    printTableKarno(header, tablekarno);
}


int readTrueTable(char *filename, vector<vector<bool>> &table)
{
    FILE *inputfile = fopen(filename, "r");
    if (inputfile == NULL) {
        return 1;
    }

    vector<bool> line;
    int c;
    while ((c = fgetc(inputfile)) != EOF) {
        switch(c) {
        case '\n':
            table.push_back(line);
            line.clear();
            break;
        case '1':
            line.push_back(true);
            break;
        case '0':
            line.push_back(false);
            break;
        }
    }
    fclose(inputfile);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 4)
        return 1;

    vector<vector<bool>> table;
    int resultread = readTrueTable(argv[1], table);
    if (resultread == 1) {
        return 2;
    }

    string header = argv[2];
    while (header.size() < 2)
        header += " ";
    int row = atoi(argv[3]);
    karno(header.substr(0, 2), row, table);

    return 0;
}
