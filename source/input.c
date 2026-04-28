/**
 * @file source/input.c
 * @brief Input parameter reading for SVFT (Screened Vector-Tensor) parameters
 * @description Handles default initialization and .ini file parsing for SVFT parameters
 * @author Bùi Đình Hoàng (QTC Project)
 */

#include "class.h"

/**
 * ============================================================================
 * FUNCTION: input_default_params()
 * ============================================================================
 * @brief Initialize default values for all SVFT parameters (ΛCDM limit)
 * @param pba: pointer to background structure
 * @return: 0 on success
 * 
 * @description This ensures the code won't crash if parameters are missing
 *              from the .ini file. Default values correspond to ΛCDM limit
 *              where SVFT effects are turned off.
 */
int input_default_params(struct background *pba) {

    /**
     * SVFT Parameter Default Values (ΛCDM Limit)
     * =========================================
     */
    
    // a0_vft: Screening threshold acceleration (MOND standard)
    // Default: 1.0e-10 m/s² (turns OFF SVFT effects)
    pba->a0_vft = 1.0e-10;
    
    // lambda_vft: Coupling constant for scalar field interaction
    // Default: 0.0 (no coupling, pure ΛCDM)
    pba->lambda_vft = 0.0;
    
    // eps_vft: Planck mass run rate (Planck mass evolution parameter)
    // Default: 0.0 (no Planck mass evolution)
    pba->eps_vft = 0.0;
    
    // n_vft: Screening function exponent (power law index)
    // Default: 2.0 (standard chameleon/symmetron screening)
    pba->n_vft = 2.0;
    
    // rho_c_vft: Critical density threshold for screening mechanism
    // Default: 1.0e-10 kg/m³ (effectively disables screening)
    pba->rho_c_vft = 1.0e-10;

    return 0;
}

/**
 * ============================================================================
 * FUNCTION: input_read_parameters()
 * ============================================================================
 * @brief Read SVFT parameters from .ini configuration file
 * @param pba: pointer to background structure
 * @param param_file: path to configuration file
 * @return: 0 on success, error code on failure
 *
 * @description Parses .ini file and reads the following SVFT parameters:
 *   - a0_vft: screening threshold acceleration
 *   - lambda_vft: coupling constant
 *   - eps_vft: Planck mass run rate
 *   - n_vft: screening function exponent
 *   - rho_c_vft: critical density for screening
 *
 * @note Uses the CLASS macro class_read_double() for safe parsing
 */
int input_read_parameters(struct background *pba, char *param_file) {

    /**
     * SVFT Parameter Reading from .ini File
     * ======================================
     * 
     * Each class_read_double() call:
     * - Searches for key in param_file
     * - Returns 0 if key not found (uses default from input_default_params)
     * - Returns error if key found but value is invalid
     */
    
    // Read a0_vft: Screening threshold acceleration
    // If not in .ini, uses default value 1.0e-10
    class_read_double("a0_vft", pba->a0_vft);
    
    // Read lambda_vft: Coupling constant for scalar field
    // Controls strength of interaction between field and matter
    class_read_double("lambda_vft", pba->lambda_vft);
    
    // Read eps_vft: Planck mass run rate
    // Characterizes evolution of effective Planck mass
    class_read_double("eps_vft", pba->eps_vft);
    
    // Read n_vft: Screening function exponent
    // Power-law index: S(a) = 1/(1 + (rho/rho_c)^n)
    class_read_double("n_vft", pba->n_vft);
    
    // Read rho_c_vft: Critical density threshold
    // Density above which screening mechanism activates
    class_read_double("rho_c_vft", pba->rho_c_vft);

    return 0;
}

/**
 * ============================================================================
 * VALIDATION: input_validate_svft_params()
 * ============================================================================
 * @brief Validate SVFT parameter ranges
 * @param pba: pointer to background structure
 * @return: 0 if valid, error code if out of bounds
 *
 * @description Ensures SVFT parameters are physically reasonable
 */
int input_validate_svft_params(struct background *pba) {

    // a0_vft should be positive (acceleration magnitude)
    if (pba->a0_vft < 0.0) {
        fprintf(stderr, "ERROR: a0_vft must be positive (got %e)\n", pba->a0_vft);
        return _FAILURE_;
    }
    
    // lambda_vft should be between 0 and 1 (coupling strength)
    if ((pba->lambda_vft < 0.0) || (pba->lambda_vft > 1.0)) {
        fprintf(stderr, "WARNING: lambda_vft should be in [0,1] (got %e)\n", pba->lambda_vft);
    }
    
    // eps_vft should be small (< 0.1 for perturbativity)
    if (pba->eps_vft < 0.0 || pba->eps_vft > 0.1) {
        fprintf(stderr, "WARNING: eps_vft should be in [0, 0.1] (got %e)\n", pba->eps_vft);
    }
    
    // n_vft should be positive (power law exponent)
    if (pba->n_vft < 0.0) {
        fprintf(stderr, "ERROR: n_vft must be positive (got %e)\n", pba->n_vft);
        return _FAILURE_;
    }
    
    // rho_c_vft should be positive (density threshold)
    if (pba->rho_c_vft < 0.0) {
        fprintf(stderr, "ERROR: rho_c_vft must be positive (got %e)\n", pba->rho_c_vft);
        return _FAILURE_;
    }

    return _SUCCESS_;
}

/**
 * ============================================================================
 * HELPER: input_print_svft_params()
 * ============================================================================
 * @brief Print current SVFT parameter values (for debugging)
 * @param pba: pointer to background structure
 * @return: void
 */
void input_print_svft_params(struct background *pba) {
    
    printf("\n=== SVFT Parameters ===\n");
    printf("  a0_vft     = %.6e  [m/s²]\n", pba->a0_vft);
    printf("  lambda_vft = %.6e  [dimensionless]\n", pba->lambda_vft);
    printf("  eps_vft    = %.6e  [dimensionless]\n", pba->eps_vft);
    printf("  n_vft      = %.6e  [dimensionless]\n", pba->n_vft);
    printf("  rho_c_vft  = %.6e  [kg/m³]\n", pba->rho_c_vft);
    printf("=======================\n\n");
}
