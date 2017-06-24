// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

//#define ENABLE_LOGGING

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Indexer
{
    public class Log : IDisposable
    {
        private readonly StreamWriter sw = null;
        private SingleThreadTimer saveTimer;

        private List<string> messages = new List<string>();

        private Log()
        {
#if ENABLE_LOGGING
            if (!Directory.Exists("Logs"))
                Directory.CreateDirectory("Logs");

            var filename = @"Logs/" + "IndexerUILog_" + DateTime.Now.Ticks + ".txt";

            sw = File.AppendText(filename);

            Task.Factory.StartNew(RunSaveTimer);
#endif
        }

        private static Log instance;
        public static Log Instance => instance ?? (instance = new Log());


        public void Debug(string message)
        {
#if ENABLE_LOGGING
            messages.Add("Debug: " + DateTime.Now + " " + message);
#endif
        }

        public void Error(string message)
        {
#if ENABLE_LOGGING
            messages.Add("Error: " + DateTime.Now + " " + message);
#endif
        }

        private void RunSaveTimer()
        {
            saveTimer = new SingleThreadTimer(TimeSpan.FromMilliseconds(100), Save);
            saveTimer.Start();
        }

        private void Save()
        {
            var tmpStorage = Interlocked.Exchange(ref messages, new List<string>());

            foreach (var msg in tmpStorage)
                sw.WriteLine(msg);

            sw.Flush();
        }

        public void Dispose()
        {
#if ENABLE_LOGGING
            sw.Flush();
            sw.Dispose();
#endif
        }
    }
}