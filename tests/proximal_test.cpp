// Copyright (c) 2018 ensmallen developers.
//
// Licensed under the 3-clause BSD license (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.opensource.org/licenses/BSD-3-Clause

#include <ensmallen.hpp>
#include "catch.hpp"

using namespace arma;
using namespace ens;
using namespace ens::test;

/**
 * Approximate vector using a vector with l1 norm small than or equal to tau.
 */
TEST_CASE("ProjectToL1","[ProximalTest]")
{
  int D = 100;  // Dimension of the problem.

  // Norm of L1 ball.
  double tau1 = 1.5;
  double tau2 = 0.5;

  // Vector to be projected, with unit l1 norm.
  vec v = randu<vec>(D);
  v = normalise(v, 1);

  // v is inside the l1 ball, so the projection will not change v.
  vec v1 = v;
  Proximal::ProjectToL1Ball(v1, tau1);
  REQUIRE(norm(v - v1, 2) == Approx(0.0).margin(1e-10));

  // v is outside the l1 ball, so the projection should find the closest.
  vec v2 = v;
  Proximal::ProjectToL1Ball(v2, tau2);
  double distance = norm(v2 - v, 2);
  for (size_t i = 1; i < 1000; i++)
  {
    // Randomly generate a vector on the surface of the l1 ball with norm tau2.
    vec vSurface = randu<vec>(D);
    vSurface = tau2 * normalise(vSurface, 1);

    double distanceNew = norm(vSurface - v, 2);

    REQUIRE(distanceNew >= distance);
  }
}

/**
 * Approximate a vector with a tau-sparse vector.
 */
TEST_CASE("ProjectToL0","[ProximalTest]")
{
  int D = 100;  // Dimension of the problem.
  int tau = 25; // Sparsity requirement.

  // Vector to be projected.
  vec v = randn<vec>(D);

  vec v0 = v;
  Proximal::ProjectToL0Ball(v0, tau);
  double distance = norm(v0 - v, 2);

  for (size_t i = 1; i < 1000; i++)
  {
    // Randomly find a subset of the support of v, generate a tau-sparse
    // vector by restricting v to this support.
    uvec indices = linspace<uvec>(0, D - 1, D);
    indices = shuffle(indices);
    indices = indices.head(tau);
    vec vNew = zeros<vec>(D);
    vNew.elem(indices) = v.elem(indices);

    double distanceNew = norm(v - vNew, 2);
    REQUIRE(distanceNew >= distance);
  }
}