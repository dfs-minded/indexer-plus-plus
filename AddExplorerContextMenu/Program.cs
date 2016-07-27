// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.IO;

namespace AddExplorerContextMenu
{
    static class Program
    {
        // to register pass: /r "full path to Indexer.exe"
        // to unregister pass: /u
        [STAThread]
        static void Main(string[] args)
        {
            string location = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string iconPath = new FileInfo(location).Directory.FullName + @"\icon_v3_2.ico";

            var manager = new RegistryManager();
            
            if (args == null || args.Length < 1)
            {
                Log(@"Error: to unregister pass: /u,  to register pass: /r ""full path to Indexer++.exe""");
                return;
            }

            switch (args[0])
            {
                case @"/u":
                    try
                    {
                        manager.Unregister();
                    }
                    catch (Exception ex)
                    {
                        Log(@"Error occurred while Unregistering: " + ex.Message);
                    }
                    break;
                case @"/r":
                    string exePath = args[1];
                    if (!File.Exists(exePath)) Log(@"Error: Could not find file by given path:" + exePath);
                    try
                    {
                        manager.Register(exePath, iconPath);
                    }
                    catch (Exception ex)
                    {
                        Log(@"Error occurred while Unregistering: " + ex.Message);
                    }
                    break;
                default:
                    Log(@"Error: args[0] = /r or /u to register or unregister");
                    break;

            }
        }

        private static void Log(string logMessage)
        {
            using (StreamWriter w = File.AppendText("AddExplorerContextMenuErrorLog.txt"))
            {
                w.WriteLine(logMessage);
            }
        }
    }
}
