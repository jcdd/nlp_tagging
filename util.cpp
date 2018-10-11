#include <algorithm>
#include "util.h"

using namespace std;

int String::Str2Int(const std::string& str) {
    int tmp = -999;
    /*
    stringstream stream(str);
    stream << tmp;
    */
    tmp = atoi(str.c_str());
    return tmp;
}

int String::SplitUtf8(const std::string& str, const unsigned char* chardel,
                      std::vector<std::string>& result, bool rm)
{
    const static uint32_t flagArray[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
    };
    int count = 0;
    size_t length = str.size();
    size_t it = 0;
    size_t ipre = 0;
    while(it < length)
    {
        uint32_t len = flagArray[static_cast<unsigned char>(str[it])];
        if(1 == len && chardel[str[it]])
        {
            std::string tstr = str.substr(ipre, it - ipre);
            if(!rm || !IsBlank(tstr))
            {
                result.push_back(tstr);
                count += tstr.size();
            }
            ipre = it + 1;
        }
        it += len;
    }
    //the last one
    if(ipre != length)
    {
        std::string tstr = str.substr(ipre);
        if(!rm || !IsBlank(tstr))
        {
            result.push_back(tstr);
            count += tstr.size();
        }
    }
    //this  count may useful
    return count;
}
int String::SplitUtf8(const std::string& str, const std::string& delimiter, std::vector<std::string>& res, bool rm)
{
    // a table which indicate that which char should be a delimiter
    unsigned char chardel[256];
    memset(chardel, 0, sizeof(chardel));
    for(size_t i = 0 ; i < delimiter.size(); ++i)
    {
        chardel[static_cast<unsigned char>(delimiter[i])] = 1;
    }
    return SplitUtf8(str, chardel, res, rm);
}
int String::SplitUtf8(const std::string& str, std::vector<std::string>& res, bool rm)
{
    const static unsigned char chardel[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    return SplitUtf8(str, chardel, res, rm);
}
//delimit first and last delimiters of string
void String::Trim(std::string& source)//const char* delims = " \t\r\n"
{
    const static char delims[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    };
    std::string::size_type end, begin;
    std::string::size_type strSize = source.size();
    if(strSize == 0)
        return ;
    int tmp_counter = strSize - 1;
    for(end = strSize - 1; tmp_counter >= 0; end--)
    {
        if(!delims[(unsigned char)source[end]])
            break;
        --tmp_counter;
    }
    for(begin = 0; begin < strSize; begin++)
    {
        if(!delims[(unsigned char)source[begin]])
            break;
    }
    if(begin >= strSize)
        begin = std::string::npos;
    if(begin == 0 && end == (strSize - 1))
    {
        //source will not be touched
        return ;
    }
    else if(begin == 0)
    {
        source.resize(end + 1);
        return ;
    }
    else if((std::string::npos != begin) && begin <= end)
    {
        source = source.substr(begin, end - begin + 1);
        return ;
    }
    else
    {
        source = "";
        return ;
    }
}

void String::Trim(std::string& source, const char* delims)
{
    std::string::size_type end = source.find_last_not_of(delims);

    std::string::size_type begin = source.find_first_not_of(delims);

    if(begin == 0 && end == (source.size() - 1))
    {
        //source will not be touched
        return;
    }
    else if(begin == 0)
    {
        source.resize(end + 1);
        return;
    }
    else if((std::string::npos != begin) && begin <= end)
    {
        source = source.substr(begin, end - begin + 1);
    }
    else
    {
        source = "";
    }
}

std::string String::Join(const std::string& str, const std::vector<std::string>& dest)
{
    std::string dstr;
    std::vector<std::string>::const_iterator begin = dest.begin();
    std::vector<std::string>::const_iterator end = dest.end();
    if (begin != end)
    {
        --end;
        for(; begin != end; ++begin)
        {
            dstr += *begin + str;
        }
        dstr += *begin;
    }
    return dstr;
}
void String::Split(const string& line, const string& delims, vector<string>& tokens, bool ignoreEmpty)
{
    tokens.clear();
    size_t n = 0, old = 0;
    while(n != std::string::npos)
    {
        n = line.find(delims, n);
        if(n != std::string::npos)
        {
            if(!ignoreEmpty || n != old)
                tokens.push_back(line.substr(old, n - old));
            n += delims.length();
            old = n;
        }
    }
    if(!ignoreEmpty || old < line.length())
    {
        tokens.push_back(line.substr(old, line.length() - old));
    }
}
void String::TokenToString(std::vector<std::string>& tokens, std::string& output, const std::string delims)
{
    output.clear();
    int n = tokens.size();
    int i = 0;
    for(; i < n - 1; ++i)
    {
        output.append(tokens[i]);
        output.append(delims);
    }
    output.append(tokens[i]);
}
/**
 * @brief caculate the Levenshtein distance of two strings
 * @param inputed two strings
 * @return the distance, in int
 */
int String::LevenshteinDistance(const std::string& left, const std::string& right)
{
    if(left.length() == 0 || right.length() == 0)
    {
        return std::max(left.length(), right.length());
    }
    int ROW_LEN = 1 + left.length();
    int COL_LEN = 1 + right.length();
    vector<uint64_t> vnMat(ROW_LEN * COL_LEN, 0);
    //initialization
    for(int i = 0; i < ROW_LEN; ++i)
    {
        vnMat[ i * COL_LEN + 0] = i;
    }
    for(int j = 0; j < COL_LEN; ++j)
    {
        vnMat[j] = j;
    }
    for(int j = 1; j < COL_LEN; ++j)
    {
        for(int i = 1; i < ROW_LEN; ++i)
        {
            if(left[i - 1] == right[j - 1])
            {
                vnMat[i * COL_LEN + j] = vnMat[(i - 1) * COL_LEN + j - 1];
            }
            else
            {
                vnMat[i * COL_LEN + j] =
                    std::min(vnMat[(i - 1) * COL_LEN + j] + 1,
                             std::min(vnMat[i * COL_LEN + j - 1] + 1,
                                      vnMat[(i - 1) * COL_LEN + j - 1] + 1));
            }
        }
    }
    int nRet = vnMat[ROW_LEN * COL_LEN - 1];
    return nRet;
}

int String::TokenizeUtf8(const std::string& str, const unsigned char* chardel,
                         std::vector<std::string>& result, bool rm)
{
    const static uint32_t flagArray[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
    };
    int count = 0;
    size_t length = str.size();
    size_t it = 0;
    size_t ipre = 0;
    while (it < length)
    {
        uint32_t len = flagArray[static_cast<unsigned char>(str[it])];
         if (1 == len && chardel[str[it]])
        {
            std::string tstr = str.substr(ipre, it - ipre);
            if (!rm || !IsBlank(tstr))
            {
                result.push_back(tstr);
                count += tstr.size();
            }
            ipre = it + 1;
        }
        it += len;
    }
    //the last one
    if (ipre != length)
    {
        std::string tstr = str.substr(ipre);
        if (!rm || !IsBlank(tstr))
        {
            result.push_back(tstr);
            count += tstr.size();
        }
    }
    //this  count may useful
    return count;
}

int String::TokenizeUtf8(const std::string& str, const std::string& delimiter, 
                         std::vector<std::string>& res, bool rm)
{
    // a table which indicate that which char should be a delimiter
    unsigned char chardel[256];
    memset(chardel, 0, sizeof(chardel));
    for (size_t i = 0 ; i < delimiter.size(); ++i)
    {
        chardel[static_cast<unsigned char>(delimiter[i])] = 1;
    }
    return TokenizeUtf8(str, chardel, res, rm);
}

int String::TokenizeUtf8(const std::string& str, std::vector<std::string>& res, bool rm)
{
    const static unsigned char chardel[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    return TokenizeUtf8(str, chardel, res, rm);
}

int String::RegularizeString(const string& str, string& rStr, vector<int>& marks)
{
//     rStr.clear();
//     marks.clear();
//     if (str.empty())
//     {
//         return 0;
//     }
// 
//     size_t originTextLength = str.size();
// 
//     // regularized text
//     rStr.reserve(originTextLength);
//     marks.reserve(originTextLength);
// 
// 	//mark every symbol, and regularize
//     const unsigned char* strStart = reinterpret_cast<const unsigned char*>(str.c_str());
// 	int mark_type = -2;
// 	int mark_len = 0;
//     int convert_type = 0;
//     const char* translatedResult = NULL;
// 	for (size_t i = 0; i< originTextLength;)
// 	{
// 		if(NULL!=get_symbol_type_utf8(str.c_str() + i, mark_type, mark_len))
// 		{
//             if (mark_len == 3 && (mark_type == 3 || mark_type == 7 || mark_type == 4)) {
//                 translatedResult = convert_multichar_symbol_digit_to_ansci(strStart + i, convert_type);
//                 if (mark_type == 7 && convert_type == 2) {
//                     rStr += tolower(*translatedResult);
//                     marks.push_back(100 + convert_type);
//                 }
//                 else if (translatedResult != NULL && convert_type != 21) {
//                     rStr += translatedResult;
//                     marks.push_back(100 + convert_type);
//                     if (convert_type == 32 || convert_type == 34 ||
//                         convert_type == 36 || convert_type == 38)
//                     {
//                         marks.push_back(100 + convert_type);
//                     }
//                 } else {
//                     marks.push_back(mark_type);
//                     rStr += str[i];
//                     for (int j = 1; j < mark_len; ++j) {
//                         rStr += str[i+j];
//                         marks.push_back(-9);
//                     }
//                 }
// 
//                 i += mark_len;
//                 continue;
//             }
// 
// 			if(mark_len == 1)
// 			{
// 				int tmp_char = tolower(str[i]);
// 				if(tmp_char != str[i])
// 				{
//                     rStr += tmp_char;
// 					mark_type = 10;//upper character
// 				} else {
//                     rStr += str[i];
//                 }
// 			} else {
//                 rStr += string(str.c_str() + i, mark_len);
//             }
// 
// 			marks.push_back(mark_type);
// 
// 			for (int j = 1; j < mark_len; j++)
// 			{
// 				marks.push_back(-9);	//sub character
// 			}
// 			i+= mark_len;
// 		}
// 		else
// 		{
//             marks.push_back(-9);	//sub character
// 			i++;
// 		}  
// 	}
// 
//     return str.size() - rStr.size();
    return 0;
}

std::string GetNormalizedText(const std::string& str)
{
//    static uint32_t stringConvertMode = HTMLESCCONVERT|T2S|SBC2DBC|UPPER2LOWER;
//    string result;
//    StringConvert(str, result, stringConvertMode);
//    String::Trim(result);
//    return result;
    return str;
}

void  String::ReplaceString(std::string &str, const std::string &old_str, const std::string &new_str) {
    if (old_str == new_str) {
        return;
    }
    string::size_type start_pos = 0;
    while (start_pos != string::npos) {
        start_pos = str.find(old_str, start_pos);
        if (start_pos != string::npos) {
            str.replace(start_pos, old_str.size(), new_str);
        }
        if (start_pos != string::npos) {
            start_pos += old_str.size();
        }
    }
}

std::string String::EraseString(const std::string &str, const std::string &start_str, const std::string &end_str) {
    std::string ret = str;
    string::size_type pos1 = 0;
    string::size_type pos2 = 0;
    while (pos1 != string::npos && pos2 != string::npos) {
        pos1 = ret.find(start_str);
        pos2 = ret.find(end_str, pos1 + 1);
        if (pos1 != string::npos && pos2 != string::npos && pos2 > pos1) {
            ret = ret.erase(pos1, pos2 - pos1 + end_str.size());
        }
        else {
            return ret;
        }
    }
    return ret; 
}

int String::FindStringCount(const std::string &str, const std::string find_str) {
    string::size_type  pos = 0;
    int count = 0;
    while(pos != string::npos) {
       pos = str.find(find_str, pos + 1);
       if (pos != string::npos) {
           count ++;
       }
    } 
    return count;
}

int String::SubString(const std::string &str, std::string start_str,  std::string end_str, std::vector<std::string> &ret_str_vec) {
    int start_size = start_str.size();
    int end_size = end_str.size();
    string::size_type start_pos = 0;
    string::size_type end_pos = 0;
    std::vector<std::string> ret_str;
    while (start_pos != string::npos && end_pos != string::npos) {
        start_pos = str.find(start_str, end_pos);
        if (start_pos == string::npos) {
            break;
        }
        end_pos = str.find(end_str, start_pos+1);
        if (end_pos == string::npos) {
            break;
        }
        int diff_pos = end_pos - start_pos;
        if (diff_pos > 0) {
            string sub_str = str.substr(start_pos + start_size, end_pos - start_pos - start_size);
            ret_str_vec.push_back(sub_str);
        }
    }
    return 0;
}


