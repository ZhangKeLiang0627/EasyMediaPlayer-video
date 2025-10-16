#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <system_error>
#include <vector>

namespace FileOperations
{

    /**
     * 文件类型枚举
     */
    enum class FileType
    {
        FILE = 0,       // 普通文件
        DIRECTORY,      // 目录
        SYMLINK,        // 符号链接
        OTHER,          // 其他类型
        NON_EXIST       // 不存在
    };

    /**
     * 文件信息结构体
     */
    struct FileInfo
    {
        std::string name;       // 名称（不含路径）
        std::string fullPath;   // 完整路径
        FileType type;          // 文件类型
        int64_t size;           // 大小（字节）
        std::string lastModify; // 最后修改时间
    };
    
    /**
     * 移动文件
     * @param source 源文件路径
     * @param destination 目标路径
     * @param overwrite 如果目标文件存在是否覆盖
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    // bool moveFile(const std::string &source, const std::string &destination, std::error_code &ec);
    bool moveFile(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec);

    /**
     * 移动任意类型的文件或目录（文件、空目录、非空目录均可移动）
     * @param source 源文件路径
     * @param destination 目标路径
     * @param overwrite 如果目标文件存在是否覆盖
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool moveAny(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec);

    /**
     * 复制文件
     * @param source 源文件路径
     * @param destination 目标路径
     * @param overwrite 如果目标文件存在是否覆盖
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool copyFile(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec);

    /**
     * 复制任意类型的文件或目录（文件、空目录、非空目录均可复制）
     * @param dirPath 目标目录路径
     * @param overwrite 如果目标文件存在是否覆盖
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool copyAny(const std::string &source, const std::string &destination, bool overwrite, std::error_code &ec);

    /**
     * 获取文件操作的错误信息
     * @param ec 错误码
     * @return 错误信息字符串
     */
    std::string getErrorMessage(const std::error_code &ec);
    
    /**
     * 创建单层目录
     * @param dirPath 目标目录路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool createDir(const std::string &dirPath, std::error_code &ec);

    /**
     * 递归创建多级目录
     * @param dirPath 目标目录路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool createDirRecursive(const std::string &dirPath, std::error_code &ec);

    /**
     * 删除文件
     * @param path 要删除的文件路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool deleteFile(const std::string &path, std::error_code &ec);

    /**
     * 删除空目录
     * @param dirPath 目标目录路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool deleteEmptyDir(const std::string &dirPath, std::error_code &ec);

    /**
     * 递归删除目录（包含所有子内容）
     * @param dirPath 目标目录路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool deleteDirRecursive(const std::string &dirPath, std::error_code &ec);

    /**
     * 删除任意类型的文件或目录（文件、空目录、非空目录均可删除）
     * @param dirPath 目标目录路径
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool deleteAny(const std::string &path, std::error_code &ec);

    /**
     * @brief 删除指定路径下的内容（仅可删除指定根目录内的内容）
     * @param path 要删除的目标路径
     * @param allowedRoot 允许删除操作的根目录（如/mnt/UDISK）
     * @return 删除成功返回true，否则返回false
     */
    bool deleteWithRestrict(std::string &path, const std::string &allowedRoot);

    /**
     * 获取文件类型
     * @param path 文件或目录路径
     * @param ec 错误码输出参数
     * @return 文件类型枚举值
     */
    FileType getFileType(const std::string &path, std::error_code &ec);

    /**
     * 获取文件大小（字节）
     * @param path 文件路径
     * @param ec 错误码输出参数
     * @return 成功返回文件大小，失败返回-1
     */
    int64_t getFileSize(const std::string &path, std::error_code &ec);

    /**
     * 获取最后修改时间
     * @param path 文件或目录路径
     * @param ec 错误码输出参数
     * @return 成功返回格式化时间字符串(YYYY-MM-DD HH:MM:SS)，失败返回空字符串
     */
    std::string getLastModifyTime(const std::string &path, std::error_code &ec);

    /**
     * 检查路径是否存在
     * @param path 目标路径
     * @param ec 错误码输出参数
     * @return 存在返回true，不存在返回false
     */
    bool exists(const std::string &path, std::error_code &ec);

    /**
     * 非递归遍历目录
     * @param dirPath 目录路径
     * @param outFiles 输出文件信息列表
     * @param includeHidden 是否包含隐藏文件
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool listDir(const std::string &dirPath, std::vector<FileInfo> &outFiles, bool includeHidden, std::error_code &ec);

    /**
     * 递归遍历目录
     * @param dirPath 目录路径
     * @param outFiles 输出文件信息列表
     * @param includeHidden 是否包含隐藏文件
     * @param ec 错误码输出参数
     * @return 成功返回true，失败返回false并设置错误码
     */
    bool listDirRecursive(const std::string &dirPath, std::vector<FileInfo> &outFiles, bool includeHidden, std::error_code &ec);

