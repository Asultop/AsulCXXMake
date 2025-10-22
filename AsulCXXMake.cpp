
#include <chrono>
#define PROJECT_NAME std::string("AMake")
#define PROJECT_VERSION std::string("alpha-v0.2.0")

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>

#include "AsulFormatString.h"
#include "Def.h"
#include "FileTools.h"
#include "3rd/json.hpp"
#include <ctime>


using json = nlohmann::json;
using namespace AsulKit::FileTools;


using std::cout;

std::map<FileType, std::string> FileTypeSuffix = {
    {FileType::C, ".c"},
    {FileType::CPP, ".cpp"},
};


bool getStatusYesOrNo(char getIn, bool defaultYes=true, std::function<void()> onYes=[](){}, std::function<void()> onNo=[](){}) {
    switch (getIn) {
    case 'Y':
    case 'y':
        onYes();
        return true;
    case 'N':
    case 'n':
        onNo();
        return false;
    case '\n':
    default:
        if (defaultYes) {
            onYes();
        } else {
            onNo();
        }
        return defaultYes;
    }
}
bool build(const std::string projectNameSource) {
  std::string projectName = getPureContent(projectNameSource);
  std::string SUFFIX,ProgramSUFFIX;
  FileType type = getType(projectNameSource);
  SUFFIX = FileTypeSuffix[type];
  ProgramSUFFIX = SUFFIX;
  std::replace(ProgramSUFFIX.begin(), ProgramSUFFIX.end(), '.', '_');
  
  std::string cmd = CPP_COMPILER + COMPILER_FLAGS + projectName + SUFFIX + " " +
                    OUTPUT(projectName+ProgramSUFFIX);
  cout << f("(INFO) 执行构建命令: {DARK_GRAY} \n", cmd);

  auto Begin = std::chrono::high_resolution_clock::now();

  if (system(cmd.c_str()) == 0) {
    auto fileStampStatus = getFileModificationTime(projectNameSource);
    if (fileStampStatus.first == false) {
      cout << f("(WARN) 获取文件修改时间错误！\n");
    } else {
      json stampDoc;
      if(fileExist(CACHE_TIME_INFO)) {
         stampDoc = json::parse(getFileContent(CACHE_TIME_INFO));
      }
      stampDoc[projectNameSource] = fileStampStatus.second;
      auto writeReturn = writeToFile(CACHE_TIME_INFO, stampDoc.dump(4));
      if (!writeReturn.first) {
        cout << f("(WARN) 无法写入缓存文件 : {DARK_GRAY}\n",
                  writeReturn.second);
      }
    }
    cout << f("(SUCCESS) 构建 {YELLOW} 完成\n", projectName);
    cout << f("(INFO) 耗时 {DARK_GRAY} s\n", std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-Begin).count());
    return true;
  } else
    cout << f("(ERROR) 构建 {YELLOW} 时出现问题\n", projectName);
  return false;
}

