// testfunc2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>

using namespace boost::asio;
using std::cout;
typedef std::shared_ptr<ip::tcp::socket> socket_ptr;
class Connection;
typedef std::shared_ptr<Connection> connection_ptr;
std::vector<connection_ptr> cons;
io_service service;

class Connection:public std::enable_shared_from_this<Connection>,boost::noncopyable
{
private:
	ip::tcp::socket sock_;
	enum { max_msg = 1024 };
	char read_buffer_[max_msg];
	char write_buffer_[max_msg];
	bool started_;
	std::string username_;
	deadline_timer timer_;
	boost::posix_time::ptime last_ping_;
	bool clients_changed_;
private:
	Connection():sock_(service), started_(false), timer_(service), clients_changed_(false) {};
public:
	typedef boost::system::error_code error_code;

	static connection_ptr new_() {
		connection_ptr new_(new Connection);
		return new_;
	}

	void start()
	{
		cons.push_back(shared_from_this());
		started_ = true;
		do_read();
	}

	void stop()
	{
		if (!started_) return;
		started_ = false;
		sock_.close();
		connection_ptr self = shared_from_this();
		std::vector<connection_ptr>::iterator it = std::find(cons.begin(), cons.end(), self);
		cons.erase(it);
	}

	bool started() const {
		return started_;
	}

	ip::tcp::socket & sock() { return sock_; }
private:
	void do_read()
	{
		sock_.async_read_some(buffer(read_buffer_), std::bind(&Connection::on_read, shared_from_this(),std::placeholders::_1,std::placeholders::_2));
	}

	void on_read(const boost::system::error_code & err, size_t bytes) {
		if (err) {
			//发生错误或对方关闭
			stop();
			return;
		}
		if (!started()) return;

		std::string msg(read_buffer_, bytes);
		cout << "###  " << msg << std::endl;
		do_read();
	}
};

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));

void handle_accept(connection_ptr client, const boost::system::error_code & err) {
	client->start();
	connection_ptr new_client = Connection::new_();
	acceptor.async_accept(new_client->sock(), boost::bind(handle_accept, new_client, _1));
}

int main()
{
	connection_ptr new_client = Connection::new_();
	acceptor.async_accept(new_client->sock(), boost::bind(handle_accept, new_client, _1));
	service.run();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