    /**
     * 重命名或移动文件/目录（支持覆盖已存在目标）
     * @param oldPath 源文件/目录的路径（需确保路径有效且目标存在）
     * @param newPath 目标路径（包含新名称或新位置，若为跨目录路径则实现“移动”效果）
     * @param overwrite 若目标路径已存在，是否覆盖（true=覆盖已存在的目标，false=不覆盖并返回错误）
     * @param ec 错误码输出参数（操作失败时会设置具体错误类型，如文件不存在、权限不足等）
     * @return 操作成功返回true；失败返回false（错误信息通过ec输出）
     * @note 1. 支持两种核心场景：同一目录下重命名（如"a.txt"→"b.txt"）、跨目录移动（如"dir1/a.txt"→"dir2/a.txt"）；
     *       2. 会自动创建目标路径的父目录（若父目录不存在）；
     *       3. 若源路径与目标路径指向同一文件/目录（通过文件系统等价性判断），直接返回成功（无实际操作）；
     *       4. 覆盖目标时，会先递归删除已存在的目标路径（确保新路径可写入）。
     */
    bool rename(const std::string &oldPath, const std::string &newPath, bool overwrite, std::error_code &ec);

    /**
     * 创建指定路径的空文件（支持覆盖已存在文件）
     * @param filePath 待创建空文件的完整路径（含文件名，如"docs/temp.txt"）
     * @param overwrite 若目标路径已存在文件，是否覆盖（true=删除原有文件后创建新空文件，false=不覆盖并返回错误）
     * @param ec 错误码输出参数（操作失败时设置具体错误，如权限不足、路径无效等）
     * @return 操作成功返回true；失败返回false（错误信息通过ec输出）
     * @note 1. 自动检查并创建目标文件的父目录（若父目录不存在，如"docs/"不存在时会自动创建）；
     *       2. 仅创建空文件，不写入任何内容（文件大小为0字节）；
     *       3. 若目标路径已存在且为目录（非文件），会返回错误（无法在目录路径上创建文件）；
     *       4. 依赖标准输出流（ofstream）实现，保证跨平台兼容性（Windows/macOS/Linux通用）。
     */
    bool createEmptyFile(const std::string &filePath, bool overwrite, std::error_code &ec);

    /**
     * 创建指定路径的空目录（仅创建单层目录，父目录需已存在）
     * @param dirPath 待创建空目录的完整路径（如"docs/new_dir"）
     * @param overwrite 若目标目录已存在，是否视为“创建成功”（true=已存在时返回成功，false=已存在时返回错误）
     * @param ec 错误码输出参数（操作失败时设置具体错误，如父目录不存在、权限不足等）
     * @return 操作成功返回true；失败返回false（错误信息通过ec输出）
     * @note 1. 仅创建单层目录，若父目录不存在（如"docs/"不存在时创建"docs/new_dir"），会返回错误；
     *       2. 与 createDirRecursive 的区别：本函数不递归创建父目录，仅创建最后一级目录；
     *       3. 若目标路径已存在且为文件（非目录），会返回错误（无法在文件路径上创建目录）。
     */
    bool createEmptyDir(const std::string &dirPath, bool overwrite, std::error_code &ec);

    /**
     * 创建指定路径的空目录/空文件夹
     * @param dirPath 待创建的完整路径（如"docs/new"）
     * @param isCreateDir 为true创建目录 / 为false创建文件
     * @param overwrite 若目标目录已存在，是否视为“创建成功”（true=已存在时返回成功，false=已存在时返回错误）
     * @param ec 错误码输出参数（操作失败时设置具体错误，如父目录不存在、权限不足等）
     * @return 操作成功返回true；失败返回false（错误信息通过ec输出）
     */
    bool createAny(const std::string &path, bool isCreateDir, bool overwrite, std::error_code &ec);

    /**
     * @brief 一次性读取文件所有内容
     * @param path 文件路径
     * @return std::string 内容
     */
    std::string readFileIntoString(const std::string &path);

    /**
     * @brief 保存内容置指定文件
     * @param path 文件路径
     * @param content 内容
     * @return bool 是否成功
     */
    bool saveFileFromString(const std::string &path, const std::string &content);

} // namespace FileOperations

#endif // FILE_OPERATIONS_H
