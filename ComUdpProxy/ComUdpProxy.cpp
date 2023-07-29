// ComUdpProxy.cpp : Defines the entry point for the application.
//

#include "ComUdpProxy.h"
#include "Sender.h"
#include <boost/lexical_cast.hpp>
using namespace std;

int main(int argc, char* argv[])
{
	cout << "Hello ComUdpProxy" << endl;

	for (int i = 1; i < argc; i++)
	{
		cout << argv[i] << endl;
	}

  try
  {
    if (argc < 3)
    {
      std::cerr << "Usage: ComUdpProxy <comport> <multicast_address>\n";
      return 1;
    }

    boost::asio::io_service io_service;
    std::string broadcastAddr = boost::asio::ip::address_v4::broadcast().to_string();
    sender s(
      io_service,
      boost::asio::ip::address::from_string(broadcastAddr),
      boost::lexical_cast<short>(argv[2])
    );
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

	return 0;
}
