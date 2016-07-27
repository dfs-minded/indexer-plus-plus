// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Indexer
{
    public class Log : IDisposable
    {
        private readonly string filename = "IndexerUILog.txt";
        private readonly StreamWriter sw;
        private SingleThreadTimer saveTimer;

        private List<string> messages = new List<string>();

        private Log()
        {
            sw = new StreamWriter(filename);
            Task.Factory.StartNew(RunSaveTimer);
        }

        private static Log instance;

        public static Log Instance
        {
            get { return instance ?? (instance = new Log()); }
        }


        public void Debug(string message)
        {
            messages.Add("Debug: " + DateTime.Now + " " + message);
        }

        public void Error(string message)
        {
            messages.Add("Error: " + DateTime.Now + " " + message);
        }

        private void RunSaveTimer()
        {
            saveTimer = new SingleThreadTimer(TimeSpan.FromMilliseconds(100), Save);
            saveTimer.Start();
        }

        private void Save()
        {
            var tmpStorage = Interlocked.Exchange(ref messages, new List<string>());

            for (var i = 0; i < tmpStorage.Count; ++i)
                sw.WriteLine(tmpStorage[i]);

            sw.Flush();
        }

        public void Dispose()
        {
            sw.Flush();
            sw.Dispose();
        }
    }
}
