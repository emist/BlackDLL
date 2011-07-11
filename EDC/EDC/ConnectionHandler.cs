using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Runtime;


namespace EDC
{
    class ConnectionHandler
    {
        Socket socket;
        IPAddress host;
        IPEndPoint endpoint;

        public ConnectionHandler(String host, int port)
        {
            this.host = IPAddress.Parse(host);
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            endpoint = new IPEndPoint(this.host, port);
            try
            {
                socket.Connect(endpoint);
            }
            catch (SocketException ex)
            {
                Console.WriteLine(ex.ToString());
                Environment.Exit(1);
            }


        }
        public void Send(byte[] buffer, int offset, int size, int timeout)
        {
            int startTickCount = Environment.TickCount;
            int sent = 0;  // how many bytes is already sent
            int tokens = 0;
            do
            {
                Console.WriteLine("Socket sending");
                if (Environment.TickCount > startTickCount + timeout)
                    throw new Exception("Timeout.");


                try
                {
                    sent += socket.Send(buffer, offset + sent, size - sent, SocketFlags.None);
                }
                catch (SocketException ex)
                {
                    if (ex.SocketErrorCode == SocketError.WouldBlock ||
                        ex.SocketErrorCode == SocketError.IOPending ||
                        ex.SocketErrorCode == SocketError.NoBufferSpaceAvailable)
                    {
                        // socket buffer is probably full, wait and try again
                        Thread.Sleep(30);
                    }
                    else
                        throw ex;  // any serious error occurr
                }
            } while (sent < size);
        }


        public void Receive(byte[] buffer, int offset, int size, int timeout)
        {
            int startTickCount = Environment.TickCount;
            int received = 0;  // how many bytes is already received
            int tokens = 0;
            do
            {
                if (Environment.TickCount > startTickCount + timeout)
                    throw new Exception("Timeout.");
                try
                {
                    int rcvd = socket.Receive(buffer, offset + received, size - received, SocketFlags.None);
                    received += rcvd;
                    if (rcvd == 0)
                    {
                        return;
                    }
                    if (buffer[buffer.Length - 1] == '>')
                        tokens++;
                    if (tokens == 3)
                        return;
                }
                catch (SocketException ex)
                {
                    if (ex.SocketErrorCode == SocketError.WouldBlock ||
                        ex.SocketErrorCode == SocketError.IOPending ||
                        ex.SocketErrorCode == SocketError.NoBufferSpaceAvailable)
                    {
                        // socket buffer is probably empty, wait and try again
                        Thread.Sleep(30);
                    }
                    else
                        throw ex;  // any serious error occurr
                }
            } while (received < size);
        }
    }
}
