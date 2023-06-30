#include <iostream>
#include <string>
#include <curl/curl.h>

std::string readBuffer;

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{
	readBuffer.append(buf, size * nmemb);
	return size * nmemb;
}

int main()
{
	CURL* curl;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://bbs.huaweicloud.com/community/usersnew/id_1685630595458769");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		curl_easy_cleanup(curl);

		std::string searchStr = "<span class=\"ucd-pager-count\">";
		size_t pos = readBuffer.find(searchStr);
		if (pos != std::string::npos) {
			pos += searchStr.length();
			size_t endPos = readBuffer.find("</span>", pos);
			if (endPos != std::string::npos) {
				std::cout << readBuffer.substr(pos, endPos - pos) << std::endl;
			}
		}
	}

	return 0;
}
