#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <sstream>
//#include <boost/regex.hpp>

class String
{
public:
    static void Trim(std::string& source);//const char* delims = " \t\r\n"
    static void Trim(std::string& source, const char* delims);
    static inline std::string TrimCopy(const std::string& str);
    static inline std::string TrimCopy(const std::string& str, const char* delims);
    static inline void ToLower(std::string& source);
    static inline void ToUpper(std::string& source);
    static inline bool StartsWith(const std::string& str, const std::string& prefix);
    static inline bool EndsWith(const std::string& str, const std::string& suffix);
    static  int Str2Int(const std::string& str);
    static inline bool IsAsciiPunctuation(char str);
    static inline bool IsPrintableAscii(char str);
    static inline bool IsPrintableAscii(const std::string& str);
    static inline bool IsPrintableAscii(const char* str, const char* end);
    static inline bool IsBlank(const std::string& str);
    static inline bool IsNumber(const std::string& str);
    static inline bool IsNumberExt(const std::string& str);

    static std::string Join(const std::string& str, const std::vector<std::string>& dest);
    template<typename T>
    static std::string Join(const std::string& str, const std::vector<T>& dest);

    static void Split(const std::string& line, const std::string& delims,
                      std::vector<std::string>& tokens, bool ignoreEmpty = true);
    static int SplitUtf8(const std::string& str, const std::string& delimiter,
                         std::vector<std::string>& result, bool removeBlank = true);
    static int SplitUtf8(const std::string& str, std::vector<std::string>& res, bool rm = true);
    static int SplitUtf8(const std::string& str, const unsigned char* chardel_Table,
                         std::vector<std::string>& res, bool rm = true);

    template<typename T>
    static inline std::string ToString(const T &x);

    template<typename T>
    static inline  void ToString(const std::vector<T> &x, std::vector<std::string>& result);

    template<typename T>
    static inline T FromString(const std::string &str);

    template<typename T>
    static inline bool FromString(const std::string &str, T &value);

    template<typename T>
    static inline bool FromString(const std::vector<std::string> &str_vec, std::vector<T> &value);

    static void TokenToString(std::vector<std::string>& tokens,
                              std::string& output, const std::string delims = " ");
    static int LevenshteinDistance(const std::string & left, const std::string & right);
    static int  TokenizeUtf8(const std::string& str, 
                             const std::string& delimiter,
                             std::vector<std::string>& result, bool removeBlank = true);
    static int TokenizeUtf8(const std::string& str, std::vector<std::string>& res, bool rm = true);
    static int TokenizeUtf8(const std::string& str, const unsigned char* chardel_Table,
                            std::vector<std::string>& res, bool rm = true);
    static int RegularizeString(const std::string& str, std::string& rStr, std::vector<int>& marks);

    static void ReplaceString(std::string &str, const std::string &old_str, const std::string &new_str);
    static std::string EraseString(const std::string &str, const std::string &start_str, const std::string &end_str);
    static int FindStringCount(const std::string &str, const std::string find_str);
    
    static int SubString(const std::string &str, std::string start_str, std::string end_str, std::vector<std::string> &ret_str_vec);
};

inline bool IsIpAddressForHost(const std::string& host)
{
    /*
    static boost::regex ip_regex("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}(:\\d{1,10})?");
    return boost::regex_match(host, ip_regex);
    */
    return true;
}

inline std::string String::TrimCopy(const std::string& str)
{
    std::string result(str);
    String::Trim(result);
    return result;
}
inline std::string String::TrimCopy(const std::string& str, const char* delims)
{
    std::string result(str);
    String::Trim(result, delims);
    return result;
}

inline void String::ToLower(std::string& source)
{
    for(unsigned i = 0; i < source.size(); ++i)
    {
        switch(source[i])
        {
        case 'A'...'Z':
            source[i] = source[i] + 'a' - 'A';
            break;
        }
    }
}
inline void String::ToUpper(std::string& source)
{
    for(unsigned i = 0; i < source.size(); ++i)
    {
        switch(source[i])
        {
        case 'a'...'z':
            source[i] = source[i] + 'A' - 'a';
            break;
        }
    }
}
inline bool String::StartsWith(const std::string& str, const std::string& prefix)
{
    return 0 == str.compare(0, prefix.size(), prefix);
}
inline bool String::IsAsciiPunctuation(char str)
{
    return ((str >= 33 && str <= 47) || (str >= 58 && str <= 64)
            || (str >= 91 && str <= 96) || (str >= 123 && str <= 126));
}
inline bool String::IsPrintableAscii(char str)
{
    return (str >= 33 && str <= 126);
}
inline bool String::IsPrintableAscii(const std::string& str)
{
    for(size_t i = 0; i < str.size(); ++i)
    {
        if(!IsPrintableAscii(str[i]))
        {
            return false;
        }
    }
    return true;
}
inline bool String::IsPrintableAscii(const char* str, const char* end)
{
    const char* p = str;
    for(; p != end; ++p)
    {
        if(!IsPrintableAscii(*p))
        {
            return false;
        }
    }
    return true;
}

