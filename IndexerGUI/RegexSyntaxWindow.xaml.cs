// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

namespace Indexer
{
    /// <summary>
    /// Interaction logic for RegexSyntax Window.
    /// </summary>
    public partial class RegexSyntaxWindow : HelpWindowBase
    {
        private string regexSyntaxText;
        public string RegexSyntaxText { get { return regexSyntaxText; } }

        public RegexSyntaxWindow()
        {
            GetFileContent("BasicRE2Syntax.txt", ref regexSyntaxText);

            InitializeComponent();
        }
    }
}