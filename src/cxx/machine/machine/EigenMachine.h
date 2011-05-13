/**
  * @file src/cxx/machine/machine/EigenMachine.h
  * @author <a href="mailto:Laurent.El-Shafey@idiap.ch">Laurent El Shafey</a> 
  *
  * @brief This class defines an EigenMachine class for subspace techniques.
  */

#ifndef TORCH5SPRO_MACHINE_EIGENMACHINE_H
#define TORCH5SPRO_MACHINE_EIGENMACHINE_H

#include "machine/Machine.h"
#include "machine/FrameSample.h"
#include "trainer/Sampler.h"

namespace Torch {
/**
  * \ingroup libmachine_api
  * @{
  *
  */
  namespace machine {
  
  
    /**
      * @brief Class which implements subspace projection.
      */
    class EigenMachine : public Machine<FrameSample, blitz::Array<double,1> > {
      public:
        /**
         * Default constructor
         */
        EigenMachine();

        /**
          * Constructors
          */
        EigenMachine(const blitz::Array<double,1>& eigenvalues, 
          const blitz::Array<double,2>& eigenvectors);
        EigenMachine(const blitz::Array<double,1>& eigenvalues, 
          const blitz::Array<double,2>& eigenvectors, int n_outputs);
        EigenMachine(const blitz::Array<double,1>& eigenvalues, 
          const blitz::Array<double,2>& eigenvectors, double p_variance);

        /**
         * Copy constructor
         */
        EigenMachine(const EigenMachine& other);

        /**
         * Assigment
         */
        EigenMachine & operator= (const EigenMachine &other);

        /**
         * Destructor
         */
        virtual ~EigenMachine(); 

        /// Set the feature dimensionality
        /// Overrides Machine::setNInputs
        void setNOutputs(int n_outputs);
        void setPVariance(double p_variance);

        /// Get number of inputs
        int getNOutputs() const;
        double getPVariance() const;

        void setEigenvaluesvectors(const blitz::Array<double,1>& eigenvalues,
          const blitz::Array<double,2>& eigenvectors);

        const blitz::Array<double,1>& getEigenvalues() const;
        const blitz::Array<double,2>& getEigenvectors() const;

        /// Remove means before the projection (in particular for PCA)
        void setPreMean(const blitz::Array<double,1>& eigenvalues);

        const blitz::Array<double,1>& getPreMean() const;

        /// Output the projected sample, x 
        /// (overrides Machine::forward)
        blitz::Array<double,1> forward(const FrameSample& input) const;

        /// Print the parameters of the GMM
        void print() const;

      protected:
        /// Copy another EigenMachine
        void copy(const EigenMachine&);

        // The percentage of variance to keep
        double m_p_variance;
        // The number of output to keep
        int m_n_outputs;

        // The eigenvalues and eigenvectors
        blitz::Array<double,1> m_eigenvalues;
        blitz::Array<double,2> m_eigenvectors;

        // Mean to be removed before the projection
        blitz::Array<double,1> m_pre_mean;
    };

  }
}

#endif