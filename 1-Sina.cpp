#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h> // libcurl库
#include <htmlcxx/html/ParserDom.h> // htmlcxx库
#include <boost/asio.hpp> // boost库

using namespace std;
using namespace htmlcxx;

// 回调函数，将HTTP响应写入字符串
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}

// 爬取新浪财经网页，并返回开盘价
double get_open_price()
{
    // 创建libcurl对象
    CURL *curl = curl_easy_init();
    // 设置URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://finance.sina.com.cn/realstock/company/sh000300/nc.shtml");
    // 设置回调函数和输出流
    stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return -1;
    }
	// 释放libcurl对象
    curl_easy_cleanup(curl);
    // 解析网页源代码
    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(out.str());
    // 查找开盘价元素
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    double open_price = 0;
    for (; it != end; ++it)
    {
        if (it->tagName() == "div" && it->attribute("class").second == "stock-current-price")
        {
            // 获取开盘价文本
            string text = it->text();
            // 去除空白字符和单位
            text.erase(remove_if(text.begin(), text.end(), ::isspace), text.end());
            text.erase(remove(text.begin(), text.end(), '元'), text.end());
            // 转换为double类型
			open_price = stod(text);
            break;
        }
    }
    return open_price;
}

// 将开盘价保存到文件中
void save_to_file(double price)
{
	// 打开文件
    ofstream file("open_price.txt", ios::app);
    if (!file.is_open())
    {
        cerr << "Failed to open file" << endl;
        return;
    }
    // 获取当前日期和时间
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // 写入文件
    file << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " "
         << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << " "
         << price << endl;
    // 关闭文件
    file.close();
}

// 定时执行爬虫程序
void run_crawler(boost::asio::io_context& io)
{
	// 获取当前时间
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // 计算下一次执行时间（早上9点25分）
    tm next;
    next.tm_year = ltm->tm_year;
    next.tm_mon = ltm->tm_mon;
    next.tm_mday = ltm->tm_mday + (ltm->tm_hour >= 9 && ltm->tm_min >= 25); // 如果已经过了今天的9点25分，就设为明天
    next.tm_hour = 9;
    next.tm_min = 25;
    next.tm_sec = 0;
    // 转换为time_t类型
    time_t t_next = mktime(&next);
    // 计算等待秒数
    int seconds = difftime(t_next, now);
    // 创建定时器对象
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(seconds));
	// 设置定时器回调函数
    timer.async_wait([&io](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            // 爬取开盘价
            double price = get_open_price();
            cout << "Open price: " << price << endl;
            // 保存到文件中
            save_to_file(price);
            // 重新执行爬虫程序
            run_crawler(io);
        }
    });
}

int main()
{
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    // 创建io_context对象
    boost::asio::io_context io;
	// 执行爬虫程序
    run_crawler(io);
    // 运行io_context对象
    io.run();
    // 清理libcurl
    curl_global_cleanup();
    return 0;
}