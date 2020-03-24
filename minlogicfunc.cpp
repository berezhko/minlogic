#include <cstdio>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <cassert>

using std::vector;
using std::string;
using std::set;
using std::map;

namespace NLogic {

class TLogicFunction;
unsigned long long calcLogic(TLogicFunction);
typedef vector<int> minterm_t;

// Получить бит в позиции числа
int getBit(int bitpos, int num)
{
    int zerofirstbits = (num >> bitpos) << bitpos;
    return (num - zerofirstbits) >> (bitpos - 1);
}

// Конвертировать последовательность битов и uint
// 1111 -> 15
int seqbittoint(const minterm_t &seqbit)
{
    int result = 0;
    for (int i = 0; i < seqbit.size(); i++) {
        if (seqbit[i])
            result += (1 << (seqbit.size()-i-1));
    }
    return result;
}

// Конвертировать uint в последовательность бит
// 15 -> 1111
minterm_t inttoseqbin(int number, int dem)
{
    minterm_t result;

    for (int i = dem; i >= 1 ; i--){
        result.push_back(getBit(i, number));
    }
    return result;
}

// Подсчет количества бит в числе
int bitCount(int number)
{
    int result = 0;
    while (number >>= 1)
        result++;

    return result;
}

class TLogicFunction
{
public:
    // {1,0,1,1,1,0,1,0}, 3
    TLogicFunction(minterm_t logic_func_result, size_t df):
        dem_func(df)
    {
        for (int i = 0; i < logic_func_result.size(); i++) {
            if (logic_func_result[i] == 1) {
                minterm_t res_line;
                for (int l = df; l >= 1; l--) {
                    // Num to seq. bit Example 6 -> {1,1,0}
                    int bit = getBit(l, i);
                    res_line.push_back(bit);
                }
                logic_table.push_back(res_line);
            }
        }
    }

    // {{000}, {010}, {011}, {100}, {110}}
    TLogicFunction(vector<minterm_t> lt):
        logic_table(lt.begin(), lt.end())
    {
        dem_func = lt[0].size();
    }

    // {{000}, {010}, {011}, {100}, {110}}
    TLogicFunction(set<minterm_t> lt):
        logic_table(lt.begin(), lt.end())
    {
        dem_func = (lt.begin())->size();
    }

    int operator()(minterm_t logic_value)
    {
        bool result_func = false;
        for (const auto & minterm: logic_table) {
            bool res_compare_minterms = true;
            for (int i = 0; i < minterm.size(); i++) {
                if (minterm[i] != X) {
                    res_compare_minterms &= (minterm[i] == logic_value[i]);
                }
            }
            result_func |= res_compare_minterms;
        }
        return static_cast<int>(result_func);
    }

    int operator()(int input_int)
    {
        return operator()(inttoseqbin(input_int, logic_table[0].size()));
    }

    set<minterm_t> getLogicTable()
    {
        return {logic_table.begin(), logic_table.end()};
    }

    void minimize()
    {
        unsigned long long res = calcLogic(*this);
        for (minterm_t &minterm: logic_table) {
            for (int &element: minterm) {
                int old_value = element;
                element = X;
                if (res != calcLogic(*this))
                    element = old_value;
            }
        }
    }

    int getDemFunc()
    {
        return dem_func;
    }

private:
    vector<minterm_t> logic_table;
    size_t dem_func;
    static const int X = 'X'-0x30;
};

class TLogicSet
{
public:
    int addMinterm(minterm_t term)
    {
        int result = 0;
        for (const auto &minterm: rollingMinterm(term)) {
            logic_table.insert(minterm);
            result += 1;
        }
        return result;
    }

    int delMinterm(minterm_t term)
    {
        int countdelete;
        for (const auto &minterm: rollingMinterm(term)) {
            countdelete += logic_table.erase(minterm);
        }
        return countdelete;
    }

    bool containsMinterm(minterm_t term)
    {
        bool result = true;
        for (const auto &minterm: rollingMinterm(term)) {
            auto rescompare = logic_table.find(minterm);
            if (rescompare != logic_table.end())
                result &= true;
            else
                result &= false;
        }
        return result;
    }

