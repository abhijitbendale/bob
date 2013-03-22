/**
 * @file math/cxx/eig.cc
 * @date Mon May 16 21:45:27 2011 +0200
 * @author Laurent El Shafey <Laurent.El-Shafey@idiap.ch>
 *
 * Copyright (C) 2011-2013 Idiap Research Institute, Martigny, Switzerland
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bob/math/eig.h>
#include <bob/math/Exception.h>
#include <bob/core/assert.h>
#include <bob/core/check.h>
#include <bob/core/array_copy.h>
#if !defined (HAVE_BLITZ_TINYVEC2_H)
#include <blitz/tinyvec-et.h>
#endif
#include <algorithm>
#include <vector>
#include <boost/shared_array.hpp>

// Declaration of the external LAPACK functions
// Eigenvalue decomposition of a real symmetric matrix (dsyevd)
//   (Divide and conquer version which is supposed to be faster than dsyev)
extern "C" void dsyevd_( const char *jobz, const char *uplo, const int *N,
  double *A, const int *lda, double *W, double *work, const int *lwork,
  int *iwork, const int *liwork, int *info);
// Generalized eigenvalue decomposition of a real symmetric definite matrix
//   (dsygvd) 
//   (Divide and conquer version which is supposed to be faster than dsygv)
extern "C" void dsygvd_( const int *itype, const char *jobz, const char *uplo,
  const int *N, double *A, const int *lda, double *B, const int *ldb, 
  double *W, double *work, const int *lwork, const int *iwork, 
  const int *liwork, int *info);

void bob::math::eigSym(const blitz::Array<double,2>& A, 
  blitz::Array<double,2>& V, blitz::Array<double,1>& D)
{
  // Size variable
  const int N = A.extent(0);
  const blitz::TinyVector<int,1> shape1(N);
  const blitz::TinyVector<int,2> shape2(N,N);

  // Check
  bob::core::array::assertZeroBase(A);
  bob::core::array::assertZeroBase(V);
  bob::core::array::assertZeroBase(D);

  bob::core::array::assertSameShape(A,shape2);
  bob::core::array::assertSameShape(V,shape2);
  bob::core::array::assertSameShape(D,shape1);

  bob::math::eigSym_(A, V, D);
}

void bob::math::eigSym_(const blitz::Array<double,2>& A, 
  blitz::Array<double,2>& V, blitz::Array<double,1>& D)
{
  // Size variable
  const int N = A.extent(0);

  // Prepares to call LAPACK function
  // Initialises LAPACK variables
  const char jobz = 'V'; // Get both the eigenvalues and the eigenvectors
  const char uplo = 'U';
  int info = 0;  
  const int lda = N;

  // Initialises LAPACK arrays
  blitz::Array<double,2> A_blitz_lapack;
  // Tries to use V directly
  blitz::Array<double,2> Vt = V.transpose(1,0);
  const bool V_direct_use = bob::core::array::isCZeroBaseContiguous(Vt);
  if (V_direct_use) 
  {
    A_blitz_lapack.reference(Vt);
    // Ugly fix for non-const transpose
    A_blitz_lapack = const_cast<blitz::Array<double,2>&>(A).transpose(1,0);
  }
  else
    // Ugly fix for non-const transpose
    A_blitz_lapack.reference(
      bob::core::array::ccopy(const_cast<blitz::Array<double,2>&>(A).transpose(1,0)));
  double *A_lapack = A_blitz_lapack.data();
  blitz::Array<double,1> D_blitz_lapack;
  const bool D_direct_use = bob::core::array::isCZeroBaseContiguous(D);
  if (D_direct_use)
    D_blitz_lapack.reference(D);
  else
    D_blitz_lapack.resize(D.shape());
  double *D_lapack = D_blitz_lapack.data();
 
  // Calls the LAPACK function 
  // A/ Queries the optimal size of the working arrays
  const int lwork_query = -1;
  double work_query;
  const int liwork_query = -1;
  int iwork_query;
  dsyevd_( &jobz, &uplo, &N, A_lapack, &lda, D_lapack, &work_query, 
    &lwork_query, &iwork_query, &liwork_query, &info);
  // B/ Computes the eigenvalue decomposition
  const int lwork = static_cast<int>(work_query);
  boost::shared_array<double> work(new double[lwork]);
  const int liwork = static_cast<int>(iwork_query);
  boost::shared_array<int> iwork(new int[liwork]);
  dsyevd_( &jobz, &uplo, &N, A_lapack, &lda, D_lapack, work.get(), &lwork,
    iwork.get(), &liwork, &info);
 
  // Checks info variable
  if (info != 0)
    throw bob::math::LapackError("The LAPACK dsyevd function returned a non-zero value.");

  // Copy singular vectors back to V if required
  if (!V_direct_use)
    Vt = A_blitz_lapack;

  // Copy result back to sigma if required
  if (!D_direct_use)
    D = D_blitz_lapack;
}


void bob::math::eigSym(const blitz::Array<double,2>& A, const blitz::Array<double,2>& B,
  blitz::Array<double,2>& V, blitz::Array<double,1>& D)
{
  // Size variable
  const int N = A.extent(0);
  const blitz::TinyVector<int,1> shape1(N);
  const blitz::TinyVector<int,2> shape2(N,N);
  bob::core::array::assertZeroBase(A);
  bob::core::array::assertZeroBase(B);
  bob::core::array::assertZeroBase(V);
  bob::core::array::assertZeroBase(D);

  bob::core::array::assertSameShape(A,shape2);
  bob::core::array::assertSameShape(B,shape2);
  bob::core::array::assertSameShape(V,shape2);
  bob::core::array::assertSameShape(D,shape1);

  bob::math::eigSym_(A, B, V, D);
}

void bob::math::eigSym_(const blitz::Array<double,2>& A, const blitz::Array<double,2>& B,
  blitz::Array<double,2>& V, blitz::Array<double,1>& D)
{
  // Size variable
  const int N = A.extent(0);

  // Prepares to call LAPACK function
  // Initialises LAPACK variables
  const int itype = 1;
  const char jobz = 'V'; // Get both the eigenvalues and the eigenvectors
  const char uplo = 'U';
  int info = 0;  
  const int lda = N;
  const int ldb = N;

  // Initialises LAPACK arrays
  blitz::Array<double,2> A_blitz_lapack;
  // Tries to use V directly
  blitz::Array<double,2> Vt = V.transpose(1,0);
  const bool V_direct_use = bob::core::array::isCZeroBaseContiguous(Vt);
  if (V_direct_use) 
  {
    A_blitz_lapack.reference(Vt);
    // Ugly fix for non-const transpose
    A_blitz_lapack = const_cast<blitz::Array<double,2>&>(A).transpose(1,0);
  }
  else
    // Ugly fix for non-const transpose
    A_blitz_lapack.reference(
      bob::core::array::ccopy(const_cast<blitz::Array<double,2>&>(A).transpose(1,0)));
  double *A_lapack = A_blitz_lapack.data();
  // Ugly fix for non-const transpose
  blitz::Array<double,2> B_blitz_lapack(
    bob::core::array::ccopy(const_cast<blitz::Array<double,2>&>(B).transpose(1,0)));
  double *B_lapack = B_blitz_lapack.data();
  blitz::Array<double,1> D_blitz_lapack;
  const bool D_direct_use = bob::core::array::isCZeroBaseContiguous(D);
  if (D_direct_use)
    D_blitz_lapack.reference(D);
  else
    D_blitz_lapack.resize(D.shape());
  double *D_lapack = D_blitz_lapack.data();
 
  // Calls the LAPACK function 
  // A/ Queries the optimal size of the working arrays
  const int lwork_query = -1;
  double work_query;
  const int liwork_query = -1;
  int iwork_query;
  dsygvd_( &itype, &jobz, &uplo, &N, A_lapack, &lda, B_lapack, &ldb, D_lapack,
    &work_query, &lwork_query, &iwork_query, &liwork_query, &info);
  // B/ Computes the generalized eigenvalue decomposition
  const int lwork = static_cast<int>(work_query);
  boost::shared_array<double> work(new double[lwork]);
  const int liwork = static_cast<int>(iwork_query);
  boost::shared_array<int> iwork(new int[liwork]);
  dsygvd_( &itype, &jobz, &uplo, &N, A_lapack, &lda, B_lapack, &ldb, D_lapack, 
    work.get(), &lwork, iwork.get(), &liwork, &info);

  // Checks info variable
  if (info != 0)
    throw bob::math::LapackError("The LAPACK dsygv function returned a non-zero value. This might be caused by a non-positive definite B matrix.");
 
  // Copy singular vectors back to V if required
  if (!V_direct_use)
    V = A_blitz_lapack.transpose(1,0);

  // Copy result back to sigma if required
  if (!D_direct_use)
    D = D_blitz_lapack;
}