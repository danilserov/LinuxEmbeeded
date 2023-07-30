

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
#include "Message.h"

class sender
{
private:
  boost::thread m_thread;
  boost::mutex m_mutex;
  boost::asio::ip::udp::endpoint m_endpoint;
  boost::asio::ip::udp::socket m_socket;

  std::deque<std::string> m_deque;
  boost::asio::io_service& m_io_service;
  bool m_stopRequest;
  boost::condition_variable m_condition;
  boost::condition_variable m_queueEmptyCondition;
public:
  sender(
    boost::asio::io_service& io_service,
    const boost::asio::ip::address& multicast_address,
    short multicast_port
  ) :
    m_io_service(io_service),
    m_endpoint(multicast_address, multicast_port),
    m_socket(io_service, m_endpoint.protocol())
  {
    m_stopRequest = false;
    m_socket.set_option(boost::asio::socket_base::broadcast(true));
    m_thread = boost::thread([this] { this->thread_Process(); });
  }

  virtual ~sender()
  {   
    
  }

  void WaitForComplete()
  {
    // to be called in the very end when send is never used anymore.
    while (true)
    {
      {
        boost::mutex::scoped_lock l(m_mutex);

        if (m_deque.empty())
        {
          break;
        }
      }
      boost::mutex queueEmptyMutex;
      boost::mutex::scoped_lock lock(queueEmptyMutex);
      m_queueEmptyCondition.timed_wait(lock, boost::posix_time::seconds(1));
    }
    m_stopRequest = true;
   // m_thread.interrupt();
    m_thread.join();
  }

  void send(const std::string& message)
  {
    {
      boost::mutex::scoped_lock l(m_mutex);
      m_deque.push_back(message);
    }
    m_condition.notify_all();
  }

private:
  void thread_Process()
  {
    while (m_stopRequest == false)
    {
      std::string buf;
      {
        boost::mutex::scoped_lock lock(m_mutex);

        if (m_deque.empty())
        {
          m_condition.timed_wait(lock, boost::posix_time::seconds(1));
          m_queueEmptyCondition.notify_all();
          continue;
        }
        buf = m_deque.front();
        m_deque.pop_front();
      }
      Message m(buf);
      const std::string& bufToSend = m.encrypt();
      m_socket.send_to(
        boost::asio::buffer(bufToSend),
        m_endpoint
      );

      //std::cout << "sent to udp:" << bufToSend << std::endl;
    }
    m_socket.close();
  }
};