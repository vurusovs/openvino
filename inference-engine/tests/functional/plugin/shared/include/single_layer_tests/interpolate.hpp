// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <memory>

#include "functional_test_utils/layer_test_utils.hpp"

#include "ngraph_functions/builders.hpp"
#include "ngraph_functions/utils/ngraph_helpers.hpp"

namespace LayerTestsDefinitions {

typedef std::tuple<
        std::set<size_t>,                                      // Axes
        ngraph::op::v3::Interpolate::InterpolateMode,          // InterpolateMode
        ngraph::op::v3::Interpolate::CoordinateTransformMode,  // CoordinateTransformMode
        ngraph::op::v3::Interpolate::NearestMode,              // NearestMode
        bool,                                                  // AntiAlias
        std::vector<size_t>,                                   // PadBegin
        std::vector<size_t>,                                   // PadEnd
        double                                                 // Cube coef
> InterpolateSpecificParams;

typedef std::tuple<
        InterpolateSpecificParams,
        InferenceEngine::Precision,     // Net precision
        InferenceEngine::SizeVector,    // Input shapes
        InferenceEngine::SizeVector,    // Target shapes
        LayerTestsUtils::TargetDevice   // Device name
> InterpolateLayerTestParams;

class InterpolateLayerTest : public testing::WithParamInterface<InterpolateLayerTestParams>,
                             public LayerTestsUtils::LayerTestsCommon {
public:
    static std::string getTestCaseName(testing::TestParamInfo<InterpolateLayerTestParams> obj);

protected:
    void SetUp() override;
};

}  // namespace LayerTestsDefinitions
