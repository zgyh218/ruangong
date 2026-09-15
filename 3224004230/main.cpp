#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <iomanip>
#include <windows.h>

// 判断是否为汉字 Unicode范围 0x4E00 ~ 0x9FA5
bool isChinese(wchar_t ch)
{
    if (ch >= 0x4E00 && ch <= 0x9FA5)
        return true;
    return false;
}

// GBK字节字符串转 wstring
std::wstring GBKToWstring(const std::string& gbkStr)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, &wstr[0], len);
    return wstr;
}

// 读取文件（原始字节GBK），只提取汉字，返回宽字符串
std::wstring readFileOnlyChinese(const std::string& filePath)
{
    std::ifstream fin(filePath, std::ios::binary);
    if (!fin.is_open())
    {
        std::cerr << "无法打开文件：" << filePath << std::endl;
        return L"";
    }
    // 一次性读取全部字节
    std::string buffer((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();

    // GBK字节转宽字符
    std::wstring wbuffer = GBKToWstring(buffer);

    std::wstring content;
    for (wchar_t ch : wbuffer)
    {
        if (isChinese(ch))
        {
            content.push_back(ch);
        }
    }
    return content;
}

// 生成原文二元词组集合 set
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

// 生成抄袭文本二元词组列表 vector
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
    // 判断命令行参数数量，必须3个参数
    if (argc != 4)
    {
        std::cerr << "参数错误！用法：main.exe 原文路径 抄袭版路径 输出答案路径" << std::endl;
        return 1;
    }
    std::string origPath = argv[1];
    std::string copyPath = argv[2];
    std::string outPath = argv[3];

    // 读取原文、抄袭文本，只保留汉字
    std::wstring origText = readFileOnlyChinese(origPath);
    std::wstring copyText = readFileOnlyChinese(copyPath);

    // =========调试打印，看读到的中文内容，测试成功后可以删掉这两行=====
    std::wcout << L"origText：" << origText << std::endl;
    std::wcout << L"copyText：" << copyText << std::endl;
    // ==================================================================

    // 原文生成二元词组集合
    auto origGramSet = get2GramSet(origText);
    // 抄袭文本生成词组列表
    auto copyGramList = get2GramList(copyText);

    if (copyGramList.empty())
    {
        std::ofstream fout(outPath);
        fout << std::fixed << std::setprecision(2) << 0.00 << std::endl;
        fout.close();
        return 0;
    }
    // 统计匹配词组数量
    int matchCnt = 0;
    int totalCnt = copyGramList.size();
    for (const auto& gram : copyGramList)
    {
        if (origGramSet.count(gram))
        {
            matchCnt++;
        }
    }
    // 计算重复率
    double repeatRate = static_cast<double>(matchCnt) / totalCnt;
    // 写入输出文件，保留2位小数
    std::ofstream fout(outPath);
    fout << std::fixed << std::setprecision(2) << repeatRate << std::endl;
    fout.close();
    return 0;
}
