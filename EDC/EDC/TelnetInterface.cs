// minimalistic telnet implementation
// conceived by Tom Janssens on 2007/06/06  for codeproject
//
// http://www.corebvba.be



using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.IO;

namespace MinimalisticTelnet
{
    enum Verbs {
        WILL = 251,
        WONT = 252,
        DO = 253,
        DONT = 254,
        IAC = 255
    }

    enum Options
    {
        SGA = 3
    }

    public class TelnetConnection
    {
        TcpClient tcpSocket;
        StreamWriter sw;
        StreamReader sr;

        int TimeOutMs = 10;

        public TelnetConnection(string Hostname, int Port)
        {

            tcpSocket = new TcpClient(Hostname, Port);
            sw = new StreamWriter(tcpSocket.GetStream());
            sr = new StreamReader(tcpSocket.GetStream());
        }

        public string Login(string Username,string Password,int LoginTimeOutMs)
        {
            int oldTimeOutMs = TimeOutMs;
            TimeOutMs = LoginTimeOutMs;
            string s = Read();
            if (!s.TrimEnd().EndsWith(":"))
               throw new Exception("Failed to connect : no login prompt");
            WriteLine(Username);

            s += Read();
            if (!s.TrimEnd().EndsWith(":"))
                throw new Exception("Failed to connect : no password prompt");
            WriteLine(Password);

            s += Read();
            TimeOutMs = oldTimeOutMs;
            return s;
        }

        public void WriteLine(string cmd)
        {
            //Write(cmd + "\n");
            sw.WriteLine(cmd);
            sw.Flush();
        }

        public void Write(string cmd)
        {
            if (!tcpSocket.Connected) return;
            
            byte[] buf = System.Text.ASCIIEncoding.ASCII.GetBytes(cmd.Replace("\0xFF","\0xFF\0xFF"));
            tcpSocket.GetStream().Write(buf, 0, buf.Length);
            

            
        }

        public string Read()
        {
            if (!tcpSocket.Connected) return null;
            StringBuilder sb=new StringBuilder();
            do
            {
                ParseTelnet(sb);
                System.Threading.Thread.Sleep(TimeOutMs);
            } while (tcpSocket.Available > 0);
            return sb.ToString();
        }

        public bool IsConnected
        {
            get { return tcpSocket.Connected; }
        }

        void ParseTelnet(StringBuilder sb)
        {
            while (tcpSocket.Available > 0)
            {

                char input = (char)sr.Read();
                sb.Append( input );
                
                
            }
            
        }
    }
}