int run(const std::string projectNameSource) {

  FileType fileType = getType(projectNameSource);
  std::string ProgramSUFFIX = FileTypeSuffix[fileType];
  std::replace(ProgramSUFFIX.begin(), ProgramSUFFIX.end(), '.', '_');

  std::string inStream = "";
  std::string projectName = getPureContent(projectNameSource)+ProgramSUFFIX;
  bool hasOutStream = false;
  std::string lastStamp;
  std::pair<bool, std::string> fileStampStatus;
  if(fileExist(CACHE_TIME_INFO)){
    try {
      lastStamp = getFileContent(CACHE_TIME_INFO);
      json stampDoc=json::parse(lastStamp);
      if(stampDoc.find(projectNameSource)==stampDoc.end()){
          lastStamp="";
      }else lastStamp=stampDoc[projectNameSource].get<std::string>();
    } catch (const std::runtime_error &e) {
      cout << f("(ERROR) 获取缓存时间失败: {DARK_GRAY}\n", e.what());
      return BadFile;
    }
    fileStampStatus = getFileModificationTime(projectNameSource);
    if (!fileStampStatus.first) { 
      cout<<f("(ERROR) 获取文件修改时间失败\n");
    }
  }
  if (fileExist(((projectName) + EXE_SUFFIX).c_str())) {
    if (fileStampStatus.second != lastStamp) {
      cout << f("(INFO) 源代码 {YELLOW} 似乎已修改但未编译，是否立刻编译？\n", projectNameSource);
      cout << f("(INFO) 上次编译时间: {DARK_GRAY}\n(ASK_Y): ", lastStamp);
      char getIn = getchar();
      int buildReturnStatus = -1;
      getStatusYesOrNo(getIn, true, [&]() {
          buildReturnStatus = build(projectNameSource);
      },[&](){
          buildReturnStatus = 0;
      });
      if (buildReturnStatus == 0)
        return BuildErr;
    } else {
      cout << f("(INFO) 上次编译时间: {DARK_GRAY}\n", lastStamp);
    }
  }
  if(fileExist((projectName+ EXE_SUFFIX).c_str()) && lastStamp.empty()){
      cout << f("(WARN) 未找到缓存时间文件，但已存在可执行文件，是否直接执行？\n(ASK_Y) ");
      getStatusYesOrNo(getchar(), true, []() {
          // do nothing, continue
      }, []() {
          cout << f("(INFO) 用户已取消运行\n");
          return Accepted;
      });
      cout << f("(INFO) 写入缓存时间？\n (ASK_N) ");
      getStatusYesOrNo(getchar(), false, [&]() {
          auto fileStampStatus = getFileModificationTime(projectNameSource);
          if (fileStampStatus.first == false) {
              cout << f("(WARN) 获取文件修改时间错误！\n");
          } else {
              json stampDoc;
              if(fileExist(CACHE_TIME_INFO)) {
                 stampDoc = json::parse(getFileContent(CACHE_TIME_INFO));
              }
              stampDoc[projectNameSource] = fileStampStatus.second;
              auto writeReturn = writeToFile(CACHE_TIME_INFO, stampDoc.dump(4));
              if (!writeReturn.first) {
                cout << f("(WARN) 无法写入缓存文件 : {DARK_GRAY}\n",
                          writeReturn.second);
              }
          }
      },[&](){
          cout << f("(INFO) 未写入文件\n");
          return Accepted;
      });
  }
  if (fileExist((projectName + FILE_IN_SUFFIX).c_str()))
    inStream = " < " + projectName + FILE_IN_SUFFIX;
  if (fileExist((projectName + FILE_OUT_SUFFIX).c_str()))
    hasOutStream = true;

  std::string cmd = FILE_PREFIX + projectName + EXE_SUFFIX + " " + inStream;
  if (hasOutStream)
    cmd += " >" + std::string(CACHE_STREAM);
  cout << f("(INFO) 执行运行命令 : {DARK_GRAY}\n", cmd);
  cout << f("═════ {YELLOW} ══════\n", projectName+"开始");
  
  int returnCode = system(cmd.c_str());
  std::string outStream;
  JudgeState state;
  if (hasOutStream) {
    std::string outFileContent;
    try {
      outStream = getFileContent(CACHE_STREAM);
    } catch (const std::runtime_error &e) {
      cout<<f("(ERROR) 获取缓存流失败: {DARK_GRAY}\n", e.what());
      return BadFile;
    }
    try {
      outFileContent = getFileContent((projectName + FILE_OUT_SUFFIX).c_str());
    } catch (const std::runtime_error &e) {
      cout<<f("(ERROR) 获取输出文件失败: {DARK_GRAY}\n", e.what());
      return BadFile;
    }
    if (outStream == outFileContent)
      state = JudgeState::AC;
    else if (outStream + "\n" == outFileContent)
      state = JudgeState::AC;
    else if (outStream == outFileContent + "\n")
      state = JudgeState::AC;
    else
      state = JudgeState::WA;
  }
  std::string result;
  if (hasOutStream) {
      result += outStream;
  }
  result += "\n";
  result += f("═════ {YELLOW} ══════", projectName+"结束");
  result += "\n";
  
  if (hasOutStream) {
      if (state == JudgeState::AC) {
          result += f("(SUCCESS) {GREEN} 与 {DARK_GRAY} 比较时通过"
            ,"[AC]",projectName + FILE_OUT_SUFFIX);
      } else if (state == JudgeState::WA) {
          result += f("(WARN) {RED} 与 {DARK_GRAY} 比较不通过"
            ,"[WA]",projectName + FILE_OUT_SUFFIX);
      }
      result += "\n";
  }

  if (returnCode == 0) {
      result += f("(SUCCESS) {YELLOW} 正常退出 (返回值 : {GREEN})\n", projectName, 0);
      std::cout << result;
      return Accepted;
  } else {
      result += f("(WARN) {YELLOW} 退出码: {RED}\n", projectName, returnCode);
      std::cout << result;
      return returnCode;
  }

}

