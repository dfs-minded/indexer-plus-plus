// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.ComponentModel;

namespace Indexer
{
    public class DriveInfo : INotifyPropertyChanged
    {
        public char Name { get; set; }
        public string Label { get; set; }

        private bool isChecked;

        public bool IsChecked
        {
            get { return isChecked; }
            set
            {
                isChecked = value;
                OnPropertyChanged("IsChecked");
            }
        }

        public DriveInfo(char name, string label, bool isChecked = false)
        {
            Name = name;
            Label = label + " (" + name + ":)";
            this.isChecked = isChecked;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
