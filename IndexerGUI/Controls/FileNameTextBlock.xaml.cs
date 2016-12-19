// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Threading;
using CLIInterop;
using KeyEventArgs = System.Windows.Input.KeyEventArgs;
using MessageBox = System.Windows.MessageBox;
using TextBox = System.Windows.Controls.TextBox;

namespace Indexer.Controls
{
    public partial class FileNameTextBlock
    {
        private readonly DispatcherTimer renameTimer = new DispatcherTimer();
        private readonly DispatcherTimer selectionClickTimer = new DispatcherTimer();

        public FileNameTextBlock()
        {
            TextTrimming = TextTrimming.WordEllipsis;
            TextWrapping = TextWrapping.NoWrap;

            InitializeComponent();

            EditableText.IsVisibleChanged += EditableText_IsVisibleChanged;

            renameTimer.Interval = TimeSpan.FromMilliseconds(SystemInformation.DoubleClickTime);
            renameTimer.Tick += RenameTimerOnTick;

            selectionClickTimer.Interval = TimeSpan.FromMilliseconds(SystemInformation.DoubleClickTime);
            selectionClickTimer.Tick += SelectionClickTimerOnTick;
        }

        public TextAlignment TextAlignment
        {
            get
            {
                return (TextAlignment) GetValue(TextAlignmentProperty);
            }
            set
            {
                SetValue(TextAlignmentProperty, value);
            }
        }

        public TextWrapping TextWrapping
        {
            get
            {
                return (TextWrapping) GetValue(TextWrappingProperty);
            }
            set
            {
                SetValue(TextWrappingProperty, value);
            }
        }

        public TextTrimming TextTrimming
        {
            get
            {
                return (TextTrimming) GetValue(TextTrimmingProperty);
            }
            set
            {
                SetValue(TextTrimmingProperty, value);
            }
        }

        public bool IsEditMode
        {
            get
            {
                return (bool) GetValue(IsEditModeProperty);
            }
            set
            {
                SetValue(IsEditModeProperty, value);
            }
        }

        public string Text
        {
            get
            {
                return (string) GetValue(TextProperty);
            }
            set
            {
                SetValue(TextProperty, value);
            }
        }

        private void SelectionClickTimerOnTick(object sender, EventArgs e)
        {
            selectionClickTimer.Stop();
        }

        private void RenameTimerOnTick(object sender, EventArgs eventArgs)
        {
            // Debug.WriteLine("RenameTimerOnTick");

            renameTimer.Stop();

            IsEditMode = true;
        }

        private void EditableText_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if (EditableText.IsVisible)
            {
                EditableText.Focus();

                Keyboard.Focus(EditableText);

                EditableText.SelectAll();
            }
        }

        private static void OnIsEditModeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var editableTextBlock = (FileNameTextBlock) d;

            if (!editableTextBlock.IsEditMode)
            {
                editableTextBlock.EndEdit();
            }
        }

        private void EditableText_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                IsEditMode = false;
            }
            else if (e.Key == Key.Escape)
            {
                Revert();
                IsEditMode = false;
            }
        }

        private void EditableText_OnLostFocus(object sender, RoutedEventArgs e)
        {
            IsEditMode = false;
        }

        private void Revert()
        {
            var fileInfoWrapper = DataContext as FileInfoWrapper;

            if (fileInfoWrapper == null) return;

            var bindingExpressionBase = BindingOperations.GetBindingExpressionBase(EditableText, TextBox.TextProperty);

            if (bindingExpressionBase != null) bindingExpressionBase.UpdateTarget();
        }

        private void EndEdit()
        {
            var fileInfoWrapper = DataContext as FileInfoWrapper;

            if (fileInfoWrapper == null || fileInfoWrapper.Name == EditableText.Text)
                return;

            var oldPath = fileInfoWrapper.FullName;

            var newFilename = EditableText.Text;
            var newPath = Path.Combine(fileInfoWrapper.Path + "\\", newFilename);

            // If filenames differ only in letters case
            if (String.Compare(fileInfoWrapper.Name, newFilename, StringComparison.OrdinalIgnoreCase) == 0)
            {
                var strTmpName = oldPath + "_temp";

                // If is directory.
                if (Directory.Exists(oldPath))
                {
                    // Rename the directory.
                    Directory.Move(oldPath, strTmpName);
                    Directory.Move(strTmpName, newPath);
                }
                else
                {
                    // Rename the file.
                    File.Move(oldPath, strTmpName);
                    File.Move(strTmpName, newPath);
                }

                return;
            }

            // Ensure that the target does not exist.
            var fileExist = File.Exists(newPath);
            var dirExist = Directory.Exists(newPath);
            if (fileExist || dirExist)
            {
                var objectType = dirExist ? "folder" : "file";
                var reply =
                    MessageBox.Show(
                        "The destination already contains a " + objectType + " named '" + newFilename +
                        "'.\nDo you want to replace it?", "Confirm Replace", MessageBoxButton.YesNo);
                if (reply == MessageBoxResult.No)
                {
                    Revert();
                    return;
                }

                try
                {
                    File.Delete(newPath);
                }
                catch (UnauthorizedAccessException)
                {
                    MessageBox.Show("Access to the path '" + newPath + "' is denied", "Cannot delete object", MessageBoxButton.OK);
                    return;
                }
            }

            if (Directory.Exists(oldPath))
            {
                // Rename the directory.
                Directory.Move(oldPath, newPath);
            }
            else
            {
                // Rename the file.
                try
                {
                    File.Move(oldPath, newPath);
                }
                catch(UnauthorizedAccessException)
                {
                    MessageBox.Show("You are not permitted to modify this file.\nAltering or changing files used by operating system could damage your system!", "Unauthorized Access");
                }
                
            }
        }

        private void ReadonlyText_OnPreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (selectionClickTimer.IsEnabled) return;

            if (renameTimer.IsEnabled)
            {
                renameTimer.Stop();

                return;
            }

            var listBoxItem = this.FindParent<ListBoxItem>();

            if (listBoxItem == null) return;

            if (!listBoxItem.IsSelected)
            {
                selectionClickTimer.Start();

                return;
            }

            renameTimer.Start();
        }

        public static readonly DependencyProperty IsEditModeProperty = DependencyProperty.Register(
            "IsEditMode", typeof (bool), typeof (FileNameTextBlock),
            new PropertyMetadata(default(bool), OnIsEditModeChanged));

        public static readonly DependencyProperty TextProperty = DependencyProperty.Register(
            "Text", typeof (string), typeof (FileNameTextBlock), new PropertyMetadata(default(string)));

        public static readonly DependencyProperty TextAlignmentProperty = DependencyProperty.Register(
            "TextAlignment", typeof (TextAlignment), typeof (FileNameTextBlock),
            new PropertyMetadata(default(TextAlignment)));

        public static readonly DependencyProperty TextWrappingProperty = DependencyProperty.Register(
            "TextWrapping", typeof (TextWrapping), typeof (FileNameTextBlock),
            new PropertyMetadata(default(TextWrapping)));

        public static readonly DependencyProperty TextTrimmingProperty = DependencyProperty.Register(
            "TextTrimming", typeof (TextTrimming), typeof (FileNameTextBlock),
            new PropertyMetadata(default(TextTrimming)));
    }
}