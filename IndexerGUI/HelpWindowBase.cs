// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Input;
using System.Windows.Navigation;

namespace Indexer
{
    public partial class HelpWindowBase : Window
    {
        public HelpWindowBase()
        {
            CommandBindings.Add(new CommandBinding(ApplicationCommands.Close, CloseExecuted, CloseCanExecute));
            InputBindings.Add(new KeyBinding(ApplicationCommands.Close, new KeyGesture(Key.Enter)));

            // Locate and set styles:
            var stylesUri = new Uri("pack://application:,,,/Styles.xaml");
            Resources.MergedDictionaries.Add(new ResourceDictionary() { Source = stylesUri });

            Style = this.FindResource("HelpWindowStyle") as Style;
        }

        protected bool GetFileContent(string filename, ref string readResult)
        {
            try
            {
                using (var sr = new StreamReader(filename))
                {
                    readResult = sr.ReadToEnd();
                    return true;
                }
            }
            catch (Exception) { }
            return false;
        }

        protected void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }

        protected void CloseCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
            e.Handled = true;
        }

        protected void CloseExecuted(object sender, ExecutedRoutedEventArgs e)
        {
            Close();
        }

        protected void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
                this.DragMove();
        }
    }
}
