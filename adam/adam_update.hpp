/**
 * @file adam.hpp
 * @author Ryan Curtin
 * @author Vasanth Kalingeri
 * @author Marcus Edel
 * @author Vivek Pal
 *
 * Adam and AdaMax optimizer. Adam is an an algorithm for first-order gradient-
 * -based optimization of stochastic objective functions, based on adaptive
 * estimates of lower-order moments. AdaMax is simply a variant of Adam based
 * on the infinity norm.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_ADAM_ADAM_UPDATE_HPP
#define MLPACK_CORE_OPTIMIZERS_ADAM_ADAM_UPDATE_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace optimization {

/**
 * Adam is an optimizer that computes individual adaptive learning rates for
 * different parameters from estimates of first and second moments of the
 * gradients. AdaMax is a variant of Adam based on the infinity norm as given
 * in the section 7 of the following paper.
 *
 * For more information, see the following.
 *
 * @code
 * @article{Kingma2014,
 *   author    = {Diederik P. Kingma and Jimmy Ba},
 *   title     = {Adam: {A} Method for Stochastic Optimization},
 *   journal   = {CoRR},
 *   year      = {2014}
 * }
 * @endcode
 *
 */
class AdamUpdate
{
 public:
  /**
   * Construct the Adam update policy with the given epsilon parameter.
   *
   * @param epsilon The epsilon value used to initialise the squared gradient
   *        parameter.
   */
  AdamUpdate(const double epsilon = 1e-8,
             const double beta1 = 0.9,
             const double beta2 = 0.999,
             const bool adaMax = false) :
    epsilon(epsilon),
    beta1(beta1),
    beta2(beta2),
    adaMax(adaMax)
  {
    // Nothing to do.
  }

  /**
   * The Initialize method is called by SGD Optimizer method before the start of
   * the iteration update process.
   *
   * @param rows number of rows in the gradient matrix.
   * @param cols number of columns in the gradient matrix.
   */
  void Initialize(const size_t rows,
                  const size_t cols)
  {
    m = arma::zeros<arma::mat>(rows, cols);
    if (adaMax)
    {
      u = arma::zeros<arma::mat>(rows, cols);
    }
    else
    {
      v = arma::zeros<arma::mat>(rows, cols);
    }
  }

  /**
   * Update step for Adam.
   *
   * @param iterate Parameters that minimize the function.
   * @param stepSize Step size to be used for the given iteration.
   * @param gradient The gradient matrix.
   */
  void Update(arma::mat& iterate,
              const double stepSize,
              const arma::mat& gradient,
              const size_t i)
  {
    // And update the iterate.
    m *= beta1;
    m += (1 - beta1) * gradient;

    if (adaMax)
    {
      // Update the exponentially weighted infinity norm.
      u *= beta2;
      u = arma::max(u, arma::abs(gradient));
    }
    else
    {
      v *= beta2;
      v += (1 - beta2) * (gradient % gradient);
    }

    const double biasCorrection1 = 1.0 - std::pow(beta1, (double) i);
    const double biasCorrection2 = 1.0 - std::pow(beta2, (double) i);

    if (adaMax)
    {
      if (biasCorrection1 != 0.0)
        iterate -= (stepSize / biasCorrection1 * m / (u + epsilon));
    }
    else
    {
      /**
       * It should be noted that the term, m / (arma::sqrt(v) + eps), in the
       * following expression is an approximation of the following actual term;
       * m / (arma::sqrt(v) + (arma::sqrt(biasCorrection2) * eps).
       */
      iterate -= (stepSize * std::sqrt(biasCorrection2) / biasCorrection1) *
                  m / (arma::sqrt(v) + epsilon);
    }
  }

  //! Get the value used to initialise the squared gradient parameter.
  double Epsilon() const { return epsilon; }
  //! Modify the value used to initialise the squared gradient parameter.
  double& Epsilon() { return epsilon; }

  //! Get the smoothing parameter.
  double Beta1() const { return beta1; }
  //! Modify the smoothing parameter.
  double& Beta1() { return beta1; }

  //! Get the second moment coefficient.
  double Beta2() const { return beta2; }
  //! Modify the second moment coefficient.
  double& Beta2() { return beta2; }

  //! Get whether or not the AdaMax optimizer is specified.
  bool AdaMax() const { return adaMax; }
  //! Modify wehther or not the AdaMax optimizer is to be used.
  bool& AdaMax() { return adaMax; }

 private:
  // The epsilon value used to initialise the squared gradient parameter.
  double epsilon;

  // The smoothing parameter.
  double beta1;

  // The second moment coefficient.
  double beta2;

  //! Specifies whether or not the AdaMax optimizer is to be used.
  bool adaMax;

  // The exponential moving average of gradient values.
  arma::mat m;

  // The exponentially weighted infinity norm.
  arma::mat u;

  // The exponential moving average of squared gradient values.
  arma::mat v;
};

} // namespace optimization
} // namespace mlpack

#endif
