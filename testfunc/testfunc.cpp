// testfunc.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


using namespace boost::asio;
using std::cout;
using std::endl;
io_service ioservice;
io_service::strand strand1(ioservice);
ip::tcp::socket sock(ioservice);
void call_handler(const boost::system::error_code ec)
{
	cout << "error coode " << ec << endl;
	cout << "start send" << endl;
	std::string msg;
	while (std::getline(std::cin,msg))
	{
		cout << "send " << msg << endl;
		sock.send(buffer(msg));
	}
}

void call1() { cout << "call1 " << boost::this_thread::get_id() << endl; }
void call2() { cout << "call2 " << boost::this_thread::get_id() << endl; }
void call3() { cout << "call3 " << boost::this_thread::get_id() << endl; }
void call4() { cout << "call4 " << boost::this_thread::get_id() << endl; }
void call5() { cout << "call5 " << boost::this_thread::get_id() << endl; }
void call_all(int i) { cout << i << " call " << boost::this_thread::get_id() << endl; }
void worker_thread() { ioservice.run(); }
int main()
{

	/*ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"),8001);
	cout << ip::host_name()<<endl;

	sock.async_connect(ep, call_handler);
	ioservice.run();*/
	ioservice.post(strand1.wrap(std::bind(call1)));
	ioservice.post(strand1.wrap(std::bind(call2)));
	ioservice.post(strand1.wrap(std::bind(call3)));
	ioservice.post(strand1.wrap(std::bind(call4)));
	ioservice.post(strand1.wrap(std::bind(call5)));
	for(int i=0;i<10;i++)
		ioservice.post(strand1.wrap(std::bind(call_all,i)));

	cout << boost::this_thread::get_id() << endl;
	boost::thread_group threads;
	for (int i = 0; i < 5; i++)
	{
		threads.create_thread(worker_thread);
	}
	threads.join_all();
	system("pause");
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
