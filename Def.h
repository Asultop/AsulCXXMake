#ifndef DEF_H
#define DEF_H
//StyleDef
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
//feDelytS

//FilterDef
#define CACHE_DIR_NAME std::string(".cache")
#define FILE_IN_SUFFIX std::string(".in")
#define FILE_OUT_SUFFIX std::string(".out")
#define CPP_COMPILER std::string("g++ ")
#define COMPILER_FLAGS std::string("-O2 ")
#define OUTPUT(x) std::string("-o "+x+EXE_SUFFIX+" ")
//feDretliF

//PlatFormDef
#ifdef _WIN32
    #define CMD_REMOVE_FOLDER_WINSPEC std::string("rd ")// 这个宏仅在 Win 下被定义
    #define CMD_REMOVE      std::string("del ")
    #define RM_FILE         CMD_REMOVE + std::string("/q /f ")
    #define RM_FOLDER       CMD_REMOVE_FOLDER_WINSPEC + std::string("/s /q ")
    #define CMD_COPY        std::string("copy /y ")
    #define INSTALL_DIR     std::string("C:\\Program Files\\"+PROJECT_NAME)
    #define INSTALL_PATH    std::string(INSTALL_DIR+"\\"+PROJECT_NAME+".exe")
    #define EXE_SUFFIX      std::string(".exe") 
    #define LAST_BUILD_INFO ".cache\\Last.build"
    #define CACHE_STREAM    ".cache\\Last.temp.stream"
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
    #define CACHE_STREAM    ".cache/Last.temp.stream"
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
    #define CACHE_STREAM    ".cache/Last.temp.stream"
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
    #define CACHE_STREAM    ".cache/Last.temp.stream"
    #define FILE_PREFIX     std::string("./")
#endif
//feDmroftalP

//System Macro
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
    #define OS_PREFIX "Ubuntu"  
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

#define SYSTEM_ARCH OS_PREFIX ARCH_SUFFIX

// orcaM metsyS

//ReturnCodeDef
#define Accepted 0
#define ArgumentErr 2
#define BadFile 3
#define RunErr 4
#define BuildErr 5
#define EnvErr 6
#define CompilerErr 7
#define TypeErr 8
#define PermissionErr 9
//feDedoCnruteR

//typeDef
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

typedef enum{
    AC,
    WA,
    RE,
    CE
} JudgeState;
//feDepyt
#endif // Def.h