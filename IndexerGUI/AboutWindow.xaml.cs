// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

namespace Indexer
{
    /// <summary>
    /// Interaction logic for About Indexer++ Window.
    /// </summary>
    public partial class AboutWindow : HelpWindowBase
    {
        private string licenseAgreement;
        public string LicenseAgreement { get { return licenseAgreement; } }

        public AboutWindow()
        {
            GetFileContent("LICENSE", ref licenseAgreement);

            InitializeComponent();
        }
    }
}