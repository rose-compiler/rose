// Copyright (c) 2018-2019, NVIDIA CORPORATION.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FORTRAN_BUILDER_BUILD_H_
#define FORTRAN_BUILDER_BUILD_H_

#include "char-block.h"
#include "characters.h"
#include <functional>
#include <iosfwd>

namespace Fortran::evaluate {
struct GenericExprWrapper;
}

namespace Fortran::parser {
struct Program;
}

namespace Fortran::builder {

using Fortran::parser::CharBlock;

// A function called before each Statement is unparsed.
using preStatementType =
    std::function<void(const CharBlock &, std::ostream &, int)>;

// A function to handle unparsing of evaluate::GenericExprWrapper
// rather than original expression parse trees.
using TypedExprAsFortran =
    std::function<void(std::ostream &, const evaluate::GenericExprWrapper &)>;

// Converts parsed program to out as Fortran.
void Build(std::ostream &out, const parser::Program &program,
    parser::Encoding encoding = parser::Encoding::UTF_8, bool capitalizeKeywords = true,
    bool backslashEscapes = true, preStatementType *preStatement = nullptr,
    TypedExprAsFortran *expr = nullptr);
}

#endif // FORTRAN_BUILDER_BUILD_H_
