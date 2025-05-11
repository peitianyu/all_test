#ifndef __INI_PARSE_H__
#define __INI_PARSE_H__

#include<iostream>
#include<fstream>
#include<sstream>
#include<map>

class IniParse
{
public:
    struct Param
    {
        char comment;
        char separator;

        Param(char c = '#', char s = '=') : comment(c), separator(s) {}
    };

    IniParse(const std::string& filename, const Param& p = Param())
    :m_param(p), m_curr_section("")
    {
        m_ifs.open(filename);
        if(!m_ifs.is_open()) {
            std::cerr << "open ini file failed" << std::endl;
            return;
        }

        std::string line;
        while(std::getline(m_ifs, line))
        {
            Trim(line);
            if(line.empty() || line[0] == m_param.comment)
                continue;

            if(line[0] == '[') 
            {
                if(line.back() == ']')
                    m_curr_section = line.substr(1, line.length() - 2);
                else
                    std::cerr << "Invalid section format: " << line << std::endl;
            }
            else if(!m_curr_section.empty())
            {
                size_t separator_pos = line.find(m_param.separator);
                if(separator_pos != std::string::npos)
                {
                    std::string key = line.substr(0, separator_pos);
                    std::string value = line.substr(separator_pos + 1);
                    Trim(key);
                    Trim(value);
                    m_map[m_curr_section][key] = value;
                }
            }
        }
        m_ifs.close();
    }

    ~IniParse() {}

    template<typename DataType>
    bool GetValue(const std::string& section, const std::string& key, DataType& value)
    {
        auto section_it = m_map.find(section);
        if(section_it == m_map.end()) return false;

        auto& section_map = section_it->second;
        auto key_it = section_map.find(key);
        if(key_it == section_map.end()) return false;

        std::istringstream iss(key_it->second);
        if((iss >> value).fail()) return false;

        return true;
    }

private:
    void Trim(std::string& str)
    {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        str = (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
    }

    Param m_param;
    std::ifstream m_ifs;
    std::string m_curr_section;
    std::map<std::string, std::map<std::string, std::string>> m_map; // key: section, value: key-value pairs
};

#endif // __INI_PARSE_H__