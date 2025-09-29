#define PROJECT_NAME std::string("AMake")
#define PROJECT_VERSION std::string("alpha-v0.1.1")

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

using std::vector; 
using std::cout;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define LIGHT_GRAY "\033[37m"
#define DARK_GRAY "\033[90m"
#define LIGHT_RED "\033[91m"
#define LIGHT_GREEN "\033[92m"
#define LIGHT_YELLOW "\033[93m"
#define LIGHT_BLUE "\033[94m"
#define LIGHT_MAGENTA "\033[95m"
#define LIGHT_CYAN "\033[96m"
#define WHITE "\033[97m"
#define UNDERLINE "\033[4m"

#define CACHE_DIR_NAME std::string(".cache")
#define FILE_IN_SUFFIX std::string(".in ")
#define FILE_OUT_SUFFIX std::string(".out ")
#define CPP_COMPILER std::string("g++ ")
#define COMPILER_FLAGS std::string("-O2 ")
#define OUTPUT(x) std::string("-o "+x+EXE_SUFFIX+" ")

#ifdef _WIN32
    #define CMD_REMOVE_FOLDER_WINSPEC std::string("rd")// 这个宏仅在 Win 下被定义
    #define CMD_REMOVE      std::string("del ")
    #define RM_FILE         CMD_REMOVE + std::string("/q /f ")
    #define RM_FOLDER       CMD_REMOVE_FOLDER_WINSPEC + std::string("/s /q ")
    #define CMD_COPY        std::string("copy /y ")
    #define INSTALL_DIR     std::string("C:\\Program Files\\"+PROJECT_NAME)
    #define INSTALL_PATH    std::string(INSTALL_DIR+"\\"+PROJECT_NAME+".exe")
    #define EXE_SUFFIX      std::string(".exe") 
    #define LAST_BUILD_INFO ".cache\\Last.build"
    #define CACHE_TIME_INFO ".cache\\Last.build.stamp"
    #define FILE_PREFIX     std::string("")
#elif defined(__APPLE__) && defined(__MACH__)
    #define CMD_REMOVE      std::string("rm ")
    #define RM_FILE         CMD_REMOVE + std::string("-f ")
    #define RM_FOLDER       CMD_REMOVE + std::string("-f -R ")
    #define CMD_COPY        std::string("cp -f ")
    #define INSTALL_DIR     std::string("/usr/local/bin")
    #define INSTALL_PATH    std::string(INSTALL_DIR+"/"+PROJECT_NAME)
    #define EXE_SUFFIX      std::string("")
    #define LAST_BUILD_INFO ".cache/Last.build"
    #define CACHE_TIME_INFO ".cache/Last.build.stamp"
    #define FILE_PREFIX     std::string("./")
#elif defined(__linux__)
    #define CMD_REMOVE      std::string("rm ")
    #define RM_FILE         CMD_REMOVE + std::string("-f ")
    #define RM_FOLDER       CMD_REMOVE + std::string("-f -R ")
    #define CMD_COPY        std::string("cp -f ")
    #define INSTALL_DIR    std::string("/usr/local/bin")
    #define INSTALL_PATH    std::string(INSTALL_DIR+"/"+PROJECT_NAME)
    #define EXE_SUFFIX      std::string("")
    #define LAST_BUILD_INFO ".cache/Last.build"
    #define CACHE_TIME_INFO ".cache/Last.build.stamp"
    #define FILE_PREFIX     std::string("./")
#else
    #define CMD_REMOVE      std::string("rm ")
    #define RM_FILE         CMD_REMOVE + std::string("-f ")
    #define RM_FOLDER       CMD_REMOVE + std::string("-f -R ")
    #define CMD_COPY        std::string("cp -f ")
    #define INSTALL_DIR    std::string("/usr/local/bin")
    #define INSTALL_PATH    std::string(INSTALL_DIR+"/"+PROJECT_NAME)
    #define EXE_SUFFIX      std::string("")
    #define LAST_BUILD_INFO ".cache/Last.build"
    #define CACHE_TIME_INFO ".cache/Last.build.stamp"
    #define FILE_PREFIX     std::string("./")
#endif

// System Macro

// 系统和架构检测宏
#if defined(_WIN32) || defined(_WIN64)
    #define OS_PREFIX "Win"
    #if defined(_WIN64)
        #define ARCH_SUFFIX "64"
        #define WINDOWS_64
    #else
        #define ARCH_SUFFIX "32"
        #define WINDOWS_32
    #endif
