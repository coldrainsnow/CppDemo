#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h> // libcurl��
#include <htmlcxx/html/ParserDom.h> // htmlcxx��
#include <boost/asio.hpp> // boost��

using namespace std;
using namespace htmlcxx;

// �ص���������HTTP��Ӧд���ַ���
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}

// ��ȡ���˲ƾ���ҳ�������ؿ��̼�
double get_open_price()
{
    // ����libcurl����
    CURL *curl = curl_easy_init();
    // ����URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://finance.sina.com.cn/realstock/company/sh000300/nc.shtml");
    // ���ûص������������
    stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    // ִ������
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return -1;
    }
	// �ͷ�libcurl����
    curl_easy_cleanup(curl);
    // ������ҳԴ����
    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(out.str());
    // ���ҿ��̼�Ԫ��
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    double open_price = 0;
    for (; it != end; ++it)
    {
        if (it->tagName() == "div" && it->attribute("class").second == "stock-current-price")
        {
            // ��ȡ���̼��ı�
            string text = it->text();
            // ȥ���հ��ַ��͵�λ
            text.erase(remove_if(text.begin(), text.end(), ::isspace), text.end());
            text.erase(remove(text.begin(), text.end(), 'Ԫ'), text.end());
            // ת��Ϊdouble����
			open_price = stod(text);
            break;
        }
    }
    return open_price;
}

// �����̼۱��浽�ļ���
void save_to_file(double price)
{
	// ���ļ�
    ofstream file("open_price.txt", ios::app);
    if (!file.is_open())
    {
        cerr << "Failed to open file" << endl;
        return;
    }
    // ��ȡ��ǰ���ں�ʱ��
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // д���ļ�
    file << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday << " "
         << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << " "
         << price << endl;
    // �ر��ļ�
    file.close();
}

// ��ʱִ���������
void run_crawler(boost::asio::io_context& io)
{
	// ��ȡ��ǰʱ��
    time_t now = time(0);
    tm *ltm = localtime(&now);
    // ������һ��ִ��ʱ�䣨����9��25�֣�
    tm next;
    next.tm_year = ltm->tm_year;
    next.tm_mon = ltm->tm_mon;
    next.tm_mday = ltm->tm_mday + (ltm->tm_hour >= 9 && ltm->tm_min >= 25); // ����Ѿ����˽����9��25�֣�����Ϊ����
    next.tm_hour = 9;
    next.tm_min = 25;
    next.tm_sec = 0;
    // ת��Ϊtime_t����
    time_t t_next = mktime(&next);
    // ����ȴ�����
    int seconds = difftime(t_next, now);
    // ������ʱ������
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(seconds));
	// ���ö�ʱ���ص�����
    timer.async_wait([&io](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            // ��ȡ���̼�
            double price = get_open_price();
            cout << "Open price: " << price << endl;
            // ���浽�ļ���
            save_to_file(price);
            // ����ִ���������
            run_crawler(io);
        }
    });
}

int main()
{
    // ��ʼ��libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    // ����io_context����
    boost::asio::io_context io;
	// ִ���������
    run_crawler(io);
    // ����io_context����
    io.run();
    // ����libcurl
    curl_global_cleanup();
    return 0;
}