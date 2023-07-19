//
// Created by Echo Zhangjie on 19/7/23.
//

#include <filesystem>
static void listFilesRecursively(const std::filesystem::path& path);
static std::filesystem::path ExecutableParentPath = std::filesystem::current_path();
static const char* getResourcePathWith(const std::string& filename);
static const char* getImageResourcePathWith(const std::string& filename);
static const char* ConcatenatePath(const std::filesystem::path& directoryPath, const std::string& filename)
{
    std::filesystem::path filePath = directoryPath;
    filePath.append(filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

static const char* FilePathFor(const std::string& filename) {
    const char*  path = ConcatenatePath(ExecutableParentPath, filename);
    std::cout << "zjzjzj--path" << path << std::endl;
    return path;
}