// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Navigation;

namespace Indexer
{
    /// <summary>
    /// Interaction logic for About Indexer++ Window.
    /// </summary>
    public partial class AboutWindow : Window
    {
        public string LicenseAgreement { get; set; }

        public AboutWindow()
        {
            try
            {
                using (StreamReader sr = new StreamReader("LICENSE"))
                {
                    LicenseAgreement = sr.ReadToEnd();
                }
            }
            catch (Exception) {}
            
            InitializeComponent();
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }


        private void CloseCanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
            e.Handled = true;
        }

        private void CloseExecuted(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
        {
            Close();
        }
    }
}
