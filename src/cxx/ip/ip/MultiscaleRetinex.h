/**
 * @file src/cxx/ip/ip/MultiscaleRetinex.h
 * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
 *
 * @brief Implements the Multiscale Retinex algorithm as described in:
 *  "A Multiscale Retinex for bridging the gap between color images and the
 *   Human observation of scenes", D. Jobson, Z. Rahman and G. Woodell,
 *  in IEEE Transactions on Image Processing, vol. 6, n. 7, July 1997
 */

#ifndef TORCH5SPRO_IP_MULTISCALE_RETINEX_H
#define TORCH5SPRO_IP_MULTISCALE_RETINEX_H

#include "core/array_assert.h"
#include "core/cast.h"
#include "sp/convolution.h"
#include "ip/Gaussian.h"
#include <boost/shared_array.hpp>

namespace Torch {

	/**
	 * \ingroup libip_api
	 * @{
	 *
	 */
	namespace ip {

    /**
      * @brief This class allows to preprocess an image with the Multiscale
      * Retinex algorithm as described in:
      *  "A Multiscale Retinex for bridging the gap between color images and
      *   the Human observation of scenes", D. Jobson, Z. Rahman and 
      *   G. Woodell,
      *  in IEEE Transactions on Image Processing, vol. 6, n. 7, July 1997
      */
		class MultiscaleRetinex
		{
  		public:
			  /**
  			 * @brief Creates an object to preprocess images with the Multiscale
         *  Retinex algorithm
	  		 * @param n_scales The number of scales
         * @param size_min The size of the smallest convolution kernel
         * @param size_step The step size of the convolution kernels
         * @param sigma The standard deviation of the kernal for the smallest
         *  convolution kernel.
		  	 * @param border_opt The interpolation type for the convolution
			   */
	  		MultiscaleRetinex(const size_t n_scales=1, const int size_min=1, 
            const int size_step=1, const double sigma=5.,
            const enum Torch::sp::Convolution::BorderOption border_opt =
              Torch::sp::Convolution::Mirror):
          m_n_scales(n_scales), m_size_min(size_min), m_size_step(size_step),
          m_sigma(sigma), m_conv_border(border_opt),
          m_gaussians(new Torch::ip::Gaussian[m_n_scales])
  			{
          computeKernels();
        }

        /**
         * @brief Process a 2D blitz Array/Image
         * @param src The 2D input blitz array
         * @param src The 2D input blitz array
         */
        template <typename T> 
        void operator()(const blitz::Array<T,2>& src, blitz::Array<double,2>& dst);

        /**
         * @brief Process a 3D blitz Array/Image
         * @param src The 3D input blitz array
         * @param src The 3D input blitz array
         */
        template <typename T> 
        void operator()(const blitz::Array<T,3>& src, blitz::Array<double,3>& dst);

      private:
        void computeKernels(); 

        /**
         * @brief Attributes
         */	
        size_t m_n_scales;
        int m_size_min;
        int m_size_step;
        double m_sigma;
        enum Torch::sp::Convolution::BorderOption m_conv_border;

        boost::shared_array<Torch::ip::Gaussian> m_gaussians;
        blitz::Array<double,2> m_tmp;
    };

    template <typename T> 
    void Torch::ip::MultiscaleRetinex::operator()(const blitz::Array<T,2>& src, 
      blitz::Array<double,2>& dst)
    {
      // Checks are postponed to the Gaussian operator() function.
      dst = 0;
      if( m_tmp.extent(0) != src.extent(0) || m_tmp.extent(1) != src.extent(1))
        m_tmp.resize(src.extent(0), src.extent(1) );
      for(size_t s=0; s<m_n_scales; ++s) {
        m_gaussians[s](src,m_tmp);
        dst += log(src+1) - log(m_tmp+1);
      }
      dst /= m_n_scales;
    }

    template <typename T> 
    void Torch::ip::MultiscaleRetinex::operator()(const blitz::Array<T,3>& src, 
      blitz::Array<double,3>& dst)
    {
      for( int p=0; p<dst.extent(0); ++p) {
        const blitz::Array<T,2> src_slice = 
          src( p, blitz::Range::all(), blitz::Range::all() );
        blitz::Array<T,2> dst_slice = 
          dst( p, blitz::Range::all(), blitz::Range::all() );
        
        // Gaussian smooth plane
        this(src_slice, dst_slice);
      }
    }

	}
}

#endif /* TORCH5SPRO_IP_MULTISCALE_RETINEX_H */