#ifndef ASULFORMATSTRING_H
#define ASULFORMATSTRING_H

#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class AsulFormatString {
public:
    using AdapterMap = std::map<std::string, std::string>;
    using VariantType = std::variant<int, double, std::string, bool>;
    AsulFormatString() = default;

    void installFormatAdapter(const AdapterMap& mp) {
        formatAdapter.insert(mp.begin(), mp.end());
    }
    void clearFormatAdapter() { formatAdapter.clear(); }

    void installLabelAdapter(const AdapterMap& mp) {
        labelAdapter.insert(mp.begin(), mp.end());
    }
    void clearLabelAdapter() { labelAdapter.clear(); }

    void installColorFormatAdapter() {
        installFormatAdapter({
            {"RED", "\033[31m{}\033[0m"},
            {"GREEN", "\033[32m{}\033[0m"},
            {"YELLOW", "\033[33m{}\033[0m"},
            {"BLUE", "\033[34m{}\033[0m"},
            {"MAGENTA", "\033[35m{}\033[0m"},
            {"CYAN", "\033[36m{}\033[0m"},
            {"LIGHT_GRAY", "\033[37m{}\033[0m"},
            {"DARK_GRAY", "\033[90m{}\033[0m"},
            {"LIGHT_RED", "\033[91m{}\033[0m"},
            {"LIGHT_GREEN", "\033[92m{}\033[0m"},
            {"LIGHT_YELLOW", "\033[93m{}\033[0m"},
            {"LIGHT_BLUE", "\033[94m{}\033[0m"},
            {"LIGHT_MAGENTA", "\033[95m{}\033[0m"},
            {"LIGHT_CYAN", "\033[96m{}\033[0m"},
            {"WHITE", "\033[97m{}\033[0m"},
            {"UNDERLINE", "\033[4m{}\033[0m"}
        });
    }

    void installLogLabelAdapter() {
        installLabelAdapter({
            {"SUCCESS", f("{GREEN}", "[成功]")},
            {"INFO", f("{LIGHT_BLUE}", "[通知]")},
            {"WARN", f("{YELLOW}", "[警告]")},
            {"ERROR", f("{RED}", "[错误]")}
        });
    }

    void installAskLabelAdapter(){
        installLabelAdapter({
            {"ASK_Y",f("({UNDERLINE}/N)","Y")},
            {"ASK_N",f("(Y/{UNDERLINE})","N")}
        });
    }

    template <typename... Args>
    std::string f(std::string fmt, const Args&... args) {
        std::vector<VariantType> argsVec;
        argvs_helper(argsVec, args...);

        if (!hasValidParentheses(fmt)) throw std::invalid_argument("Mismatched parentheses in format string");
        if (!hasValidCurlyBraces(fmt)) throw std::invalid_argument("Mismatched curly braces in format string");

        std::string tempFmt = processLabelAdapter(fmt);
        std::string processedFmt = processAdapter(tempFmt);
        std::string result;
        size_t argIndex = 0, i = 0, len = processedFmt.length();
        while (i < len) {
            if (processedFmt[i] == '{') {
                size_t j = processedFmt.find('}', i);
                if (j == std::string::npos) throw std::invalid_argument("Unclosed curly brace in format string");
                std::string placeholder = processedFmt.substr(i + 1, j - i - 1);
                if (placeholder.empty()) {
                    if (argIndex < argsVec.size()) {
                        result += variantToString(argsVec[argIndex]);
                        argIndex++;
                    } else {
                        result += "{}";
                    }
                } else {
                    result += processedFmt.substr(i, j - i + 1);
                }
                i = j + 1;
            } else {
                result += processedFmt[i];
                i++;
            }
        }
        return result;
    }

private:
    AdapterMap formatAdapter;
    AdapterMap labelAdapter;

    void argvs_helper(std::vector<VariantType>& vec) { /* no args */ }

    template <typename T>
    void argvs_helper(std::vector<VariantType>& vec, const T& first) {
        vec.push_back(first);
    }
    template <typename T, typename... Args>
    void argvs_helper(std::vector<VariantType>& vec, const T& first, const Args&... args) {
        vec.push_back(first);
        argvs_helper(vec, args...);
    }

    bool hasValidCurlyBraces(const std::string& s) {
        std::stack<char> p;
        for (char c : s) {
            if (c == '{') p.push(c);
            else if (c == '}') {
                if (p.empty() || p.top() != '{') return false;
                p.pop();
            }
        }
        return p.empty();
    }

    bool hasValidParentheses(const std::string& s) {
        std::stack<char> p;
        for (char c : s) {
            if (c == '(') p.push(c);
            else if (c == ')') {
                if (p.empty() || p.top() != '(') return false;
                p.pop();
            }
        }
        return p.empty();
    }

    std::string processAdapter(const std::string& fmt) {
        std::string processed = fmt;
        std::regex pattern(R"(\{(\w+)\})");
        std::smatch match;
        std::string temp = processed;
        
        processed.clear();
        while (std::regex_search(temp, match, pattern)) {
            processed += temp.substr(0, match.position());
            std::string placeholder = match[1];
            if (formatAdapter.find(placeholder) != formatAdapter.end()) {
                processed += formatAdapter[placeholder];
            } else {
                processed += match[0];
            }
            
            temp = match.suffix();
        }
        processed += temp;
        
        return processed;
    }

    std::string processLabelAdapter(const std::string& fmt) {
        std::string processed = fmt;
        std::regex pattern(R"(\((\w+)\))");
        std::smatch match;
        std::string temp = processed;
        
        processed.clear();
        while (std::regex_search(temp, match, pattern)) {
            processed += temp.substr(0, match.position());
            std::string placeholder = match[1];
            if (labelAdapter.find(placeholder) != labelAdapter.end()) {
                processed += labelAdapter[placeholder];
            } else {
                processed += match[0];
            }
            temp = match.suffix();
        }
        processed += temp;
        
        return processed;
    }

    std::string variantToString(const VariantType& v) {
        std::ostringstream oss;
        if (std::holds_alternative<int>(v)) oss << std::get<int>(v);
        else if (std::holds_alternative<double>(v)) oss << std::get<double>(v);
        else if (std::holds_alternative<std::string>(v)) oss << std::get<std::string>(v);
        else if (std::holds_alternative<bool>(v)) oss << (std::get<bool>(v) ? "true" : "false");
        return oss.str();
    }
};

inline AsulFormatString &asul_formatter() {
  static AsulFormatString inst;
  return inst;
}

template <typename... Args>
inline std::string f(const std::string &fmt, const Args &...args) {
  return asul_formatter().f(fmt, args...);
}

#endif // ASULFORMATSTRING_H
    