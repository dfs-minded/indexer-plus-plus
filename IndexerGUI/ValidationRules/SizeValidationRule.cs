// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.Globalization;
using System.Windows.Controls;

namespace Indexer
{
    public class SizeValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            int parsedVal;

            if (!Helper.TryParseSize((string) value, out parsedVal))
                return new ValidationResult(false, "Allowed size shortcuts: KB, MB, GB, TB.\nIf not specified KB assumed.");

            return new ValidationResult(true, null);
        }
    }
}
