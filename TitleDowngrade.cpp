#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
	std::regex pattern(R"(^(#+)(\s\d+))");
	std::string replacement = "$1#$2";

	for (const auto& entry : fs::directory_iterator(fs::current_path())) {
		if (entry.is_regular_file() && entry.path().extension() == ".md") {
			std::ifstream input(entry.path());
			std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
			input.close();

			content = std::regex_replace(content, pattern, replacement);

			std::ofstream output(entry.path());
			output << content;
			output.close();
		}
	}

	return 0;
}
