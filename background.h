#ifndef BACKGROUND_H
#define BACKGROUND_H

/**
 * @file background.h
 * @brief Background cosmology structure and SVFT parameters
 * @description Contains definitions for cosmological background evolution
 *              and Symmetron Vector Field Theory (SVFT) parameters
 */

struct Background {
    /**
     * Standard cosmological parameters
     */
    double Omega_m;        // Matter density parameter
    double Omega_lambda;   // Dark energy density parameter
    double H0;             // Hubble constant (km/s/Mpc)
    double a_initial;      // Initial scale factor
    
    /**
     * SVFT (Symmetron Vector Field Theory) Parameters
     * @brief Screening mechanisms for modified gravity theories
     */
    double a0_vft;         // Acceleration/density screening threshold
                           // Controls the transition between screened 
                           // and unscreened regimes
    
    double lambda_vft;     // Coupling constant (linking strength)
                           // Determines the strength of interaction
                           // between the scalar field and matter
    
    double eps_vft;        // Planck mass run rate
                           // Characterizes how the effective Planck mass
                           // evolves with scale or time
    
    double n_vft;          // Screening function exponent
                           // Power-law index for the screening mechanism
                           // Controls the form of the chameleon/symmetron effect
    
    /**
     * Constructor with default values
     */
    Background() 
        : Omega_m(0.3), 
          Omega_lambda(0.7), 
          H0(67.4),
          a_initial(1.0),
          a0_vft(1.0),
          lambda_vft(1.0),
          eps_vft(1e-2),
          n_vft(2.0)
    {}
};

#endif // BACKGROUND_H
