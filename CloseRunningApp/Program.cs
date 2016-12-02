// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;

// Closes an Indexer++ program via named pipes.

namespace CloseRunningApp
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            var pipeName = GetNamePrefix();
            int connectionTimeoutMs = 500;

            try
            {
                using (var client = new NamedPipeClientStream(pipeName))
                {
                    try
                    {
                        client.Connect(connectionTimeoutMs);
                    }
                    catch (TimeoutException e)
                    {
                        Console.WriteLine(connectionTimeoutMs + " ms waiting server connection timeout has passed. Exiting.");
                        return;
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine("En error occurred while trying to connect to pipe.");
                        return;
                    }

                    using (var writer = new StreamWriter(client))
                    {
                        writer.WriteLine("Exit");
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("En error occurred while creating a pipe.");
                return;
            }
        }

        // The same method and the same hard-coded pipe name as in IndexerGUI.Helper.
        static string GetNamePrefix()
        {
            var userName = WindowsIdentity.GetCurrent().Name.Replace("\\", "-");
            return @"Local\" + userName + "-Indexer++Pipe";
        }
    }
}
