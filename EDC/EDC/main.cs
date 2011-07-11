// minimalistic telnet implementation
// conceived by Tom Janssens on 2007/06/06  for codeproject
//
// http://www.corebvba.be

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;


namespace MinimalisticTelnet
{
    class main
    {
        static void Main(string[] args)
        {
            
            TelnetConnection tc = new TelnetConnection("localhost", 7777);
            String prompt = "";

            while (tc.IsConnected && prompt.Trim() != "exit" )
            {
                int i = 0;
                
                String serveroutput = tc.Read();
                while (serveroutput.Length < 1)
                {
                    Thread.Sleep(50);
                    serveroutput = tc.Read();
                    i++;
                }
                Console.Write(serveroutput);
                serveroutput = "";

                // send client input to server
                prompt = Console.ReadLine();
                
                tc.WriteLine(prompt);                
                
            }
            Console.WriteLine("***DISCONNECTED");
   
        }
    }
}
