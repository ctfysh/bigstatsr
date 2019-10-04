/******************************************************************************/

#include <bigstatsr/BMAcc.h>

using namespace Rcpp;
using std::size_t;

/******************************************************************************/

// [[Rcpp::export]]
NumericMatrix& scaling(NumericMatrix& source,
                       const NumericVector& mean,
                       const NumericVector& sd) {

  size_t n = source.rows();
  size_t m = source.cols();
  size_t i, j;

  for (j = 0; j < m; j++) {
    for (i = 0; i < n; i++) {
      source(i, j) -= mean[j];
      source(i, j) /= sd[j];
    }
  }

  return source;
}

// [[Rcpp::export]]
NumericMatrix& centering(NumericMatrix& source,
                         const NumericVector& mean) {

  size_t n = source.rows();
  size_t m = source.cols();
  size_t i, j;

  for (j = 0; j < m; j++)
    for (i = 0; i < n; i++)
      source(i, j) -= mean[j];

  return source;
}

/******************************************************************************/

//' Increment an FBM
//'
//' @param BM An `FBM` of type double.
//' @param source A matrix of same size as the `FBM` to increment.
//'
//' @return Returns nothing (`NULL`, invisibly).
//'
//' @export
//'
//' @examples
//' X <- FBM(10, 10, init = 0)
//' mat <- matrix(rnorm(100), 10, 10)
//'
//' big_increment(X, mat)
//' all.equal(X[], mat)
//'
//' big_increment(X, mat)
//' all.equal(X[], 2 * mat)
//'
// [[Rcpp::export]]
void big_increment(Environment BM,
                   const NumericMatrix& source) {

  XPtr<FBM> xpBM = BM["address"];
  if (xpBM->matrix_type() != 8)
    Rcpp::stop("'big_increment()' works with 'double' FBMs only.");

  BMAcc<double> macc(xpBM);

  size_t n = macc.nrow();
  size_t m = macc.ncol();

  myassert_size(source.rows(), n);
  myassert_size(source.cols(), m);

  for (size_t j = 0; j < m; j++)
    for (size_t i = 0; i < n; i++)
      macc(i, j) += source(i, j);
}

/******************************************************************************/

// For a square FBM
// [[Rcpp::export]]
void scaleK(Environment BM,
            const NumericVector& sums,
            const NumericVector& mu,
            const NumericVector& delta,
            int nrow) {

  XPtr<FBM> xpBM = BM["address"];
  BMAcc<double> K(xpBM);

  size_t n = K.nrow();
  myassert_size(K.ncol(), n);

  for (size_t j = 0; j < n; j++) {
    for (size_t i = 0; i < n; i++) {
      K(i, j) -= sums[i] * mu[j] + mu[i] * sums[j];
      K(i, j) += nrow * mu[i] * mu[j];
      K(i, j) /= delta(i) * delta(j);
    }
  }
}

/******************************************************************************/
