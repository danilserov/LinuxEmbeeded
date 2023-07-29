#pragma once

#include<vector>
#include<string>
#include<string_view>
class Message
{
private:
  std::string m_message;
  const char* HEADER = "HELLO";
  constexpr std::size_t constexpr_strlen(const char* s)
  {
    return std::char_traits<char>::length(s);
  }
public:
  Message(const std::string& str)
  {
    m_message = HEADER + str;
  }

  const std::string& encrypt()
  {
    auto len = constexpr_strlen(HEADER);

    for (auto it = m_message.begin() + len; it != m_message.end(); it++)
    {
      (*it)++;
    }

    return m_message;
  }
};