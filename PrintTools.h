#ifndef PRINTTOOLS_H
#define PRINTTOOLS_H

#include "Def.h"
#include <iostream>
#include <vector>

namespace AsulKit {
    namespace PrintTools{
        /** 
        * @brief 打印结构体，存储内容和颜色
        */
        template<typename T>
        struct PrintStruct {
            const T content;
            ::ConsoloColor color;
            
            PrintStruct(const T& content, ::ConsoloColor color) 
                : content(content), color(color) {}
        };

        // 前向声明
        template<typename T>
        class PrintMap;

        template<typename T>
        void print(const PrintMap<T>& printMap, bool lineBreak = true);

        /**
        * @brief 打印映射类，用于构建格式化输出
        */
        template<typename T>
        class PrintMap {
        private:
            std::vector<PrintStruct<T>> m_map;
            
        public:
            // 迭代器类型定义
            using iterator = typename std::vector<PrintStruct<T>>::iterator;
            using const_iterator = typename std::vector<PrintStruct<T>>::const_iterator;
            using reverse_iterator = typename std::vector<PrintStruct<T>>::reverse_iterator;
            using const_reverse_iterator = typename std::vector<PrintStruct<T>>::const_reverse_iterator;

            // 迭代器方法
            iterator begin() noexcept { return m_map.begin(); }
            iterator end() noexcept { return m_map.end(); }
            const_iterator begin() const noexcept { return m_map.begin(); }
            const_iterator end() const noexcept { return m_map.end(); }
            const_iterator cbegin() const noexcept { return m_map.cbegin(); }
            const_iterator cend() const noexcept { return m_map.cend(); }
            reverse_iterator rbegin() noexcept { return m_map.rbegin(); }
            reverse_iterator rend() noexcept { return m_map.rend(); }
            const_reverse_iterator rbegin() const noexcept { return m_map.rbegin(); }
            const_reverse_iterator rend() const noexcept { return m_map.rend(); }
            const_reverse_iterator crbegin() const noexcept { return m_map.crbegin(); }
            const_reverse_iterator crend() const noexcept { return m_map.crend(); }

            /**
            * @brief 获取内部映射容器
            * @return 打印结构体向量的副本
            */
            std::vector<PrintStruct<T>> getMap() const { return m_map; }
            
            /**
            * @brief 添加内容到打印映射
            * @param content 要添加的内容
            * @param color 内容的颜色
            * @return 当前对象的引用，用于链式调用
            */
            PrintMap<T>& append(const T& content, ::ConsoloColor color = ::Normal) {
                m_map.emplace_back(content, color);
                return *this;
            }
            
            /**
            * @brief 添加日志级别前缀
            * @param level 日志级别
            * @return 当前对象的引用，用于链式调用
            */
            PrintMap<T>& append(::LogLevel level) {
                switch (level) {
                    case ::Success: append("[成功] ", ::Green); break;
                    case ::Info: append("[通知] ", ::DarkGray); break;
                    case ::Warn: append("[警告] ", ::Yellow); break;
                    case ::Err: append("[错误] ", ::Red); break;
                }
                return *this;
            }
            
            /**
            * @brief 添加换行符
            * @return 当前对象的引用，用于链式调用
            */
            PrintMap<T>& endl() {
                m_map.emplace_back("\n", ::Normal);
                return *this;
            }
            
            /**
            * @brief 填充方法（保留空实现以保持兼容性）
            * @return 当前对象的引用，用于链式调用
            */
            PrintMap<T>& fill() {
                return *this;
            }
            
            /**
            * @brief 添加是/否选择提示
            * @param dCase 默认选项
            * @return 当前对象的引用，用于链式调用
            */
            PrintMap<T>& askYN(::DefaultCase dCase) {
                switch (dCase) {
                    case ::Y: 
                        m_map.emplace_back("(Y/" + std::string(UNDERLINE) + "N" + std::string(RESET) + "):", ::Normal); 
                        break;
                    case ::N: 
                        m_map.emplace_back("(" + std::string(UNDERLINE) + "Y" + std::string(RESET) + "/N):", ::Normal); 
                        break;
                    case ::None: 
                        m_map.emplace_back("(Y/N)", ::Normal); 
                        break;
                }
                return *this;
            }
            
            /**
            * @brief 打印当前映射内容
            * @param lineBreak 是否在末尾添加换行
            */
            void printMap(bool lineBreak = true) const {
                print(*this, lineBreak);
            }
        };

        /**
        * @brief 打印单个内容
        * @param content 要打印的内容
        * @param color 打印颜色
        */
        template<typename T>
        void print(const T& content, ::ConsoloColor color = ::Normal) {
            switch (color) {
                case ::Red: std::cout << RED << content << RESET; break;
                case ::Blue: std::cout << BLUE << content << RESET; break;
                case ::Green: std::cout << GREEN << content << RESET; break;
                case ::Yellow: std::cout << YELLOW << content << RESET; break;
                case ::Magenta: std::cout << MAGENTA << content << RESET; break;
                case ::Cyan: std::cout << CYAN << content << RESET; break;
                case ::LightGray: std::cout << LIGHT_GRAY << content << RESET; break;
                case ::DarkGray: std::cout << DARK_GRAY << content << RESET; break;
                case ::LightRed: std::cout << LIGHT_RED << content << RESET; break;
                case ::LightGreen: std::cout << LIGHT_GREEN << content << RESET; break;
                case ::LightYellow: std::cout << LIGHT_YELLOW << content << RESET; break;
                case ::LightBlue: std::cout << LIGHT_BLUE << content << RESET; break;
                case ::LightMagenta: std::cout << LIGHT_MAGENTA << content << RESET; break;
                case ::LightCyan: std::cout << LIGHT_CYAN << content << RESET; break;
                case ::White: std::cout << WHITE << content << RESET; break;
                case ::Normal: std::cout << content; break;
            }
        }

        /**
        * @brief 打印映射内容
        * @param printMap 要打印的映射
        * @param lineBreak 是否在末尾添加换行
        */
        template<typename T>
        void print(const PrintMap<T>& printMap, bool lineBreak) {
            for (const auto& printCmd : printMap) {
                print(printCmd.content, printCmd.color);
            }
            if (lineBreak) {
                print("\n");
            }
        }

        /**
        * @brief 打印错误信息
        * @param content 错误内容
        * @param color 内容颜色
        */
        template<typename T>
        void printErr(const T& content, ::ConsoloColor color = ::Normal) {
            print("[错误] ", ::Red);
            print(content, color);
            print("\n");
        }

        /**
        * @brief 打印警告信息
        * @param content 警告内容
        * @param color 内容颜色
        */
        template<typename T>
        void printWarn(const T& content, ::ConsoloColor color = ::Normal) {
            print("[警告] ", ::Yellow);
            print(content, color);
            print("\n");
        }

        /**
        * @brief 打印通知信息
        * @param content 通知内容
        * @param color 内容颜色
        */
        template<typename T>
        void printInfo(const T& content, ::ConsoloColor color = ::Normal) {
            print("[通知] ", ::DarkGray);
            print(content, color);
            print("\n");
        }

        /**
        * @brief 打印成功信息
        * @param content 成功内容
        * @param color 内容颜色
        */
        template<typename T>
        void printSucc(const T& content, ::ConsoloColor color = ::Normal) {
            print("[成功] ", ::LightGreen);
            print(content, color);
            print("\n");
        }

    }  // namespace PrintTools
}   // namespace AsulKit

#endif // PRINTTOOLS_H