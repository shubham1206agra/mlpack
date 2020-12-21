/**
 * @file methods/ann/layer/log_softmax_impl.hpp
 * @author Marcus Edel
 *
 * Implementation of the LogSoftmax class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_LOG_SOFTMAX_IMPL_HPP
#define MLPACK_METHODS_ANN_LAYER_LOG_SOFTMAX_IMPL_HPP

// In case it hasn't yet been included.
#include "log_softmax.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputDataType, typename OutputDataType>
LogSoftMax<InputDataType, OutputDataType>::LogSoftMax()
{
  // Nothing to do here.
}

template<typename InputDataType, typename OutputDataType>
void LogSoftMax<InputDataType, OutputDataType>::Forward(
    const arma::mat& input, arma::mat& output)
{
  arma::mat maxInput = arma::repmat(arma::max(input), input.n_rows, 1);
  output = (maxInput - input);

  // Approximation of the base-e exponential function. The acuracy however is
  // about 0.00001 lower as using exp. Credits go to Leon Bottou.
  output.transform([](double x)
  {
    //! Fast approximation of exp(-x) for x positive.
    static constexpr double A0 = 1.0;
    static constexpr double A1 = 0.125;
    static constexpr double A2 = 0.0078125;
    static constexpr double A3 = 0.00032552083;
    static constexpr double A4 = 1.0172526e-5;

    if (x < 13.0)
    {
      double y = A0 + x * (A1 + x * (A2 + x * (A3 + x * A4)));
      y *= y;
      y *= y;
      y *= y;
      y = 1 / y;

      return y;
    }

    return 0.0;
  });

  maxInput.each_row() += arma::log(arma::sum(output));
  output = input - maxInput;
}

template<typename InputDataType, typename OutputDataType>
void LogSoftMax<InputDataType, OutputDataType>::Backward(
    const arma::mat& input,
    const arma::mat& gy,
    arma::mat& g)
{
  g = arma::exp(input) + gy;
}

template<typename InputDataType, typename OutputDataType>
template<typename Archive>
void LogSoftMax<InputDataType, OutputDataType>::serialize(
    Archive& /* ar */,
    const uint32_t /* version */)
{
  // Nothing to do here.
}

} // namespace ann
} // namespace mlpack

#endif
