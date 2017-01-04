// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using Microsoft.Win32;

namespace AddExplorerContextMenu
{
    internal class RegistryManager
    {
        private const string KeyFolderName = "Indexer++";

        private const string FilterDirPathArgName = "filterDirPath";

        public void Register(string exePath, string iconPath)
        {
            using (RegistryKey shellKey = Registry.ClassesRoot.OpenSubKey("Directory\\shell", true))
            {
                RegistryKey keyFolder = shellKey?.CreateSubKey(KeyFolderName);
                if (keyFolder == null) return;
                
                keyFolder.SetValue("Icon", iconPath);
                keyFolder.CreateSubKey("command").SetValue("", exePath + " " + FilterDirPathArgName + "=\"%1\"");
                
            }
        }

        public void Unregister()
        {
            using (RegistryKey shellKey = Registry.ClassesRoot.OpenSubKey("Directory\\shell", true))
            {
                using (var subKey = shellKey.OpenSubKey(KeyFolderName))
                {
                    if (subKey != null)
                        shellKey.DeleteSubKeyTree(KeyFolderName);
                }
            }
        }
    }
}
