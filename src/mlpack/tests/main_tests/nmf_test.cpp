/**
  * @file nmf_test.cpp
  * @author Wenhao Huang
  *
  * Test mlpackMain() of nmf_main.cpp
  *
  * mlpack is free software; you may redistribute it and/or modify it under the
  * terms of the 3-clause BSD license.  You should have received a copy of the
  * 3-clause BSD license along with mlpack.  If not, see
  * http://www.opensource.org/licenses/BSD-3-Clause for more information.
  */
#include <string>

#define BINDING_TYPE BINDING_TYPE_TEST

static const std::string testName = "NonNegativeMatrixFactorization";

#include <mlpack/core.hpp>
#include <mlpack/methods/nmf/nmf_main.cpp>
#include <mlpack/core/util/mlpack_main.hpp>
#include "test_helper.hpp"

#include "../catch.hpp"

using namespace mlpack;
using namespace arma;


struct NMFTestFixture
{
 public:
  NMFTestFixture()
  {
    // Cache in the options for this program.
    IO::RestoreSettings(testName);
  }

  ~NMFTestFixture()
  {
    // Clear the settings.
    bindings::tests::CleanMemory();
    IO::ClearSettings();
  }
};

static void ResetSettings()
{
  bindings::tests::CleanMemory();
  IO::ClearSettings();
  IO::RestoreSettings(testName);
}

/**
 * Ensure the resulting matrices W, H have expected shape.
 * Multdist update rule (Default Case).
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFMultdistShapeTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(8, 10);
  int r = 5;

  SetInputParam("update_rules", std::string("multdist"));
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  // Perform NMF.
  mlpackMain();

  // Get resulting matrices.
  const mat& w = IO::GetParam<mat>("w");
  const mat& h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 8);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}

/**
 * Ensure the resulting matrices W, H have expected shape.
 * Multdiv update rule.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFMultdivShapeTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(8, 10);
  int r = 5;

  SetInputParam("update_rules", std::string("multdiv"));
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  // Perform NMF.
  mlpackMain();

  // Get resulting matrices.
  const mat& w = IO::GetParam<mat>("w");
  const mat& h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 8);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}

/**
 * Ensure the resulting matrices W, H have expected shape.
 * Als update rule.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFAlsShapeTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(8, 10);
  int r = 5;

  SetInputParam("update_rules", std::string("als"));
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  // Perform NMF.
  mlpackMain();

  // Get resulting matrices.
  const mat& w = IO::GetParam<mat>("w");
  const mat& h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 8);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}

/**
 * Ensure the rank is positive.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFRankBoundTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(10, 10);
  int r;

  // Rank should not be negative.
  r = -1;
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  Log::Fatal.ignoreInput = true;
  REQUIRE_THROWS_AS(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;

  // Rank should not be 0.
  r = 0;
  SetInputParam("rank", r);

  Log::Fatal.ignoreInput = true;
  REQUIRE_THROWS_AS(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Ensure the max_iterations is non-negative.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFMaxIterartionBoundTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(10, 10);
  int r = 5;

  // max_iterations should be non-negative.
  SetInputParam("max_iterations", int(-1));
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  Log::Fatal.ignoreInput = true;
  REQUIRE_THROWS_AS(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Ensure the update rule is one of 
 * {"multdist", "multdiv", "als"}.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFUpdateRuleTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = randu<mat>(10, 10);
  int r = 5;

  // Invalid update rule.
  SetInputParam("update_rules", std::string("invalid_rule"));
  SetInputParam("input", std::move(v));
  SetInputParam("rank", r);

  Log::Fatal.ignoreInput = true;
  REQUIRE_THROWS_AS(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Ensure min_residue is used, by testing that 
 * min_resude makes a difference to the program.  
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFMinResidueTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = arma::randu(10, 10);
  mat initialW = arma::randu(10, 5);
  mat initialH = arma::randu(5, 10);
  int r = 5;

  // Set a larger min_residue.
  SetInputParam("min_residue", double(1));
  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w1 = IO::GetParam<mat>("w");
  const mat h1 = IO::GetParam<mat>("h");

  ResetSettings();

  // Set a smaller min_residue.
  SetInputParam("min_residue", double(1e-3));
  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w2 = IO::GetParam<mat>("w");
  const mat h2 = IO::GetParam<mat>("h");

  // The resulting matrices should be different.
  REQUIRE(arma::norm(w1 - w2) > 1e-5);
  REQUIRE(arma::norm(h1 - h2) > 1e-5);
}

/**
 * Ensure max_iterations is used, by testing that 
 * max_iterations makes a difference to the program.  
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFMaxIterationTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = arma::randu(10, 10);
  mat initialW = arma::randu(10, 5);
  mat initialH = arma::randu(5, 10);
  int r = 5;

  // Set a larger max_iterations.
  SetInputParam("max_iterations", int(100));
  // Remove the influence of min_residue.
  SetInputParam("min_residue", double(0));
  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w1 = IO::GetParam<mat>("w");
  const mat h1 = IO::GetParam<mat>("h");

  ResetSettings();

  // Set a smaller max_iterations.
  SetInputParam("max_iterations", int(5));
  // Remove the influence of min_residue.
  SetInputParam("min_residue", double(0));
  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w2 = IO::GetParam<mat>("w");
  const mat h2 = IO::GetParam<mat>("h");

  // The resulting matrices should be different.
  REQUIRE(arma::norm(w1 - w2) > 1e-5);
  REQUIRE(arma::norm(h1 - h2) > 1e-5);
}

/**
 * Test NMF with given initial_w and initial_h.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFWHGivenInitTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = arma::randu(10, 10);
  mat initialW = arma::randu(10, 5);
  mat initialH = arma::randu(5, 10);
  int r = 5;

  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w = IO::GetParam<mat>("w");
  const mat h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 10);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}

/**
 * Test NMF with given initial_w.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFWGivenInitTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = arma::randu(10, 10);
  mat initialW = arma::randu(10, 5);
  int r = 5;

  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_w", initialW);

  mlpackMain();

  const mat w = IO::GetParam<mat>("w");
  const mat h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 10);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}

/**
 * Test NMF with given initial_h.
 */
TEST_CASE_METHOD(NMFTestFixture, "NMFHGivenInitTest",
                "[NMFMainTest][BindingTests]")
{
  mat v = arma::randu(10, 10);
  mat initialH = arma::randu(5, 10);
  int r = 5;

  SetInputParam("input", v);
  SetInputParam("rank", r);
  SetInputParam("initial_h", initialH);

  mlpackMain();

  const mat w = IO::GetParam<mat>("w");
  const mat h = IO::GetParam<mat>("h");

  // Check the shapes of W and H.
  REQUIRE(w.n_rows == 10);
  REQUIRE(w.n_cols == 5);
  REQUIRE(h.n_rows == 5);
  REQUIRE(h.n_cols == 10);
}
