// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Threading;
using System.Threading.Tasks;

namespace Indexer
{
    public class SingleThreadTimer
    {
        public TimeSpan Time { get; private set; }
        public Action Action { get; private set; }
        private bool stop;

        public SingleThreadTimer(TimeSpan time, Action action)
        {
            Time = time;
            Action = action;
        }

        public void Start()
        {
            Task.Factory.StartNew(Run);
        }

        public void Stop()
        {
            stop = true;
        }

        private void Run()
        {
            var span = new TimeSpan();

            while (!stop)
            {
                if (span < Time)
                    Thread.Sleep(Time - span);

                var t1 = DateTime.Now;
                Action();
                var t2 = DateTime.Now;
                span = t2 - t1;
            }
        }
    }
}
