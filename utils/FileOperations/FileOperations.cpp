#include "FileOperations.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace FileOperations
{
    bool moveFile(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path srcPath(source);
        fs::path destPath(destination);

        // 源文件不存在，返回失败
        if (!fs::exists(srcPath, ec))
            return false;

        // 目标文件如果存在
        if (fs::exists(destPath, ec))
        {
            ec.clear();
            if (overwrite)
            {
                // 删除目标文件
                if (!fs::remove(destPath, ec) || ec)
                    return false;
            }
            else
            {
                // 目标文件存在且不允许覆盖，返回错误
                ec = std::make_error_code(std::errc::file_exists);
                return false;
            }
        }

        fs::rename(srcPath, destPath, ec);
        return !ec;
    }

    bool moveAny(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path srcPath(source);
        fs::path destPath(destination);

        // 首先解析路径，处理相对路径和符号链接
        fs::path srcResolved = fs::canonical(srcPath, ec);
        if (ec)
            return false;

        fs::path destResolved = fs::canonical(destPath.parent_path(), ec);
        // 如果目标父目录不存在，先获取其绝对路径用于比较
        if (ec)
        {
            ec.clear();
            destResolved = fs::absolute(destPath.parent_path());
        }
        // 构建完整的目标路径
        destResolved /= destPath.filename();

        // 检查源路径和目标路径是否相同
        if (srcResolved == destResolved)
        {
            // 源路径和目标路径相同，无需移动
            ec.clear();
            return true;
        }

        // 检查源是否存在
        if (!fs::exists(srcPath, ec))
            return false;

        if (fs::exists(destPath, ec))
        {
            ec.clear();
            if (overwrite)
            {
                if (!fs::remove_all(destPath, ec) || ec)
                    return false;
            }
            else
            {
                ec = std::make_error_code(std::errc::file_exists);
                return false;
            }
        }

        // 确保目标路径的父目录存在
        fs::path destParent = destPath.parent_path();
        if (!fs::exists(destParent, ec))
        {
            ec.clear();
            if (!fs::create_directories(destParent, ec))
                return false;
        }

        fs::rename(srcPath, destPath, ec);
        return !ec;
    }

    bool rename(const std::string &oldPath, const std::string &newPath, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path old_path(oldPath);
        fs::path new_path(newPath);

        // 检查源路径是否存在
        if (!fs::exists(old_path, ec))
        {
            return false;
        }

        // 检查新旧路径是否相同
        if (fs::equivalent(old_path, new_path, ec))
        {
            ec.clear();
            return true; // 路径相同，无需重命名
        }

        // 处理目标路径已存在的情况
        if (fs::exists(new_path, ec))
        {
            ec.clear();
            if (overwrite)
            {
                // 删除目标路径
                if (!fs::remove_all(new_path, ec) || ec)
                {
                    return false;
                }
            }
            else
            {
                // 目标存在且不允许覆盖
                ec = std::make_error_code(std::errc::file_exists);
                return false;
            }
        }

        // 确保目标路径的父目录存在
        fs::path new_parent = new_path.parent_path();
        if (!new_parent.empty() && !fs::exists(new_parent, ec))
        {
            ec.clear();
            if (!fs::create_directories(new_parent, ec))
            {
                return false;
            }
        }

        // 执行重命名操作
        fs::rename(old_path, new_path, ec);
        return !ec;
    }

    bool copyFile(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::copy_options options = overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none;

        fs::copy_file(fs::path(source), fs::path(destination), options, ec);
        return !ec;
    }

    bool copyAny(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path srcPath(source);
        fs::path destPath(destination);

        // 检查源是否存在
        if (!fs::exists(srcPath, ec))
        {
            return false;
        }

        // 设置复制选项
        fs::copy_options options = fs::copy_options::none;
        if (overwrite)
        {
            options |= fs::copy_options::overwrite_existing;
        }

        // 处理目录复制的特殊情况
        if (fs::is_directory(srcPath, ec))
        {
            options |= fs::copy_options::recursive;

            // 检查目标是否存在
            if (fs::exists(destPath, ec))
            {
                // 如果目标存在且是文件，无法复制目录到文件
                if (fs::is_regular_file(destPath, ec))
                {
                    ec = make_error_code(std::errc::is_a_directory);
                    return false;
                }
                // 如果目标是目录，确保我们复制的是目录内容到目标目录
                destPath /= srcPath.filename();
            }
            else
            {
                // 确保目标目录的父目录存在
                fs::path parentDir = destPath.parent_path();
                if (!parentDir.empty() && !fs::exists(parentDir, ec))
                {
                    if (!fs::create_directories(parentDir, ec))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            // 处理文件复制：确保目标目录存在
            if (!fs::exists(destPath.parent_path(), ec))
            {
                if (!fs::create_directories(destPath.parent_path(), ec))
                {
                    return false;
                }
            }
        }

        // 执行复制
        fs::copy(srcPath, destPath, options, ec);
        return !ec;
    }

    bool deleteFile(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        fs::remove(fs::path(path), ec);
        return !ec;
    }

    std::string getErrorMessage(const std::error_code &ec)
    {
        if (!ec)
        {
            return "Success";
        }
        return "Error: " + ec.message() + " (errorcode: " + std::to_string(ec.value()) + ")";
    }

    bool createDir(const std::string &dirPath, std::error_code &ec)
    {
        ec.clear();
        return fs::create_directory(fs::path(dirPath), ec);
    }

    bool createDirRecursive(const std::string &dirPath, std::error_code &ec)
    {
        ec.clear();
        return fs::create_directories(fs::path(dirPath), ec);
    }

    bool deleteEmptyDir(const std::string &dirPath, std::error_code &ec)
    {
        ec.clear();
        fs::path dir = fs::path(dirPath);

        // 先判断路径是否存在且为目录
        if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
        {
            ec = std::make_error_code(std::errc::not_a_directory);
            return false;
        }

        bool result = fs::remove(dir, ec);

        // 如果删除失败且错误是"目录非空"，明确错误码
        if (!result && ec.value() == static_cast<int>(std::errc::directory_not_empty))
        {
            return false;
        }

        return result;
    }

    bool deleteDirRecursive(const std::string &dirPath, std::error_code &ec)
    {
        ec.clear();
        fs::remove_all(fs::path(dirPath), ec);
        return !ec;
    }

    bool deleteAny(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        fs::path targetPath(path);

        // 检查路径是否存在
        if (!fs::exists(targetPath, ec))
        {
            // 路径不存在也视为成功删除状态
            ec.clear();
            return true;
        }

        fs::remove_all(targetPath, ec);
        return !ec;
    }

    FileType getFileType(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        fs::file_status status = fs::status(fs::path(path), ec);
        if (ec)
            return FileType::NON_EXIST;

        if (fs::is_regular_file(status))
            return FileType::FILE;
        else if (fs::is_directory(status))
            return FileType::DIRECTORY;
        else if (fs::is_symlink(status))
            return FileType::SYMLINK;
        else
            return FileType::OTHER;
    }

    int64_t getFileSize(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        fs::path p(path);

        // 先判断路径是否存在
        if (!fs::exists(p, ec))
            return -1;

        // 普通文件：直接获取大小；目录：返回 0（需单独计算目录大小）
        if (fs::is_regular_file(p, ec))
            return static_cast<int64_t>(fs::file_size(p, ec));
        else
            return 0;
    }

    std::string getLastModifyTime(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        fs::path p(path);
        if (!fs::exists(p, ec))
            return "";

        // 获取最后修改时间（兼容实验性filesystem）
        auto ftime = fs::last_write_time(p, ec);
        if (ec)
            return "";

        // 实验性版本兼容代码
        auto cftime = std::chrono::system_clock::to_time_t(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                ftime.time_since_epoch() - std::chrono::system_clock::now().time_since_epoch()) +
            std::chrono::system_clock::now());

        // 格式化时间为 "YYYY-MM-DD HH:MM:SS"
        std::stringstream ss;
        ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    bool exists(const std::string &path, std::error_code &ec)
    {
        ec.clear();
        return fs::exists(fs::path(path), ec);
    }

    // 辅助函数：将 fs::path 转换为 FileInfo（内部使用）
    static FileInfo pathToFileInfo(const fs::path &p, std::error_code &ec)
    {
        FileInfo info;
        info.name = p.filename().string();             // 文件名（不含路径）
        info.fullPath = fs::canonical(p, ec).string(); // 绝对路径（避免相对路径问题）
        if (ec)
            info.fullPath = p.string(); // 若获取绝对路径失败，用原始路径

        // 填充文件类型
        info.type = getFileType(info.fullPath, ec);
        // 填充大小
        info.size = getFileSize(info.fullPath, ec);
        // 填充最后修改时间
        info.lastModify = getLastModifyTime(info.fullPath, ec);

        return info;
    }

    bool listDir(const std::string &dirPath, std::vector<FileInfo> &outFiles, bool includeHidden, std::error_code &ec)
    {
        ec.clear();
        fs::path dir_p(dirPath);

        // 先判断目录是否存在且是目录
        if (!fs::exists(dir_p, ec) || !fs::is_directory(dir_p, ec))
            return false;

        outFiles.clear();
        // 非递归遍历目录（directory_iterator 仅遍历当前目录）
        for (const auto &entry : fs::directory_iterator(dir_p, ec))
        {
            if (ec)
                break;

            fs::path entry_p = entry.path();
            std::string entry_name = entry_p.filename().string();

            // 过滤隐藏文件（以 '.' 开头，且不是 "." 和 ".."）
            if (!includeHidden && entry_name.size() > 0 && entry_name[0] == '.' && entry_name != "." && entry_name != "..")
                continue;

            // 转换为 FileInfo 并加入结果
            outFiles.push_back(pathToFileInfo(entry_p, ec));
        }

        return !ec;
    }

    bool listDirRecursive(const std::string &dirPath, std::vector<FileInfo> &outFiles, bool includeHidden, std::error_code &ec)
    {
        ec.clear();
        fs::path dir_p(dirPath);

        if (!fs::exists(dir_p, ec) || !fs::is_directory(dir_p, ec))
            return false;

        outFiles.clear();
        // 递归遍历目录（recursive_directory_iterator 深入子目录）
        for (const auto &entry : fs::recursive_directory_iterator(dir_p, ec))
        {
            if (ec)
                break;

            fs::path entry_p = entry.path();
            std::string entry_name = entry_p.filename().string();

            if (!includeHidden && entry_name.size() > 0 && entry_name[0] == '.' && entry_name != "." && entry_name != "..")
                continue;

            outFiles.push_back(pathToFileInfo(entry_p, ec));
        }

        return !ec;
    }

    bool deleteWithRestrict(std::string &path, const std::string &allowedRoot)
    {
        fs::path allowedPath(allowedRoot);
        fs::path targetPath(path);

        // 转换为绝对路径，避免相对路径绕过检查
        fs::path absoluteTarget = fs::absolute(targetPath);
        fs::path absoluteAllowed = fs::absolute(allowedPath);

        // 检查目标路径是否在允许的目录范围内
        // 即判断allowedPath是否为absoluteTarget的祖先目录
        if (absoluteTarget.string().substr(0, absoluteAllowed.string().size()) != absoluteAllowed.string())
        {
            // 不在允许的目录范围内，拒绝删除
            return false;
        }

        // 检查路径是否存在且可访问
        std::error_code ec;
        if (!FileOperations::exists(absoluteTarget.string(), ec) || ec)
        {
            return false;
        }

        // 调用FileOperations命名空间中的删除函数
        return FileOperations::deleteAny(absoluteTarget.string(), ec);
    }

    bool createEmptyFile(const std::string &filePath, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path file_path(filePath);

        // 检查文件是否已存在
        if (fs::exists(file_path, ec))
        {
            ec.clear();
            if (!overwrite)
            {
                // 文件已存在且不允许覆盖
                ec = std::make_error_code(std::errc::file_exists);
                return false;
            }
            // 如果允许覆盖，先删除现有文件
            if (!fs::remove(file_path, ec) || ec)
            {
                return false;
            }
        }

        // 确保父目录存在
        fs::path parent_dir = file_path.parent_path();
        if (!parent_dir.empty() && !fs::exists(parent_dir, ec))
        {
            ec.clear();
            if (!fs::create_directories(parent_dir, ec))
            {
                return false;
            }
        }

        // 创建空文件
        std::ofstream file(file_path.string(), std::ios::binary);
        if (!file.is_open())
        {
            ec = std::make_error_code(std::errc::permission_denied);
            return false;
        }

        // 关闭文件
        file.close();
        return true;
    }

    bool createEmptyDir(const std::string &dirPath, bool overwrite, std::error_code &ec)
    {
        ec.clear();
        fs::path dir_path(dirPath);

        // 检查目标路径是否已存在
        if (fs::exists(dir_path, ec))
        {
            ec.clear();
            // 检查路径类型：若已存在且是文件，返回错误（不能在文件上创建目录）
            if (fs::is_regular_file(dir_path, ec))
            {
                ec = std::make_error_code(std::errc::not_a_directory);
                return false;
            }
            // 若已存在且是目录：根据overwrite决定是否返回成功（true=视为成功，false=返回文件已存在错误）
            if (overwrite)
                return true;
            else
            {
                ec = std::make_error_code(std::errc::file_exists);
                return false;
            }
        }

        // 检查父目录是否存在（仅创建单层目录，父目录必须已存在）
        fs::path parent_dir = dir_path.parent_path();
        if (!parent_dir.empty() && !fs::exists(parent_dir, ec))
        {
            ec = std::make_error_code(std::errc::no_such_file_or_directory);
            return false;
        }

        // 创建单层空目录
        return fs::create_directory(dir_path, ec);
    }

    bool createAny(const std::string &path, bool isCreateDir, bool overwrite, std::error_code &ec)
    {
        ec.clear();

        if (isCreateDir)
        {
            // 创建目录
            return createEmptyDir(path, overwrite, ec);
        }
        else
        {
            // 创建文件
            return createEmptyFile(path, overwrite, ec);
        }
    }

    std::string readFileIntoString(const std::string& path) 
    {
        std::ifstream input_file(path);
        if (!input_file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << input_file.rdbuf();
        std::string contents(buffer.str());
        input_file.close();
        return contents;
    }

    bool saveFileFromString(const std::string &path, const std::string &content)
    {
        std::ofstream output_file(path);
        if (!output_file.is_open())
        {
            return false;
        }

        output_file << content;
        output_file.close();

        return true;
    }

} // namespace FileOperations
