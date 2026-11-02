#ifndef MYPROJECT_STRINGUTIL_H
#define MYPROJECT_STRINGUTIL_H

class StringUtil {
public:
    // 去掉左右两边的空格
    static void trim(std::string& s) {
        const auto start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            s.clear();
            return;
        }

        const auto end = s.find_last_not_of(" \t\r\n");
        s = s.substr(start, end - start + 1);
    }
};

#endif //MYPROJECT_STRINGUTIL_H