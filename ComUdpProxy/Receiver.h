#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <coroutine>

#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

template <typename T>
struct Receiver
{
  // The class name 'Receiver' is our choice and it is not required for coroutine
  // magic. Compiler recognizes coroutine by the presence of 'co_yield' keyword.
  // You can use name 'MyReceiver' (or any other name) instead as long as you include
  // nested struct promise_type with 'MyReceiver get_return_object()' method.

  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  struct promise_type // required
  {
    T value_;
    std::exception_ptr exception_;

    Receiver get_return_object()
    {
      return Receiver(handle_type::from_promise(*this));
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { exception_ = std::current_exception(); } // saving
    // exception

    template <std::convertible_to<T> From> // C++20 concept
    std::suspend_always yield_value(From&& from)
    {
      value_ = std::forward<From>(from); // caching the result in promise
      return {};
    }
    void return_void() { }
  };

  handle_type h_;

  Receiver(handle_type h)
    : h_(h)
  {
  }
  ~Receiver() { h_.destroy(); }
  explicit operator bool()
  {
    fill(); // The only way to reliably find out whether or not we finished coroutine,
    // whether or not there is going to be a next value generated (co_yield)
    // in coroutine via C++ getter (operator () below) is to execute/resume
    // coroutine until the next co_yield point (or let it fall off end).
    // Then we store/cache result in promise to allow getter (operator() below
    // to grab it without executing coroutine).
    return !h_.done();
  }
  T operator()()
  {
    fill();
    full_ = false; // we are going to move out previously cached
    // result to make promise empty again
    return std::move(h_.promise().value_);
  }

private:
  bool full_ = false;

  void fill()
  {
    if (!full_)
    {
      h_();
      if (h_.promise().exception_)
        std::rethrow_exception(h_.promise().exception_);
      // propagate coroutine exception in called context

      full_ = true;
    }
  }
};

/*
Powershell.exe -File runComTest.ps1
$port = new-Object System.IO.Ports.SerialPort COM5, 8000, None, 8, one
$port.open()
$port.WriteLine("some string")
$port.WriteLine("some string1")
*/

Receiver<std::string>  receiver(int maxLines, std::string port)
{
  boost::asio::io_service io;
  boost::asio::serial_port serial(io, port);
  std::chrono::time_point<std::chrono::system_clock> timePoint;
  // Configure basic serial port parameters: 8kBaud, 8N1
  serial.set_option(boost::asio::serial_port_base::baud_rate(8000));
  serial.set_option(boost::asio::serial_port_base::character_size(8 /* data bits */));
  serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
  serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

  std::vector<char> buf(1024, 0);
  std::string line;
  int lineCounter = 0;

  auto resetTimer = [&]() -> void 
  { 
    timePoint = std::chrono::system_clock::now() + std::chrono::seconds(5);
  };

  auto lineReceived = [](
    std::string& resLine,
    int& lineCounter
  ) -> void  
    {
      if (!resLine.empty())
      {
        lineCounter++;
        std::cout << "line:" << lineCounter << " received:" << resLine << std::endl;      
      }    
    };

  while (lineCounter < maxLines)
  {
    std::vector<std::string> results;

    serial.async_read_some(boost::asio::buffer(buf, buf.size()),
    [&](
      const boost::system::error_code& error, // Result of operation.
      std::size_t bytes_transferred           // Number of bytes read.
    )
    {
      for (int i = 0; i < bytes_transferred; i++)
      {
        if (buf[i] == '\n')
        {
          if (!line.empty())
          {
            results.push_back(line);
            line.clear();
          }
          continue;
        }
        line += buf[i];
      }

      std::fill(buf.begin(), buf.begin() + bytes_transferred, 0);
    });


    io.run_one_until(timePoint);

    if (std::chrono::system_clock::now() >= timePoint)
    {
      if (!line.empty())
      {
        results.push_back(line);
        line.clear();
      }
    }

    for (auto res : results)
    {
      lineReceived(res, lineCounter);
      co_yield line;      
    }
    resetTimer();
  }
  serial.close();
  co_return;
}