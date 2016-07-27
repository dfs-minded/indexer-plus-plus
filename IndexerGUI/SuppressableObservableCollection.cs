// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Indexer.Utilities
{
    public class SuppressableObservableCollection<T> : ObservableCollection<T>
    {
        public bool SuppressChanged { get; set; }

        public SuppressableObservableCollection()
        {
            SuppressChanged = false;
        }

        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (!SuppressChanged)
                base.OnCollectionChanged(e);
        }

        public void FireCollectionChange()
        {
            base.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
    }
}