    set<minterm_t> getLogicTable()
    {
        return logic_table;
    }

private:
    set<minterm_t> rollingMinterm(minterm_t minterm)
    {
        set<minterm_t> result;
        for (auto &el: minterm) {
            if (el == X) {
                set<minterm_t> halfres;
                el = 0;
                halfres = rollingMinterm(minterm);
                result.insert(halfres.begin(), halfres.end());
                el = 1;
                halfres = rollingMinterm(minterm);
                result.insert(halfres.begin(), halfres.end());
            }
        }
        result.insert(minterm);
        return result;
    }

private:
    set<minterm_t> logic_table;
    static const int X = 'X'-0x30;
};

unsigned long long calcLogic(TLogicFunction f)
{
    unsigned long long result = 0;
    int size = 1 << f.getDemFunc();
    for (int i = 0; i < size; i++) {
        result += (f(i) << (size-1 - i));
    }
    return result;
}

int countLogicPin(const set<minterm_t> &logic_table)
{
    int result = 0;
    for (const auto & minterm: logic_table) {
        for (const auto & element: minterm) {
            if (element == 0 || element == 1)
                result += 1;
        }
    }
    return result;
}

TLogicFunction minimize(minterm_t term, int dem)
{
    int demDelta = (1 << dem) - term.size();
    assert(demDelta >= 0);

    TLogicFunction result(term, dem);
    result.minimize();

    if (demDelta != 0) {
        int count_logic_pin = countLogicPin(result.getLogicTable());
        for (int i = 1; i < (1 << demDelta); i++) {
            minterm_t minterm = term;
            for (const auto & element: inttoseqbin(i, demDelta)) {
                minterm.push_back(element);
            }

            TLogicFunction f(minterm, dem);
            f.minimize();
            if (count_logic_pin > countLogicPin(f.getLogicTable())) {
                count_logic_pin = countLogicPin(f.getLogicTable());
                result = f;
            }
        }
    }

    return result;
}

TLogicFunction cutduplicate(TLogicFunction f)
{
    set<minterm_t> good_minterms;
    set<minterm_t> bad_minterms;
    for (const auto & checked: f.getLogicTable()) {
        TLogicSet s;
        for (const auto & other: f.getLogicTable()) {
            if (other != checked && bad_minterms.find(other) == bad_minterms.end()) {
                s.addMinterm(other);
            }
        }
        if (s.containsMinterm(checked) == false) {
            good_minterms.insert(checked);
        } else {
            bad_minterms.insert(checked);
        }
    }
    return {good_minterms};
}

} // NLogic


using NLogic::minterm_t;
using NLogic::TLogicFunction;
using NLogic::TLogicSet;


namespace NInOutput {

int readTrueTable(char *filename, vector<minterm_t> &table)
{
    FILE *inputfile = fopen(filename, "r");
    if (inputfile == NULL) {
        return 1;
    }

    int c;
    minterm_t line;
    while ((c = fgetc(inputfile)) != EOF) {
        switch(c) {
        case '\n':
            table.push_back(line);
            line.clear();
            break;
        case '1':
            line.push_back(1);
            break;
        case '0':
            line.push_back(0);
            break;
        }
    }
    fclose(inputfile);
    return 0;
}

void printMinterm(const minterm_t &minterm)
{
    printf(" {");
    for (const auto &element: minterm) {
        printf("%c^", element+0x30);
    }
    printf("\b}");
}

void printFormatResult(TLogicFunction f, int dem, int pos_not_main)
{
    printf("%d/%d\t", (int)f.getLogicTable().size(), NLogic::countLogicPin(f.getLogicTable()));
    for (int i = 0; i < (1 << dem); i++) {
        if (i == pos_not_main) {
            printf(" ");
        }
        printf("%d", f(i));
    }
    for (const auto &minterm: f.getLogicTable())
        printMinterm(minterm);
    printf("\n");
}

void printCountMinterms(map<minterm_t, int> elements)
{
    int count = 0;
    printf("\n");
    for (const auto & element: elements) {
        printMinterm(element.first);
        printf(" %d ", element.second);
        if (++count % 5 == 0)
            printf("\n");
    }
    printf("\n");
}

} // NInOutput


vector<minterm_t> transperent(const vector<minterm_t> & table)
{
    vector<minterm_t> result;
    minterm_t row;
    for (int i = 0; i < table[0].size(); i++) {
        for (const auto & minterm: table) {
            row.push_back(minterm[i]);
        }
        result.push_back(row);
        row.clear();
    }
    return result;
}

// Главная функция
int main(int argc, char **argv)
{
    using NLogic::cutduplicate;
    using NLogic::minimize;

    if (argc < 2) {
        return 1;
    }

    vector<minterm_t> table;
    NInOutput::readTrueTable(argv[1], table);
    map<minterm_t, int> all_elements;
    for(const auto & minterm: transperent(table)) {
        int dem = NLogic::bitCount(minterm.size()-1)+1;
        TLogicFunction f = cutduplicate(minimize(minterm, dem));
        NInOutput::printFormatResult(f, dem, minterm.size());

        for (const auto &term: f.getLogicTable())
            all_elements[term] += 1;
    }

    if (argc > 2) {
        string param = argv[2];
        if (param == "-v" || param == "--verbose")
            NInOutput::printCountMinterms(all_elements);
    }
    return 0;
}
