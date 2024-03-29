﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System;
using System.Net;
using System.Net.Sockets;

namespace UdpListener
{
  public class UDPListener1
  {
    private const int listenPort = 30001;

    private static void StartListener()
    {
      UdpClient listener = new UdpClient(listenPort);
      IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, listenPort);

      try
      {
        while (true)
        {
          Console.WriteLine("Waiting for broadcast");
          byte[] bytes = listener.Receive(ref groupEP);

          Console.WriteLine($"Received broadcast from {groupEP} :");
          Console.WriteLine($" {Encoding.ASCII.GetString(bytes, 0, bytes.Length)}");
        }
      }
      catch (SocketException e)
      {
        Console.WriteLine(e);
      }
      finally
      {
        listener.Close();
      }
    }

    public static void Run()
    {
      StartListener();
    }
  }
}