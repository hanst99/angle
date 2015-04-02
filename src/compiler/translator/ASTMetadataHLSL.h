//
// Copyright (c) 2002-2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Defines analyses of the AST needed for HLSL generation

#ifndef COMPILER_TRANSLATOR_ASTMETADATAHLSL_H_
#define COMPILER_TRANSLATOR_ASTMETADATAHLSL_H_

#include <set>
#include <vector>

class CallDAG;
class TIntermNode;
class TIntermSelection;
class TIntermLoop;

struct ASTMetadataHLSL
{
    ASTMetadataHLSL()
        : mUsesGradient(false)
    {
    }

    // Here "something uses a gradient" means here that it either contains a
    // gradient operation, or a call to a function that uses a gradient.
    bool hasGradientInCallGraph(TIntermSelection *node);
    bool hasGradientInCallGraph(TIntermLoop *node);

    // Does the function use a gradient.
    bool mUsesGradient;

    // Even if usesGradient is true, some control flow might not use a gradient
    // so we store the set of all gradient-using control flows.
    std::set<TIntermNode*> mControlFlowsContainingGradient;
};

typedef std::vector<ASTMetadataHLSL> MetadataList;

// Return the AST analysis result, in the order defined by the call DAG
MetadataList CreateASTMetadataHLSL(TIntermNode *root, const CallDAG &callDag);

#endif // COMPILER_TRANSLATOR_ASTMETADATAHLSL_H_
