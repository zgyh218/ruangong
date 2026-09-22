#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <iomanip>
#include <windows.h>

// 高精度计时，返回毫秒数
double getTime()
{
    LARGE_INTEGER freq, cnt;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cnt);
    return (double)cnt.QuadPart / freq.QuadPart * 1000;
}

bool isChinese(wchar_t ch)
{
    if (ch >= 0x4E00 && ch <= 0x9FA5)
        return true;
    return false;
}

std::wstring GBKToWstring(const std::string& gbkStr)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, &wstr[0], len);
    return wstr;
}

std::wstring readFileOnlyChinese(const std::string& filePath)
{
    std::ifstream fin(filePath, std::ios::binary);
    if (!fin.is_open())
    {
        std::cerr << "无法打开文件：" << filePath << std::endl;
        return L"";
    }
    std::string buffer((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();
    std::wstring wbuffer = GBKToWstring(buffer);
    std::wstring content;
    for (wchar_t ch : wbuffer)
    {
        if (isChinese(ch))
            content.push_back(ch);
    }
    return content;
}

std::set<std::wstring> get2GramSet(const std::wstring& s)
{
    std::set<std::wstring> st;
    for (int i = 0; i + 1 < s.size(); i++)
    {
        std::wstring gram;
        gram += s[i];
        gram += s[i + 1];
        st.insert(gram);
    }
    return st;
}

std::vector<std::wstring> get2GramList(const std::wstring& s)
{
    std::vector<std::wstring> vec;
    for (int i = 0; i + 1 < s.size(); i++)
    {
        std::wstring gram;
        gram += s[i];
        gram += s[i + 1];
        vec.push_back(gram);
    }
    return vec;
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "参数错误！用法：main.exe 原文路径 抄袭版路径 输出答案路径" << std::endl;
        return 1;
    }
    std::string origPath = argv[1];
    std::string copyPath = argv[2];
    std::string outPath = argv[3];

    // 计时1：文件读取
    double t1 = getTime();
    std::wstring origText = readFileOnlyChinese(origPath);
    std::wstring copyText = readFileOnlyChinese(copyPath);
    double t2 = getTime();
    std::cout << "1. 文件读取总耗时：" << t2 - t1 << " ms" << std::endl;

    // 计时2：2-Gram提取
    double t3 = getTime();
    auto origGramSet = get2GramSet(origText);
    auto copyGramList = get2GramList(copyText);
    double t4 = getTime();
    std::cout << "2. 2-Gram分词总耗时：" << t4 - t3 << " ms" << std::endl;

    if (copyGramList.empty())
    {
        std::ofstream fout(outPath);
        fout << std::fixed << std::setprecision(2) << 0.00 << std::endl;
        fout.close();
        return 0;
    }

    // 计时3：相似度计算
    double t5 = getTime();
    int matchCnt = 0;
    int totalCnt = copyGramList.size();
    for (const auto& gram : copyGramList)
    {
        if (origGramSet.count(gram))
            matchCnt++;
    }
    double repeatRate = static_cast<double>(matchCnt) / totalCnt;
    double t6 = getTime();
    std::cout << "3. 相似度计算耗时：" << t6 - t5 << " ms" << std::endl;
    std::cout << "最终查重率：" << std::fixed << std::setprecision(2) << repeatRate << std::endl;

    std::ofstream fout(outPath);
    fout << std::fixed << std::setprecision(2) << repeatRate << std::endl;
    fout.close();
    return 0;
}
