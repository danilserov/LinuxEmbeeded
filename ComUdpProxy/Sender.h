

//
// sender.h
// ~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <sstream>
#include <string>
#include <deque>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

const int max_message_count = 10;

class sender
{

private:
  boost::thread thread_;
  boost::mutex mutex_;
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer timer_;
  std::deque<std::string> deque_;
  boost::asio::io_service& io_service_;
  std::string stringBuf_;
  bool stopRequest_;
public:
  sender(
    boost::asio::io_service& io_service,
    const boost::asio::ip::address& multicast_address,
    short multicast_port
  ) :
    io_service_(io_service),
    endpoint_(multicast_address, multicast_port),
    socket_(io_service, endpoint_.protocol()),
    timer_(io_service)
  {
    stopRequest_ = false;
    socket_.set_option(boost::asio::socket_base::broadcast(true));    
    thread_ = boost::thread([this] { this->thread_Process(); });
  }

  virtual ~sender()
  {
    thread_.interrupt();
    thread_.join();
  }

  void WaitForComplete()
  {
    while (true)
    {
      {
        boost::mutex::scoped_lock l(mutex_);

        if (deque_.empty() && stringBuf_.empty())
        {
          break;
        }
      }

      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
    stopRequest_ = true;
  }

  void send(const std::string& message)
  {
    {
      boost::mutex::scoped_lock l(mutex_);
      deque_.push_back(message);
    }
  }

private:
  void thread_Process()
  {
    while (!stopRequest_)
    {
      {
        boost::mutex::scoped_lock l(mutex_);

        if (deque_.empty())
        {
          return;
        }
        stringBuf_ = deque_.front();
        deque_.pop_front();
      }

      socket_.async_send_to(
        boost::asio::buffer(stringBuf_), endpoint_,
        boost::bind(&sender::handle_send_to, this,
          boost::asio::placeholders::error));

      io_service_.run_one();
    }
  }

  void handle_send_to(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout << "sent:" << stringBuf_ << std::endl;
    }
    stringBuf_.clear();
  }
};