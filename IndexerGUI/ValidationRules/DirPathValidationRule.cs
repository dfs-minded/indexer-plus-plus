// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.Globalization;
using System.IO;
using System.Windows.Controls;

namespace Indexer
{
    public class DirPathValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            var path = (string)value;

            if (string.IsNullOrWhiteSpace(path) || Directory.Exists(path))
                return new ValidationResult(true, null);

            return new ValidationResult(false, "Path does not exist.");
        }
    }
}