#elif defined(__linux__)
    #define OS_PREFIX "Ubuntu"  // 这里简化处理为Ubuntu，实际可能需要更复杂的检测
    #if defined(__x86_64__) || defined(_M_X64)
        #define ARCH_SUFFIX "amd64"
        #define UBUNTU_AMD64
    #elif defined(__aarch64__)
        #define ARCH_SUFFIX "arm64"
        #define UBUNTU_ARM64
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #define OS_PREFIX "MacOS"
    #if defined(__x86_64__) || defined(_M_X64)
        #define ARCH_SUFFIX "x86"
        #define MACOS_X86
    #elif defined(__arm64__) || defined(_M_ARM64)
        #define ARCH_SUFFIX "arm"
        #define MACOS_ARM
    #endif
#else
    #define OS_PREFIX "UnknownOS"
    #define ARCH_SUFFIX "UnknownArch"
    #define UnKnown
#endif

// 组合成完整的系统架构标识
#define SYSTEM_ARCH OS_PREFIX ARCH_SUFFIX

// orcaM metsyS


#define Accepted 0
#define ArgumentErr 2
#define BadFile 3
#define RunErr 4
#define BuildErr 5
#define EnvErr 6
#define CompilerErr 7
#define TypeErr 8
#define PermissionErr 9
typedef enum{
    C,
    CPP,
    Other
} FileType;

typedef enum{ 
    Red,
    Blue,
    Green,
    Yellow,
    Magenta,
    Cyan,
    LightGray,
    DarkGray,
    LightRed,
    LightGreen,
    LightYellow,
    LightBlue,
    LightMagenta,
    LightCyan,
    White,
    Normal
} ConsoloColor;

typedef enum{
    Success,
    Info,
    Warn,
    Err
} LogLevel;

typedef enum{
    Y,
    N,
    None
} DefaultCase;

template<typename T>
struct PrintStruct{
    const T content;
    ConsoloColor color;
    PrintStruct(const T m_content, ConsoloColor m_color) 
        : content(m_content), color(m_color) {}
};

template<typename T>
class PrintMap;
template<typename T>
void print(PrintMap<T> printMap,bool lineBreak=true);


template<typename T>
class PrintMap{
private:
    vector<PrintStruct<T>> m_map;
public:
    using iterator = typename std::vector<PrintStruct<T>>::iterator;
    using const_iterator = typename std::vector<PrintStruct<T>>::const_iterator;
    using reverse_iterator = typename std::vector<PrintStruct<T>>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<PrintStruct<T>>::const_reverse_iterator;

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

    vector<PrintStruct<T>> getMap() { return this->m_map; }
    
    PrintMap<T>& append(const T content, ConsoloColor color = ConsoloColor::Normal) {
        this->m_map.push_back(PrintStruct<T>(content, color));
        return *this;
    }
    
    PrintMap<T>& append(LogLevel level) {
        switch (level) {
            case LogLevel::Success : this->append("[成功] ", ConsoloColor::Green);break;
            case LogLevel::Info : this->append("[通知] ", ConsoloColor::DarkGray);break;
            case LogLevel::Warn : this->append("[警告] ", ConsoloColor::Yellow);break;
            case LogLevel::Err : this->append("[错误] ", ConsoloColor::Red);break;
        }
        
        return *this;
    }
    PrintMap<T>& endl() {
        this->m_map.push_back(PrintStruct<T>("\n",ConsoloColor::Normal));
        return *this;
    }
    PrintMap<T>& fill(){
        
        return *this;
    }
    PrintMap<T>& askYN(DefaultCase dCase) {
        switch (dCase) {
            case DefaultCase::Y : this->m_map.push_back(PrintStruct<T>("(Y/" + std::string(UNDERLINE) + "N" + std::string(RESET) +"):",ConsoloColor::Normal)); break;
            case DefaultCase::N : this->m_map.push_back(PrintStruct<T>("(" + std::string(UNDERLINE) + "Y" + std::string(RESET) + "/N):",ConsoloColor::Normal)); break;
            case DefaultCase::None : this->m_map.push_back(PrintStruct<T>("(Y/N)",ConsoloColor::Normal)); break;
        }
        
        return *this;
    }
    void printMap(bool lineBreak=true){
        print(*this,lineBreak);
    }
};

