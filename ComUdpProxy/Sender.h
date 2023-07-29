

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
  bool stopRequest_;
  boost::condition_variable m_condition;
  int m_counter;

public:
  sender(
    boost::asio::io_service& io_service,
    const boost::asio::ip::address& multicast_address,
    short multicast_port
  ):
    io_service_(io_service),
    endpoint_(multicast_address, multicast_port),
    socket_(io_service, endpoint_.protocol()),
    timer_(io_service)
  {
    m_counter = 0;
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

        if (deque_.empty())
        {
          break;
        }
      }

      boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
    stopRequest_ = true;
  }

  void send(const std::string message)
  {
    {
      boost::mutex::scoped_lock l(mutex_);
      deque_.push_back(message);
    }
    m_condition.notify_all();
  }

private:
  void thread_Process()
  {
    while (stopRequest_ == false)
    {
      std::string buf;
      {
        boost::mutex::scoped_lock lock(mutex_);

        if (deque_.empty())
        {
          m_condition.timed_wait(lock, boost::posix_time::seconds(1));
          continue;
        }
        buf = deque_.front();
        deque_.pop_front();
      }

      socket_.send_to(
        boost::asio::buffer(buf),
        endpoint_
      );
    }
  }
};