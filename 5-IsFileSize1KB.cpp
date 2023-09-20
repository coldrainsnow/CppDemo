#include <fstream>

bool isFileSize1KB(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.close();
        // 将字节转换为千字节
        double sizeInKB = size / 1024.0;
        // 判断文件大小是否为1KB
        if (sizeInKB == 1.0) {
            return true;
        }
    }
    return false;
}
