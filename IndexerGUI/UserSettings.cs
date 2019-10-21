// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Windows;
using Indexer.Properties;

namespace Indexer
{
    [DataContract]
    public class UserSettings
    {
        private const string SerializationPath = "UserSettings.xml";

        private readonly DataContractSerializer serializer;

        static UserSettings()
        {
            Instance = new UserSettings();
        }

        private UserSettings()
        {
            serializer = new DataContractSerializer(typeof (UserSettings));
            try
            {
                if (!File.Exists(SerializationPath))
                {
                    File.WriteAllText(SerializationPath, Resources.DefaultUserSettings);
                }

                var saved = (UserSettings) serializer.ReadObject(File.OpenRead(SerializationPath));

                if (saved == null) return;

                WndWidth = saved.WndWidth;
                WndHeight = saved.WndHeight;
                FiltersVisibility = saved.FiltersVisibility;
                SelectedDrives = saved.SelectedDrives ?? new List<char>();
                ExcludeHiddenAndSystem = saved.ExcludeHiddenAndSystem;
                ExcludeFolders = saved.ExcludeFolders;
                ExcludeFiles = saved.ExcludeFiles;
                YPos = saved.YPos;
                XPos = saved.XPos;
            }
            catch (Exception ex)
            {
                Log.Instance.Error("Cannot read user settings: " + ex.Message);
            }
        }

        public static UserSettings Instance { get; set; }

        [DataMember]
        public double WndWidth { get; private set; }

        [DataMember]
        public double WndHeight { get; private set; }

        [DataMember]
        public Visibility FiltersVisibility { get; set; }

        [DataMember]
        public List<char> SelectedDrives { get; private set; }

        [DataMember]
        public bool ExcludeHiddenAndSystem { get; set; }

        [DataMember]
        public bool ExcludeFolders { get; set; }

        [DataMember]
        public bool ExcludeFiles { get; set; }

        [DataMember]
        public double XPos { get; set; }

        [DataMember]
        public double YPos { get; set; }

        public void Save(MainWindow w)
        {
            WndHeight = w.Height;
            WndWidth = w.Width;
            FiltersVisibility = w.FiltersVisibility;
            ExcludeHiddenAndSystem = w.ExcludeHiddenAndSystem;
            ExcludeFolders = w.ExcludeFolders;
            ExcludeFiles = w.ExcludeFiles;
            XPos = w.Top;
            YPos = w.Left;

            SelectedDrives.Clear();

            foreach (var drive in w.Drives.Where(d => d.IsChecked).Select(d => d.Name))
            {
                SelectedDrives.Add(drive);
            }

            try
            {
                using (var stream = File.Create(SerializationPath))
                {
                    serializer.WriteObject(stream, this);
                }
            }
            catch (Exception ex)
            {
                Log.Instance.Error("Cannot save user settings: " + ex.Message);
            }
        }
    }
}
