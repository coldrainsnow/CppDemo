#include <fstream>
#include <string>

int main() {
	std::ifstream inFile("name.txt");
	std::ofstream outFile("temp.txt");
	std::string line;
	bool found = false;

	while (std::getline(inFile, line)) {
		if (!found) {
			size_t pos = line.find("2829");
			if (pos != std::string::npos) {
				outFile << line.substr(pos);
				found = true;
			}
		}
		else {
			outFile << line << '\n';
		}
	}

	inFile.close();
	outFile.close();

	// 删除原文件并将临时文件重命名为原文件
	remove("yourfile.txt");
	rename("temp.txt", "yourfile.txt");

	return 0;
}