template<typename T>
void print(const T content, ConsoloColor color = ConsoloColor::Normal) {
    switch (color) {
        case ConsoloColor::Red: cout << RED << content << RESET; break;
        case ConsoloColor::Blue: cout << BLUE << content << RESET; break;
        case ConsoloColor::Green: cout << GREEN << content << RESET; break;
        case ConsoloColor::Yellow: cout << YELLOW << content << RESET; break;
        case ConsoloColor::Magenta: cout << MAGENTA << content << RESET; break;
        case ConsoloColor::Cyan: cout << CYAN << content << RESET; break;
        case ConsoloColor::LightGray: cout << LIGHT_GRAY << content << RESET; break;
        case ConsoloColor::DarkGray: cout << DARK_GRAY << content << RESET; break;
        case ConsoloColor::LightRed: cout << LIGHT_RED << content << RESET; break;
        case ConsoloColor::LightGreen: cout << LIGHT_GREEN << content << RESET; break;
        case ConsoloColor::LightYellow: cout << LIGHT_YELLOW << content << RESET; break;
        case ConsoloColor::LightBlue: cout << LIGHT_BLUE << content << RESET; break;
        case ConsoloColor::LightMagenta: cout << LIGHT_MAGENTA << content << RESET; break;
        case ConsoloColor::LightCyan: cout << LIGHT_CYAN << content << RESET; break;
        case ConsoloColor::White: cout << WHITE << content << RESET; break;
        case ConsoloColor::Normal: cout << content; break;
    }
}

template<typename T>
void print(PrintMap<T> printMap,bool lineBreak) {
    for (auto& printCmd : printMap) {
        print(printCmd.content, printCmd.color);
    }
    if(lineBreak)
        print("\n");
}

template<typename T>
void printErr(const T content, ConsoloColor color = ConsoloColor::Normal) {
    print("[错误] ", ConsoloColor::Red);
    print(content, color);
}

template<typename T>
void printWarn(const T content, ConsoloColor color = ConsoloColor::Normal) {
    print("[警告] ", ConsoloColor::Yellow);
    print(content, color);
}

template<typename T>
void printInfo(const T content, ConsoloColor color = ConsoloColor::Normal) {
    print("[通知] ", ConsoloColor::DarkGray);
    print(content, color);
}

template<typename T>
void printSucc(const T content, ConsoloColor color = ConsoloColor::Normal) {
    print("[成功] ", ConsoloColor::LightGreen);
    print(content, color);
}