inline bool String::IsBlank(const std::string& str)
{
    return str.empty() || str.find_first_not_of("\t\n \v") == std::string::npos;
}

inline bool String::IsNumber(const std::string& str)
{
    std::string::const_iterator it = str.begin();
    for(; it != str.end() && isdigit(*it); ++it)
        ;
    return it == str.end() && !str.empty();
}
inline bool String::IsNumberExt(const std::string& str)
{
    /*
    static boost::regex number("\\d*(\\.\\d*)?([eE]\\d*)?");
    return boost::regex_match(str, number);
    */
    return true;
}

bool String::EndsWith(const std::string &str, const std::string &suffix)
{
    size_t s1 = str.size();
    size_t s2 = suffix.size();
    return (s1 >= s2) && (str.compare(s1 - s2, s2, suffix) == 0);
}
template<typename T>
inline T String::FromString(const std::string& str)
{
    T value = T();
    FromString(str, value);
    return value;
}
template<typename T>
inline bool String::FromString(const std::string &str, T &value)
{
    std::stringstream iss(str);
    iss >> value;
    return iss.eof();
}

template<typename T>
inline bool String::FromString(const std::vector<std::string> &str_vec, std::vector<T> &value)
{
    value.clear();
    value.reserve(str_vec.size());
    std::vector<std::string>::const_iterator it = str_vec.begin();
    for(; it != str_vec.end(); ++it)
    {
        value.push_back(FromString<T>(*it));
    }
    return value.size();
}

template<typename T>
static inline void ToString(const std::vector<T> &x, std::vector<std::string>& result)
{
    result.clear();
    result.reserve(x.size());
    typename std::vector<T>::const_iterator it = x.begin();
    for(it; it != x.end(); ++it)
    {
        result.push_back(ToString<T>(*it));
    }
}
template<>
inline bool String::FromString<bool>(const std::string &str, bool &value)
{
    std::stringstream iss(str);
    if(str == "true" || str == "1")
    {
        value = true;
    }
    else if(str == "false" || str == "0")
    {
        value = false;
    }
    else
    {
        return false;
    }
    return iss.eof();
}
template<typename T>
inline std::string String::ToString(const T &x) {
    std::stringstream oss;
    oss << x;
    return oss.str();
}
template<>
inline std::string String::ToString<int8_t>(const int8_t &x) {
    char buf[8] = {0,};
    snprintf(buf, sizeof(buf), "%d", x);
    std::string res(buf);
    return res;
}

template<>
inline std::string String::ToString<uint8_t>(const uint8_t &x) {
    char buf[8] = {0,};
    snprintf(buf, sizeof(buf), "%u", x);
    std::string res(buf);
    return res;
}

template<typename T>
std::string String::Join(const std::string& str, const std::vector<T>& dest)
{
    std::string dstr;
    typename std::vector<T>::const_iterator begin = dest.begin();
    typename std::vector<T>::const_iterator end = dest.end();
    if (begin != end)
    {
        --end;
        for(; begin != end; ++begin)
        {
            dstr += ToString(*begin) + str;
        }
        dstr += ToString(*begin);
    }
    return dstr;
}

std::string GetNormalizedText(const std::string& str);

inline std::string TrimAndBackup(const std::string& str)
{
    std::string result = str;
    String::Trim(result);
    return result;
}

inline std::string TrimAndBackup(const std::string& str, const char* delims)
{
    std::string result = str;
    String::Trim(result, delims);
    return result;
}

inline bool IsPrintableAscii(char str)
{
    return (str >= 33 && str <= 126);
}

inline bool IsPrintableAscii(const std::string& str)
{
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (!IsPrintableAscii(str[i]))
        {
            return false;
        }
    }
    return true;
}

inline bool IsPrintableAscii(const char* str, const char* end)
{
    const char* p = str;
    for (;p != end; ++p)
    {
        if (!IsPrintableAscii(*p))
        {
            return false;
        }
    }
    return true;
}

inline bool IsNumberExt(const std::string& str)
{
    /*
    static boost::regex number("\\d*(\\.\\d*)?([eE]\\d*)?");
    return boost::regex_match(str, number);
    */
    return true;
}

#endif
