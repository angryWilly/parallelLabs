#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <format>
#include <numeric>
#include <omp.h>

const int AlphabetCount = 26;

void readFiles(const int fileCount, const int threads, std::vector<int>& freqOut)
{
    auto threadID = omp_get_thread_num();
    std::vector localFreq(AlphabetCount, 0);

    for (int i = threadID; i < fileCount; i += threads)
    {
        std::ifstream fin("text_files_5.2/file" + std::to_string(i) + ".txt");
        if (fin.is_open())
        {
            char symbol;
            while (fin.get(symbol))
            {
                if (isalpha(symbol))
                {
                    localFreq[tolower(symbol) - 'a']++;
                }
            }
        }
        else {
            std::cout << std::format("Can't open file {} for thread {}\n", i, threadID);
        }
    }

#pragma omp critical
    {
        for (int i = 0; i < AlphabetCount; i++)
        {
            freqOut[i] += localFreq[i];
        }
    }
}

void PrintMostCommon(const std::vector<std::pair<int, char>>& sortedFreq)
{
    std::cout << "5 most common letters: ";
    for (int i = AlphabetCount - 1; i > 20; i--)
    {
        std::cout << sortedFreq[i].second << " ";
    }
}

void PrintProbability(const char symbol, std::vector<int>& freq)
{
    auto total = std::accumulate(freq.begin(), freq.end(), 0);
    auto p = static_cast<double>(freq[tolower(symbol) - 'a']) / total;
    std::cout << "Probability of letter " << symbol << ": " << p;
}

void PrintLeastCommon(const std::vector<std::pair<int, char>>& sortedFreq)
{
    std::cout << "3 least common letters: ";
    for (int i = 0; i < 3; i++)
    {
        std::cout << sortedFreq[i].second << " ";
    }
}

std::vector<std::pair<int, char>> SortFrequencies(const std::vector<int>& freq)
{
    std::vector<std::pair<int, char>> sortedFreq(AlphabetCount);
    for (int i = 0; i < AlphabetCount; i++)
    {
        sortedFreq[i] = std::make_pair(freq[i], 'a' + i);
    }
    std::ranges::sort(sortedFreq);

    return sortedFreq;
}

int main(int argc, char** argv)
{
    const int threads = 6;
    const int filesPerThread = 2;
    const auto fileCount = threads * filesPerThread;
    omp_set_num_threads(threads);

    std::vector freq(AlphabetCount, 0);

#pragma omp parallel
    {
        readFiles(fileCount, threads, freq);
    }

    auto sortedFreq = SortFrequencies(freq);

    std::cout << "Enter 1 : print 5 most common\n";
    std::cout << "Enter 2 : print probability\n";
    std::cout << "Enter 3 : print 3 least common\n";
    std::cout << "Enter 4 : exit\n";

    int input = 0;
    while (input != 4)
    {
        std::cin >> input;

        switch (input)
        {
        case 1:
            PrintMostCommon(sortedFreq);
            break;
        case 2:
            char symbol;
            std::cout << "--Enter symbol : ";
            std::cin >> symbol;
            if (isalpha(symbol))
            {
                PrintProbability(symbol, freq);
            }
            break;
        case 3:
            PrintLeastCommon(sortedFreq);
            break;
        case 4:
            std::cout << "Exiting...";
            break;
        default:
            break;
        }

        std::cout << std::endl;
    }

    return 0;
}