bool fileExist(const char* name) {
    return !access(name, F_OK);
}
bool folderExist(const std::string& path) {
    return std::filesystem::is_directory(std::filesystem::path(path));
}
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
std::string getFileContent(const char* fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

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
std::string getSpace(int sumLength,int occupyLength){
    std::string tmp="";
    for(int i=0;i<sumLength-occupyLength;i++){
        tmp+=" ";
    }
    return tmp;
}
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
std::string getPureContent(const std::string fileName){
    int fileNameLength=fileName.length();
    FileType type=getType(fileName);
    if(type==FileType::C)
        return fileName.substr(0,fileNameLength-std::string(".c").length());
    else if (type==FileType::CPP) 
        return fileName.substr(0,fileNameLength-std::string(".cpp").length());
    else return fileName;
}  
bool build(const std::string projectNameSource){
    std::string projectName=getPureContent(projectNameSource);
    std::string SUFFIX;
    FileType type=getType(projectNameSource);
    if(type==FileType::C) SUFFIX = ".c";
    else SUFFIX = ".cpp";
    std::string cmd = CPP_COMPILER + COMPILER_FLAGS + projectName + SUFFIX + " " + OUTPUT(projectName) ;
    PrintMap<std::string>()
    .append(LogLevel::Info)
    .append("执行构建命令：")
    .append(cmd,ConsoloColor::LightGray)
    .printMap();

    if(system(cmd.c_str()) == 0){
        auto fileStampStatus = getFileModificationTime(projectNameSource);
        if(fileStampStatus.first==false){
            printWarn("获取文件修改时间错误！\n");
        }else{
            auto writeReturn=writeToFile(CACHE_TIME_INFO,fileStampStatus.second);
            if(!writeReturn.first){
                PrintMap<std::string>()
                .append(LogLevel::Warn)
                    .append("无法写入缓存文件 :")
                    .append(writeReturn.second,ConsoloColor::LightGray)
                .printMap();
	        }
        }
        
        PrintMap<std::string>()
        .append(LogLevel::Success)
        .append("构建 ")
        .append(projectName,ConsoloColor::Yellow)
        .append(" 完成")
        .printMap();
        return true;
    }
    else PrintMap<std::string>()
        .append(LogLevel::Err)
        .append("构建 ")
        .append(projectName,ConsoloColor::Yellow)
        .append(" 时出现问题")
        .printMap();
    return false;
}

int run(const std::string projectNameSource){

    FileType fileType=getType(projectNameSource);
    std::string inStream="",outStream="";
    std::string projectName=getPureContent(projectNameSource);
    if(fileExist(getPureContent((projectName + EXE_SUFFIX)).c_str()) && fileExist(CACHE_TIME_INFO)){
        std::string lastStamp;
        try {
            lastStamp = getFileContent(CACHE_TIME_INFO);
        } catch (const std::runtime_error& e) {
            printErr(e.what());
            print("\n");
            return BadFile;
        } 
        auto fileStampStatus=getFileModificationTime(projectNameSource);
        if(!fileStampStatus.first){
            printWarn("获取文件修改时间错误！\n");
        }
        if(fileStampStatus.second != lastStamp){
            PrintMap<std::string>()
            .append(LogLevel::Info)
                .append("源代码 ")
                .append(projectNameSource,ConsoloColor::Yellow)
                .append(" 似乎已修改但未编译，是否立刻编译？")
            .endl()
            .append(LogLevel::Info)
                .append("上次编译时间: ").append(lastStamp,ConsoloColor::LightGray)
            .endl()
                .askYN(DefaultCase::N)
            .printMap(false);
            char getIn=getchar();
            int buildReturnStatus=-1;
            switch(getIn){
                case 'Y' :
                case 'y' : 
                case '\n':
                    buildReturnStatus=build(projectNameSource);break;
                default:
                case 'N' :
                case 'n' :
                    break;
            }
            if(buildReturnStatus==0) return BuildErr;
        } else {
            PrintMap<std::string>()
            .append(LogLevel::Info)
                .append("上次编译时间: ").append(lastStamp,ConsoloColor::LightGray)
            .printMap();
        }
    }
    if(fileExist((projectName+FILE_IN_SUFFIX).c_str())) inStream = " < "+projectName+FILE_IN_SUFFIX;
    if(fileExist((projectName+FILE_OUT_SUFFIX).c_str())) outStream = " > "+projectName+FILE_OUT_SUFFIX;
    std::string cmd=FILE_PREFIX+projectName+EXE_SUFFIX+" "+inStream+" "+outStream;

    PrintMap<std::string>()
    .append(LogLevel::Info)
        .append("执行运行命令 : ")
        .append(cmd,ConsoloColor::LightGray)
    .endl()
        .append("╔═════ ")
        .append(projectName+" 开始",ConsoloColor::Yellow)
        .append(" ══════╗")
    .endl()
    .printMap();
    
    int returnCode=system(cmd.c_str());
    if(returnCode == 0){
        PrintMap<std::string>()
        .endl()
            .append("╚═════ ")
            .append(projectName+" 结束",ConsoloColor::Yellow)
            .append(" ══════╝")
        .endl()
            .append(LogLevel::Success)
            .append(projectName,ConsoloColor::Yellow)
            .append(" 正常退出 (Return 0;)")
        .printMap();
        return Accepted;
    }
    else PrintMap<std::string>()
            .append("╚═════ ")
            .append(projectName+" 结束",ConsoloColor::Yellow)
            .append(" ══════╝")
        .endl()
            .append(LogLevel::Warn)
            .append(projectName,ConsoloColor::Yellow)
            .append(" 退出码: ")
            .append(std::to_string(returnCode),ConsoloColor::Red)
        .printMap();
    return returnCode;
}

bool systemSilent(const char* command) {
    char fullCommand[1024];
    #ifdef _WIN32
        snprintf(fullCommand, sizeof(fullCommand), "%s >nul 2>&1", command);
    #else
        snprintf(fullCommand, sizeof(fullCommand), "%s >/dev/null 2>&1", command);
    #endif
    return system(fullCommand);
}



int main(int argc, char* argv[]) {
    // std::ios::sync_with_stdio(false);
    // cin.tie(nullptr), cout.tie(nullptr);
    #ifdef _WIN32
        system("chcp 65001");
    #endif
    if (argc != 2) {
        PrintMap<std::string>()
        .append(LogLevel::Err)
            .append("参数应有")
            .append(" 1 ", ConsoloColor::DarkGray)
            .append("个，而这里出现 ")
            .append(std::to_string(argc - 1), ConsoloColor::DarkGray)
            .append(" 个 \n")
            .append("\t使用 ")
            .append("-h 或 --help", ConsoloColor::Yellow)
            .append(" 查看帮助信息")
            .printMap();
        return ArgumentErr;
    }
    std::string arg = argv[1];
    
    bool hasCXXCompiler=
    #ifdef _WIN32
    	!
    #endif
    systemSilent(std::string(std::string("where ")+CPP_COMPILER).c_str());
	if(!hasCXXCompiler && arg != "--env"){
		PrintMap<std::string>()
		.append(LogLevel::Err)
			.append("未检测到环境中的编译器!")
		.endl()
		.append(LogLevel::Info)
			.append("使用 ")
			.append(FILE_PREFIX + PROJECT_NAME + EXE_SUFFIX + " --env",ConsoloColor::Yellow)
			.append("来检测并修复环境")
		.printMap();
		// return CompilerErr;
	}
    
    if (arg == "--install"){
        #ifdef _WIN32
        	bool runAsAdmin=[=](){
				BOOL isAdmin = FALSE;
			    PSID adminGroupSid = nullptr;
			    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
			    if (!AllocateAndInitializeSid(
			        &ntAuthority,
			        2,
			        SECURITY_BUILTIN_DOMAIN_RID,
			        DOMAIN_ALIAS_RID_ADMINS,
			        0, 0, 0, 0, 0, 0,
			        &adminGroupSid
			    )) 
			        return false;
			    

			    if (!CheckTokenMembership(
			        nullptr,
			        adminGroupSid,
			        &isAdmin
			    )) {
			        FreeSid(adminGroupSid);
			        return false;
			    }
			    FreeSid(adminGroupSid);
			    return isAdmin != FALSE;
			}();
			std::string winSysDir=[=](){
//				std::string buffer;
//		        buffer.resize(MAX_PATH);
//		        UINT length = GetSystemDirectoryA(&buffer[0], MAX_PATH);
//		        if (length == 0 || length > MAX_PATH) {
//		            return std::string("");
//		        }
//		        buffer.resize(length);
//		        return buffer;
				return "C:\\Windows\\System32";
			}();
        	if(!runAsAdmin) {
                bool elevatorAdmin=[=](){
                    char currentExePath[MAX_PATH];
                    GetModuleFileNameA(NULL, currentExePath, MAX_PATH);
                    HINSTANCE result = ShellExecuteA(
                        NULL,
                        "runas",
                        currentExePath,
                        "--install",
                        NULL,
                        SW_SHOWNORMAL
                    );
                    return reinterpret_cast<INT_PTR>(result) > 32;
                }();
                if(!elevatorAdmin){
                    PrintMap<std::string>()
                    .append(LogLevel::Warn)
                        .append("未检测到管理员权限，安装 ")
                        .append(PROJECT_NAME,ConsoloColor::Yellow)
                        .append("@",ConsoloColor::LightBlue)
                        .append(PROJECT_VERSION,ConsoloColor::LightGreen)
                        .append(" 可能无效！")
                    .printMap();
                    return PermissionErr;
                }else{
					return Accepted;
				}
			}
			std::string arg0=std::string(argv[0]);
			size_t pos = arg0.find(EXE_SUFFIX);
		    while (pos != std::string::npos) {
		        arg0.erase(pos, EXE_SUFFIX.length());
		        pos = arg0.find(EXE_SUFFIX, pos);
		    }
		    arg0+=EXE_SUFFIX;
		    std::string fullFilePath=winSysDir + PROJECT_NAME + EXE_SUFFIX;
		    if(fileExist(fullFilePath.c_str())){
				if(systemSilent(std::string(CMD_REMOVE + fullFilePath).c_str()) != 0){
					PrintMap<std::string>()
					.append(LogLevel::Err)
						.append("删除 ")
						.append(fullFilePath, ConsoloColor::Yellow)
						.append("出错 ")
					.printMap();
					system("pause");
					return EnvErr;
				}
			}
			std::string cmd=CMD_COPY + "\"" + arg0 + "\"" + " \"" + winSysDir + "\\" + PROJECT_NAME + EXE_SUFFIX + "\"";
			cout<<"[DEBUG]"<<cmd<<std::endl;
			if(system(cmd.c_str()) == 0){
				PrintMap<std::string>()
				.append(LogLevel::Success)
					.append("安装到 ")
					.append(winSysDir+"\\"+PROJECT_NAME+EXE_SUFFIX,ConsoloColor::LightGray)
				.printMap();
				system("pause");
				return Accepted;
			} else {
				PrintMap<std::string>()
				.append(LogLevel::Err)
					.append("安装失败，请提升至")
					.append("管理员权限",ConsoloColor::Yellow)
					.append("重试")
				.printMap();
				return EnvErr;
			}
        #else
            std::string cmd = CMD_COPY + std::string(argv[0]) + " " + INSTALL_PATH;
            if(!createDirectoryIfNotExists(INSTALL_DIR)){
                PrintMap<std::string>()
                .append(LogLevel::Err)
	                .append("无法创建文件夹 :")
	                .append(INSTALL_DIR,ConsoloColor::LightGray)
	            .printMap();
                return EnvErr;
            }
            if(system(cmd.c_str())==0){
                PrintMap<std::string>()
                .append(LogLevel::Success)
                    .append("已安装到 ")
                    .append(INSTALL_PATH,ConsoloColor::Yellow)
                .printMap();
            	return Accepted;
            }else{
                PrintMap<std::string>()
                .append(LogLevel::Err)
                    .append("无法安装，请使用")
                    .append(" sudo ",ConsoloColor::Yellow)
                    .append("提升权限！")
                .printMap();
                return PermissionErr;
            }
        #endif
        
        return Accepted;
    }
    else if(arg == "--version" || arg == "-v"){
        PrintMap<std::string>()
        .append("╔════")
            .append(" Asul SingleFie Cpp 构建工具",ConsoloColor::Yellow)
            .append(" ═════╗")
        .endl()
            .append("║ 设计者：")
            .append("AsulTop ",ConsoloColor::LightMagenta)
            .append(getSpace(45, std::string("║ 设计者：AsulTop ").length()))
            .append("║")
        .endl()
            .append("║ 版本号：")
            .append(PROJECT_VERSION,ConsoloColor::LightGreen)
            .append(getSpace(45, std::string("║ 版本号："+PROJECT_VERSION).length()))
            .append("║")
        .endl()
            .append("║ 构建日期：")
            .append(__DATE__ "@" __TIME__,ConsoloColor::LightBlue)
            .append(getSpace(45, std::string("║ 构建日期:"+std::string(__DATE__ "@" __TIME__)).length()+1))
            .append("║")
        .endl()
            .append("║ 系统架构：")
            .append(OS_PREFIX,
            #ifdef WINDOWS_64
                ConsoloColor::Blue
            #elif defined(WINDOWS_32)
                ConsoloColor::Blue
            #elif defined(UBUNTU_AMD64) || defined(UBUNTU_ARM64)
                ConsoloColor::Red
            #elif defined(MACOS_X86) || defined(MACOS_ARM)
                ConsoloColor::White
            #endif
            )
            .append(" on ",ConsoloColor::LightGray)
            .append(ARCH_SUFFIX,ConsoloColor::Yellow)
            .append(getSpace(45, std::string("║ 系统架构："+ std::string(OS_PREFIX " on " ARCH_SUFFIX)).length()-1))
            .append("║")
        .endl()
            .append("╚══════════════════════════════════════╝")
        .printMap();
        return Accepted;
    }
    else if(arg == "--help" || arg == "-h") {
        PrintMap<std::string>()
        .append("═════ ", ConsoloColor::LightBlue)
            .append("Asul SingleFile Cpp 构建工具 - 帮助文档", ConsoloColor::Yellow)
            .append(" ═════", ConsoloColor::LightBlue)
        .endl()
            .append("功能：", ConsoloColor::LightGreen)
            .append("\n\t自动化编译、运行单个C++文件，支持构建记录管理（如清理/重新编译）", ConsoloColor::Normal)
        .endl()
            .append("格式：", ConsoloColor::LightGreen)
            .append("\n\t"+std::string(argv[0])+EXE_SUFFIX+" [参数/命令]", ConsoloColor::LightGray)
        .endl()
            .append("参数：", ConsoloColor::LightGreen)
            .append("\n\t-h / --help\t", ConsoloColor::Yellow)
            .append("查看本帮助文档", ConsoloColor::Normal)
            .append("\n\t-v / --version\t", ConsoloColor::Yellow)
            .append("查看工具版本信息（开发者、版本号、构建日期）", ConsoloColor::Normal)
            .append("\n\t--install\t", ConsoloColor::Yellow)
            .append("将本工具安装到 "+INSTALL_PATH+" 当中", ConsoloColor::Normal)
            .append("\n\t--dev\t\t", ConsoloColor::Yellow)
			.append("使用本工具尝试修复C/C++环境", ConsoloColor::Normal)
        .endl()
            .append("命令：", ConsoloColor::LightGreen)
            .append("\n\tclear\t\t", ConsoloColor::Yellow)
            .append("清理上次构建生成的可执行文件", ConsoloColor::Normal)
            .append("\n\tmake\t\t", ConsoloColor::Yellow)
            .append("基于上次构建记录"+ std::string(LAST_BUILD_INFO) +"重新编译项目", ConsoloColor::Normal)
            .append("\n\trun\t\t", ConsoloColor::Yellow)
            .append("运行上次构建的可执行文件（未构建则提示是否编译）", ConsoloColor::Normal)
            .append("\n\tremake\t\t", ConsoloColor::Yellow)
            .append("重新编译并自动运行上次构建的项目（等价于 make + run）", ConsoloColor::Normal)
            .append("\n\tstatus\t\t", ConsoloColor::Yellow)
            .append("查看上一次项目", ConsoloColor::Normal)
        .endl()
            .append("示例：", ConsoloColor::LightGreen)
            .append("\n\t示例1：编译运行 test.cpp 文件")
            .append("\n\t\t"+std::string(argv[0])+" test.cpp", ConsoloColor::LightGray)
            .append("\t（工具会自动生成 test 可执行文件并运行）")
            .append("\n\t示例2：运行上次构建的项目")
            .append("\n\t\t"+std::string(argv[0])+" run", ConsoloColor::LightGray)
            .append("\t（若上次构建文件不存在，会提示是否重新编译）")
        .printMap();
        return Accepted;
    }
    else if(arg == "--env"){
        if(hasCXXCompiler) {
            printSucc("C/C++ 环境完整 \n");
        } else {
            printErr("功能暂未实现 \n");
        }
        return Accepted;
    }
    else if((arg.length()>=1 && arg[0] == '-' ) ||arg.length()>=2 && (arg[0] == '-' && arg[1] == '-')){
        PrintMap<std::string>()
        .append(LogLevel::Warn)
            .append(arg,ConsoloColor::Yellow)
            .append(" ：看起来这似乎是一个选项，仍要将其当作源文件吗?")
        .endl()
            .askYN(DefaultCase::N)
        .printMap(false);
        char getIn=getchar();
        switch(getIn){
            case 'Y' :
            case 'y' : break;
            default:
            case 'N' :
            case 'n' :
                printInfo("用户已取消 \n");
                return Accepted;
        }
    }

    std::string lastBuild = "";

    if (fileExist(LAST_BUILD_INFO)) {
        try {
            lastBuild = getFileContent(LAST_BUILD_INFO);
        } catch (const std::runtime_error& e) {
            printErr(e.what());
            print("\n");
            return BadFile;
        } 
    }
    if (arg == "clear") {
        std::string lastBuildName = getPureContent(lastBuild);
        if (!folderExist(CACHE_DIR_NAME)) {
            PrintMap<std::string>()
            .append(LogLevel::Info)
                .append("无上次构建记录，无需清理 ")
                .append(CACHE_DIR_NAME,ConsoloColor::Yellow)
            .printMap();
            return Accepted;
        }
        // if (!fileExist(lastBuild.c_str())){
        //     printInfo("构建文件不存在，无需清理\n");
        //     return Accepted;
        // }
        std::string cmd_rm_build_file = RM_FILE + FILE_PREFIX + lastBuildName + EXE_SUFFIX;
        std::string cmd_rm_cache_dir = RM_FOLDER + FILE_PREFIX + CACHE_DIR_NAME;
        int errCount = 0;
        if (folderExist(FILE_PREFIX + CACHE_DIR_NAME)){
            PrintMap<std::string>()
            .append("执行清理命令 : ")
                .append(cmd_rm_cache_dir,ConsoloColor::LightGray)
            .printMap();
            errCount += systemSilent(cmd_rm_cache_dir.c_str());
        }
        if (fileExist(lastBuildName.c_str())){
            PrintMap<std::string>()
            .append("执行清理命令：")
                .append(cmd_rm_build_file,ConsoloColor::LightGray)
            .printMap();

            errCount += systemSilent(cmd_rm_build_file.c_str());
        }
        if(errCount == 0){
            PrintMap<std::string>()
            .append(LogLevel::Success)
	            .append("清理存在的项目：")
	            .append(lastBuildName,ConsoloColor::Yellow)
	            .append(" 完成")
            .printMap();
            return Accepted;
        }
        PrintMap<std::string>()
            .append(LogLevel::Warn)
	            .append("清理完成，但出现 ")
                .append(std::to_string(errCount),ConsoloColor::Red)
                .append(" 个错误！")
            .printMap();
        return BadFile;
    }
    else if (arg == "make"){
        if (lastBuild.empty()) {
            printErr("无上次构建记录，无法构建\n");
            return BuildErr;
        }
        if(!build(lastBuild)) return BuildErr;
    
        return Accepted;
    }
    else if (arg == "run"){
        if(lastBuild.empty()){
            printErr("无上次构建记录，无法运行\n");
            return BuildErr;
        }
        if(fileExist((lastBuild+EXE_SUFFIX).c_str())){
            return run(lastBuild+EXE_SUFFIX); 
        }else{
            PrintMap<std::string>()
            .append(LogLevel::Warn)
	            .append("暂未构建 ")
	            .append(lastBuild,ConsoloColor::Yellow)
	            .append(" 是否立刻构建？\n(Y/"+std::string(UNDERLINE) + "N" + std::string(RESET) +"): ")
            .printMap(false);
            char getIn=getchar();
            switch (getIn) {
                case 'y' :
                case 'Y' : 
                    if(!build(lastBuild)) return BuildErr;
                    return run(lastBuild);
                default :
                case 'n' :
                case 'N' :
                    printInfo(std::string("用户已取消 \n"));
            }
        }
        return Accepted;
    }
    else if(arg == "remake"){
        if(lastBuild.empty()){
            printErr("无上次构建记录，无法重构\n");
            return BuildErr;
        }
        if(!fileExist(lastBuild.c_str())) PrintMap<std::string>()
                                        .append(LogLevel::Warn)
                                        .append("不存在构建文件，此时推荐使用")
                                        .append(" make ",ConsoloColor::Yellow)
                                        .append("命令，以减少[错误]警报！")
                                        .printMap();
        if(!build(lastBuild)) return BuildErr;
        return run(lastBuild);
    }
    else if(arg == "status"){
        if(lastBuild.empty()){
            printInfo("暂未构建过项目 （暂存区干净）\n");
            return Accepted;
        }
        PrintMap<std::string>()
        .append(LogLevel::Success)
            .append("上次构建的项目：")
            .append(lastBuild)
        .printMap();
        return Accepted;
    }
    
    if(getType(arg)!=FileType::C && getType(arg)!=FileType::CPP){
        PrintMap<std::string>()
        .append(LogLevel::Err)
            .append(arg,ConsoloColor::Yellow)
            .append(" 类型无效")
        .printMap();
        return TypeErr;
    }

    std::string targetProject=arg;

    if(!fileExist((targetProject).c_str())){
        PrintMap<std::string>()
        .append(LogLevel::Err)
	        .append("文件 ")
	        .append(targetProject,ConsoloColor::Yellow)
	        .append(" 不存在！")
        .printMap();
        return BadFile;
    }
    
    if(!createDirectoryIfNotExists(CACHE_DIR_NAME)){
		PrintMap<std::string>()
		.append(LogLevel::Err)
			.append("无法创建文件夹 :")
			.append(CACHE_DIR_NAME,ConsoloColor::LightGray)
		.printMap();
		return EnvErr;
	};
	
	auto writeReturn=writeToFile(LAST_BUILD_INFO,targetProject);
    if(!writeReturn.first){
		PrintMap<std::string>()
		.append(LogLevel::Warn)
			.append("无法写入缓存文件 :")
			.append(writeReturn.second,ConsoloColor::LightGray)
		.printMap();
	}
	
    if(!fileExist(getPureContent((arg + EXE_SUFFIX)).c_str()))
    // if(true)
        if(!build(targetProject)) return BuildErr;
	
    return run(targetProject);

}
// To-do : 配置清单 -> AMake/.config {targetLanguage:JSON}

/*
"
╔═╦═╗ ╔╗╚╝║═
║ ║ ║
╠═╬═╣
║ ║ ║
╚═╩═╝

*/