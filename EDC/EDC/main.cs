// minimalistic telnet implementation
// conceived by Tom Janssens on 2007/06/06  for codeproject
//
// http://www.corebvba.be

using System;
using System.Collections.Generic;
using System.Text;


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
                // display server output
                Console.Write(tc.Read());

                // send client input to server
                prompt = Console.ReadLine();
                tc.WriteLine(prompt);

                // display server output
                Console.Write(tc.Read());
            }
            Console.WriteLine("***DISCONNECTED");
   
        }
    }
}
