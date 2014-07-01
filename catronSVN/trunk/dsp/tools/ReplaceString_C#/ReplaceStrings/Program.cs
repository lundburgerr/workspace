using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace ReplaceStrings
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 3)
            {
                Console.WriteLine("wrong number of arguments");
                return;
            }

            if (args.Length == 3)
            {
                string srcFile = args[0];
                StreamReader streamReader = new StreamReader(srcFile, true);
                StringBuilder text = new StringBuilder(streamReader.ReadToEnd());

                text.Replace(args[1], args[2]);
                Encoding e = new System.Text.UTF8Encoding(false);

                streamReader.Close();

                System.IO.File.WriteAllText(srcFile, text.ToString(), e);

            }

            else if (args.Length >= 4)
            {
                string destFile = args[0];
                string srcFile = args[1];
                string postfix = args[2];

                StreamReader streamReader = new StreamReader(srcFile, true);
                StringBuilder text = new StringBuilder(streamReader.ReadToEnd());

                for (int i = 3; i < args.Length; i++)
                {
                    String newString = args[i] + postfix;
                    text.Replace(args[i], newString);
                }

                Encoding e = new System.Text.UTF8Encoding(false);

                streamReader.Close();

                System.IO.File.WriteAllText(destFile, text.ToString(), e);

            }
        }
    }
}
