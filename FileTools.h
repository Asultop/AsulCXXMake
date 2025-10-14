#ifndef FILETOOLS_H
#define FILETOOLS_H

#include "Def.h"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <filesystem>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif
namespace AsulKit {
    namespace FileTools{
        /**
        * @brief 判断文件是否存在
        * @param name 文件路径
        * @return true 文件存在
        * @return false 文件不存在
        */
        bool fileExist(const char* name) {
            return !access(name, F_OK);
        }
        /**
        * @brief 判断文件夹是否存在
        * @param path 文件夹路径
        * @return true 文件夹存在
        * @return false 文件夹不存在
        */
        bool folderExist(const std::string& path) {
            return std::filesystem::is_directory(std::filesystem::path(path));
        }
        /**
        * @brief 获取文件修改时间
        * @param filePath 文件路径
        * @return std::pair<bool,std::string> 第一个元素为是否成功，第二个元素为时间字符串
        */
        std::pair<bool,std::string> getFileModificationTime(const std::string& filePath) {
        #ifdef _WIN32
            HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            if (hFile == INVALID_HANDLE_VALUE) {
                return {false,"无法打开文件"};
            }
            FILETIME ftCreate, ftAccess, ftWrite;
            if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
                CloseHandle(hFile);
                return {false,"获取文件时间失败"};
            }
            CloseHandle(hFile);
            SYSTEMTIME stUTC, stLocal;
            FileTimeToSystemTime(&ftWrite, &stUTC);
            SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

            char timeStr[256];
            sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d",
                stLocal.wYear, stLocal.wMonth, stLocal.wDay,
                stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
            return {true,timeStr};
        #else
            struct stat fileInfo;
            if (stat(filePath.c_str(), &fileInfo) != 0) {
                return {false,"无法获取文件信息"};
            }
            struct tm* localTime = localtime(&fileInfo.st_mtime);
            if (localTime == nullptr) {
                return {false,"时间转换失败"};
            }
            char timeStr[256];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
            return {true,std::string(timeStr)};
        #endif
        }
        /**
        * @brief 获取文件内容
        * @param fileName 文件路径
        * @return std::string 文件内容
        */
        std::string getFileContent(const char* fileName) {
            std::ifstream file(fileName, std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("无法打开文件");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
        /**
        * @brief 将内容写入文件
        * @param fileName 文件路径
        * @param content 要写入的内容
        * @return std::pair<bool, std::string> 第一个元素为是否成功，第二个元素为错误信息（如果失败）
        */
        std::pair<bool, std::string> writeToFile(const char* fileName, const std::string& content) {
            std::ofstream file(fileName, std::ios::binary | std::ios::trunc);
            if (!file.is_open()) {
                return {false, "无法打开文件进行写入"};
            }
            file << content;
            if (!file) {
                return {false, "写入文件时发生错误"};
            }
            return {true, ""};
        }
        /**
        * @brief 创建目录（如果不存在）
        * @param path 目录路径
        * @return true 目录已存在或成功创建
        * @return false 创建目录失败
        */
        bool createDirectoryIfNotExists(const std::string& path) {
        #ifdef _WIN32
            DWORD attr = GetFileAttributesA(path.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES && 
                (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                return true;
            }
            if (!CreateDirectoryA(path.c_str(), NULL)) {
                return false;
            }
            return true;

        #else
            struct stat info;
            if (stat(path.c_str(), &info) == 0) {
                return (info.st_mode & S_IFDIR) != 0;
            }

            if (mkdir(path.c_str(), 0755) != 0) {
                return false;
            }
            return true;
        #endif
        }
        /**
        * @brief 获取空格字符串
        * @param sumLength 总长度
        * @param occupyLength 已占用长度
        * @return std::string 空格字符串
        */
        std::string getSpace(int sumLength,int occupyLength){
            std::string tmp="";
            for(int i=0;i<sumLength-occupyLength;i++){
                tmp+=" ";
            }
            return tmp;
        }
        /**
        * @brief 获取文件类型
        * @param fileName 文件路径
        * @return FileType 文件类型
        */
        FileType getType(const std::string fileName){
            int fileNameLength=fileName.length();

            if(fileNameLength<=2) return FileType::Other;

            if(fileName.substr((fileNameLength) - std::string(".c").length(),(fileNameLength )) == ".c"){
                return FileType::C;
            }
            else if(fileNameLength>=5 && fileName.substr((fileNameLength) - std::string(".cpp").length(),(fileNameLength )) == ".cpp" ){
                return FileType::CPP;
            }
            return FileType::Other;
        }
        /**
        * @brief 获取文件路径中的纯文件名（不包含扩展名）
        * @param fileName 文件路径
        * @return std::string 纯文件名
        */
        std::string getPureContent(const std::string fileName){
            int fileNameLength=fileName.length();
            FileType type=getType(fileName);
            if(type==FileType::C)
                return fileName.substr(0,fileNameLength-std::string(".c").length());
            else if (type==FileType::CPP) 
                return fileName.substr(0,fileNameLength-std::string(".cpp").length());
            else return fileName;
        }  
    }// namespace FileTools

} // namespace AsulKit

#endif // FILETOOLS_H