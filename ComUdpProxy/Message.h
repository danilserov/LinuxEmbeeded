#pragma once

#include<vector>
#include<string>

class Message
{
  std::string m_message;
public:
  Message(const std::string& str) :
    m_message(str)
  {

  }
  std::string encrypt()
  {
    std::string retVal = { 'H','E','L','L','O' };

    for (auto it : m_message)
    {
      retVal.push_back(it + 1);
    }

    return retVal;
  }
};