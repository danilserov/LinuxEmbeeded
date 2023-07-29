#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

class receiver
{
private:
  boost::asio::io_service m_io;
  std::string m_port;
  boost::asio::serial_port m_serial;
  int m_counter;
  std::chrono::time_point<std::chrono::system_clock> m_timePoint;
public:
  receiver(std::string port):
    m_port(port),
    m_serial(m_io, m_port),
    m_counter(0)
  {
    // Configure basic serial port parameters: 8kBaud, 8N1
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(8000));
    m_serial.set_option(boost::asio::serial_port_base::character_size(8 /* data bits */));
    m_serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    m_serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
  }

  virtual ~receiver()
  {
    m_serial.close();
  }
  //  PS C : \Windows\system32 > $port = new-Object System.IO.Ports.SerialPort COM5, 8000, None, 8, one
  //  PS C : \Windows\system32 > $port.open()
  //  PS C : \Windows\system32 > $port.WriteLine("some string")
  //  PS C : \Windows\system32 > $port.WriteLine("some string1")
  void receive(std::function <void(std::string line)> func)
  {
    std::vector<char> buf(1024, 0);
    std::string line;
    resetTimer();

    while (m_counter < 10)
    {
      m_serial.async_read_some(boost::asio::buffer(buf, buf.size()),
        [&](
          const boost::system::error_code& error, // Result of operation.
          std::size_t bytes_transferred           // Number of bytes read.
          )
        {
          resetTimer();

          for (int i = 0; i < bytes_transferred; i++)
          {
            if (buf[i] == '\n')
            {
              lineReceived(line, func);
              break;
            }
            line += buf[i];
          }

          std::fill(buf.begin(), buf.begin() + bytes_transferred, 0);
        });


      m_io.run_one_until(m_timePoint);

      if (std::chrono::system_clock::now() >= m_timePoint)
      {
        resetTimer();
        lineReceived(line, func);
      }
    }
  }

private:

  void lineReceived(std::string& line, std::function <void(std::string line)> func)
  { 
    if (line.empty())
    {
      return;
    }
    std::cout << "line received:" << line << std::endl;
    func(line);
    line.clear();
    m_counter++;
  }

  void resetTimer()
  {
    m_timePoint = std::chrono::system_clock::now() + std::chrono::seconds(5);
  }  
};