bool systemSilent(const char *command) {
  char fullCommand[1024];
#ifdef _WIN32
  snprintf(fullCommand, sizeof(fullCommand), "%s >nul 2>&1", command);
#else
  snprintf(fullCommand, sizeof(fullCommand), "%s >/dev/null 2>&1", command);
#endif
  return system(fullCommand);
}

int main(int argc, char *argv[]) {
  asul_formatter().installColorFormatAdapter();
  asul_formatter().installLogLabelAdapter();
  asul_formatter().installAskLabelAdapter();

  if (argc != 2) {
    cout << f("(ERROR) 参数应有 {DARK_GRAY} 个，而这里出现 {DARK_GRAY} "
              "个\n",
              1, argc - 1);
    return ArgumentErr;
  }
  std::string arg = argv[1];

  bool hasCXXCompiler =
#ifdef _WIN32
      !
#endif
      systemSilent(std::string(std::string("where ") + CPP_COMPILER).c_str());
  if (!hasCXXCompiler && arg != "--env") {
    cout<< f("(ERROR) 未检测到环境中的编译器!\n(INFO) 使用 {YELLOW} 来修复环境\n", FILE_PREFIX + PROJECT_NAME + EXE_SUFFIX + " --env");
    // return CompilerErr;
  }
  if (arg == "--install") {
#ifdef _WIN32
    bool runAsAdmin = [=]() {
      BOOL isAdmin = FALSE;
      PSID adminGroupSid = nullptr;
      SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
      if (!AllocateAndInitializeSid(
              &ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
              DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroupSid))
        return false;

      if (!CheckTokenMembership(nullptr, adminGroupSid, &isAdmin)) {
        FreeSid(adminGroupSid);
        return false;
      }
      FreeSid(adminGroupSid);
      return isAdmin != FALSE;
    }();
    std::string winSysDir = [=]() {
      return "C:\\Windows\\System32";
    }();
    if (!runAsAdmin) {
      bool elevatorAdmin = [=]() {
        char currentExePath[MAX_PATH];
        GetModuleFileNameA(NULL, currentExePath, MAX_PATH);
        HINSTANCE result = ShellExecuteA(NULL, "runas", currentExePath,
                                         "--install", NULL, SW_SHOWNORMAL);
        return reinterpret_cast<INT_PTR>(result) > 32;
      }();
      if (!elevatorAdmin) {
        cout << f("(WARN) 未检测到管理员权限，安装 {YELLOW} {BLUE} {LIGHT_GREEN} 可能无效！\n", PROJECT_NAME, "@", PROJECT_VERSION);
        return PermissionErr;
      } else {
        return Accepted;
      }
    }
    std::string arg0 = std::string(argv[0]);
    size_t pos = arg0.find(EXE_SUFFIX);
    while (pos != std::string::npos) {
      arg0.erase(pos, EXE_SUFFIX.length());
      pos = arg0.find(EXE_SUFFIX, pos);
    }
    arg0 += EXE_SUFFIX;
    std::string fullFilePath = winSysDir + PROJECT_NAME + EXE_SUFFIX;
    if (fileExist(fullFilePath.c_str())) {
      if (systemSilent(std::string(CMD_REMOVE + fullFilePath).c_str()) != 0) {
        cout << f("(ERROR) 删除 {YELLOW} 出错\n", fullFilePath);
        system("pause");
        return EnvErr;
      }
    }
    std::string cmd = CMD_COPY + "\"" + arg0 + "\"" + " \"" + winSysDir + "\\" +
                      PROJECT_NAME + EXE_SUFFIX + "\"";
    cout << "[DEBUG]" << cmd << std::endl;
    if (system(cmd.c_str()) == 0) {
      cout << f("(SUCCESS) 安装到 {DARK_GRAY} \n", winSysDir + "\\" + PROJECT_NAME + EXE_SUFFIX);
      system("pause");
      return Accepted;
    } else {
      cout<< f("(ERROR) 安装失败，请提升至 {YELLOW} 重试\n","管理员权限");
      return EnvErr;
    }
#else
    std::string cmd = CMD_COPY + std::string(argv[0]) + " " + INSTALL_PATH;
    if (!createDirectoryIfNotExists(INSTALL_DIR)) {
      cout << f("(ERROR) 无法创建文件夹：{DARK_GRAY}\ns",INSTALL_DIR);
      return EnvErr;
    }
    if (system(cmd.c_str()) == 0) {

      cout << f("(SUCCESS) 安装到 {DARK_GRAY} \n", INSTALL_PATH);
      return Accepted;
    } else {
      cout << f("(ERROR) 无法安装 {YELLOW} {BLUE} {DARK_GRAY},请使用 {DARK_GRAY} 提升权限！\n",PROJECT_NAME,"on",PROJECT_VERSION, "sudo");
      return PermissionErr;
    }
#endif

    return Accepted;
  } else if (arg == "--version" || arg == "-v") {
    cout << f(
        "╔════ {YELLOW} ══════╗\n\
║ 设计者： {LIGHT_MAGENTA}\t\t\t║\n\
║ 版本号：{LIGHT_GREEN}\t\t\t║\n\
║ 构建日期：{LIGHT_BLUE}\t║\n\
║ 系统架构："
#ifdef NOTANYTHING
#elif defined(WINDOWS_64) || defined(WINDOWS_32)
        "{BLUE}"
#elif defined(UBUNTU_AMD64) || defined(UBUNTU_ARM64)
        "{RED}"
#elif defined(MACOS_X86) || defined(MACOS_ARM)
        "{WHITE}"
#endif

        " {DARK_GRAY} {YELLOW}\t\t"
#ifdef NOTANYTHING
#elif defined(WINDOWS_64) || defined(WINDOWS_32)
        "\t"
#endif

        
"║\n╚═══════════════════════════════════════╝\n",
        "Asul SingleFie Cpp 构建工具", "AsulTop", PROJECT_VERSION,
        __DATE__ "@" __TIME__, OS_PREFIX,"on", ARCH_SUFFIX);

    return Accepted;
  } else if (arg == "--help" || arg == "-h") {
    cout << f("═════ {LIGHT_BLUE} ═════\n", "Asul SingleFile Cpp 构建工具 - 帮助文档");
    cout << f("{LIGHT_GREEN}\n\t自动化编译、运行单个 C/C++ 文件，支持构建记录管理（如清理/重新编译）\n","功能：");
    cout<< f("{LIGHT_GREEN}\n\tAMake [参数/命令]\n","格式：");
    cout << f("{LIGHT_GREEN}\n","参数：");
    cout<< f("\t{YELLOW}\t{DARK_GRAY}\n","-h / --help","查看本帮助文档");
    cout << f("\t{YELLOW}\t{DARK_GRAY}\n","-v / --version","查看工具版本信息（开发者、版本号、构建日期）");
    cout << f("\t{YELLOW}\t{DARK_GRAY}\n", "--install",f("将本工具安装到 {} 当中",INSTALL_PATH));
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","--dev","使用本工具尝试修复C/C++环境");
    cout << f("{LIGHT_GREEN}\n","命令：");
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","clear","清理上次构建生成的可执行文件");
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","make",f("基于上次构建记录 {} 重新编译项目", std::string(LAST_BUILD_INFO)));
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","run","运行上次构建的可执行文件（未构建则提示是否编译）");
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","remake","重新编译并自动运行上次构建的项目（等价于 make + run）");
    cout << f("\t{YELLOW}\t\t{DARK_GRAY}\n","status","查看上一次项目");
    cout << f("{LIGHT_GREEN}","示例：\n");
    cout << f("\t示例1：编译运行 test.cpp 文件\n");
    cout << f("\t\t{DARK_GRAY} test.cpp\t（工具会自动生成 test 可执行文件并运行）\n", std::string(argv[0]));
    cout << f("\t示例2：运行上次构建的项目\n");
    cout << f("\t\t{DARK_GRAY} run\t（若上次构建文件不存在，会提示是否重新编译）\n", std::string(argv[0]));
    cout << "════════════════════════════════════════════════════\n";
    return Accepted;
  } else if (arg == "--env") {
    if (hasCXXCompiler) {
      cout << f("(SUCCESS) C/C++ 环境完整 \n");
    } else {
      cout << f("(WARN) 功能暂未实现 \n(Error) 缺失环境！");
    }
    return Accepted;
  } else if ((arg.length() >= 1 && arg[0] == '-') ||
             arg.length() >= 2 && (arg[0] == '-' && arg[1] == '-')) {
    cout << f("(WARN) {YELLOW} ：看起来似乎是一个选项，仍要将其当作源文件吗? (ASK_N) \n",arg);
    char getIn = getchar();

    getStatusYesOrNo(getIn, false, []() {
        // do nothing, continue
    }, []() {
        cout << f("(INFO) 用户已取消\n");
        exit(Accepted);
    });
  }

  std::string lastBuild = "";
  if (fileExist(LAST_BUILD_INFO)) {
    try {
      lastBuild = getFileContent(LAST_BUILD_INFO);
      json lastBuildDoc=json::parse(lastBuild);
      lastBuild=lastBuildDoc[PROJECT_NAME].get<std::string>();
    } catch (const std::runtime_error &e) {
      cout << f("(ERROR) {}\n",e.what());
      return BadFile;
    }
  }
  if (arg == "clear") {
    std::string lastBuildName = getPureContent(lastBuild);
    if (!folderExist(CACHE_DIR_NAME)) {
      cout << f("(INFO) 无上次构建记录，无需清理 {YELLOW}\n",CACHE_DIR_NAME);
      return Accepted;
    }
    std::string cmd_rm_build_file =
        RM_FILE + FILE_PREFIX + lastBuildName + EXE_SUFFIX;
    std::string cmd_rm_cache_dir = RM_FOLDER + FILE_PREFIX + CACHE_DIR_NAME;
    int errCount = 0;
    if (folderExist(FILE_PREFIX + CACHE_DIR_NAME)) {
      cout << f("(INFO) 执行清理命令 : {DARK_GRAY}\n", cmd_rm_cache_dir);
      errCount += systemSilent(cmd_rm_cache_dir.c_str());
    }
    if (fileExist(lastBuildName.c_str())) {
      cout << f("(INFO) 执行清理命令：{DARK_GRAY}",cmd_rm_build_file);
      errCount += systemSilent(cmd_rm_build_file.c_str());
    }
    if (errCount == 0) {
      cout << f("(SUCCESS) 清理存在的项目：{YELLOW} 完成\n", lastBuildName);
      return Accepted;
    }
    cout << f("(WARN) 清理完成，但出现 {RED} 个错误！\n", errCount);
    return BadFile;
  } else if (arg == "make") {
    if (lastBuild.empty()) {
      cout << f("(ERROR) 无上次构建记录，无法构建\n");
      return BuildErr;
    }
    if (!build(lastBuild))
      return BuildErr;

    return Accepted;
  } else if (arg == "run") {
    if (lastBuild.empty()) {
      cout << f("(ERROR) 无上次构建记录，无法运行\n");
      return BuildErr;
    }
    std::string lastBuildPure = getPureContent(lastBuild);
    if (fileExist((lastBuildPure + EXE_SUFFIX).c_str())) {
      return run(lastBuild);
    } else {
      cout << f("(WARN) 暂未构建 {YELLOW} 是否立刻构建？\n(ASK_N):", lastBuildPure);
      char getIn = getchar();
      getStatusYesOrNo(getIn, false, [&]() {
          if (!build(lastBuild))
              return BuildErr;
          return run(lastBuild);
      },[&](){
          cout << f("(INFO) 用户已取消 \n");
          return Accepted;
      });
    }
    return Accepted;
  } else if (arg == "remake") {
    if (lastBuild.empty()) {
      cout << f("(ERROR) 无上次构建记录，无法重构\n");
      return BuildErr;
    }
    std::string lastBuildPure = getPureContent(lastBuild);

    if (!fileExist(std::string(lastBuildPure + EXE_SUFFIX).c_str()))
      cout << f("(WARN) 不存在构建文件，此时推荐使用 {YELLOW} 命令，以减少错误警报！\n", "make");
    if (!build(lastBuild))
      return BuildErr;
    return run(lastBuild);
  } else if (arg == "status") {
    if (lastBuild.empty()) {
      cout << f("(INFO) 暂未构建过项目 （暂存区干净）\n");
      return Accepted;
    }
    cout << f("(SUCCESS) 上次构建的项目：{YELLOW} \n", lastBuild);
    return Accepted;
  }

  if (getType(arg) != FileType::C && getType(arg) != FileType::CPP) {
    cout << f("(ERROR) {YELLOW} 类型无效\n", arg);
    return TypeErr;
  }
  std::string targetProject = arg;

  if (!fileExist((targetProject).c_str())) {
    cout << f("(ERROR) 文件 {YELLOW} 不存在！\n", targetProject);
    return BadFile;
  }

  if (!createDirectoryIfNotExists(CACHE_DIR_NAME)) {
    cout << f("(ERROR) 无法创建文件夹 : {DARK_GRAY}\n", CACHE_DIR_NAME);
    return EnvErr;
  };
  json lastBuildDoc;
  if(fileExist(LAST_BUILD_INFO)) {
     lastBuildDoc = json::parse(getFileContent(LAST_BUILD_INFO));
  }
  lastBuildDoc[PROJECT_NAME]=targetProject;
  auto writeReturn = writeToFile(LAST_BUILD_INFO, lastBuildDoc.dump(4));
  if (!writeReturn.first) {
    cout << f("(WARN) 无法写入缓存文件 : {DARK_GRAY}\n", writeReturn.second);
  }
  std::string ProgramSUFFIX = FileTypeSuffix[getType(arg)];
  std::replace(ProgramSUFFIX.begin(), ProgramSUFFIX.end(), '.', '_');
  if (!fileExist((getPureContent(arg)+ ProgramSUFFIX + EXE_SUFFIX).c_str()))
    // if(true)
    if (!build(targetProject))
      return BuildErr;

  
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
