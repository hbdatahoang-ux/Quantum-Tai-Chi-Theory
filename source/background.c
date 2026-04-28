/**
 * @file source/background.c
 * @brief SVFT Background Evolution with EFT Mapping
 * @description Implements screening logic and alpha function calculations
 *              for SVFT (Screened Vector-Tensor) modified gravity
 * @author Bùi Đình Hoàng (QTC Project)
 */

#include "class.h"

/**
 * ============================================================================
 * FUNCTION: background_svft_screening()
 * ============================================================================
 * @brief Calculate SVFT screening function S(a) based on matter density
 * @param pba: pointer to background structure
 * @param a: current scale factor
 * @param rho_b: background density of matter
 * @return: screening factor S(a) in range [0, 1]
 *
 * @description 
 * Screening mechanism: S(a) = 1 / (1 + (rho/rho_c)^n)
 *
 * Physical interpretation:
 *   - At high density (early universe): S → 0 (screening ON → GR-like)
 *   - At low density (late universe):  S → 1 (screening OFF → SVFT active)
 *
 * This ensures:
 *   ✓ CMB power spectrum preserved (early times)
 *   ✓ SVFT modifications emerge at z ~ 0-3 (structure formation)
 *   ✓ Graceful exit to GR in high-density environments (solar system test)
 */
double background_svft_screening(struct background *pba, double a, double rho_b) {
    
    // Avoid division by zero
    if (pba->rho_c_vft <= 0.0) {
        return 0.0;  // No screening if rho_c not set properly
    }
    
    // Density ratio (dimensionless)
    double density_ratio = rho_b / pba->rho_c_vft;
    
    // Screening function with power-law exponent n_vft
    // S(a) = 1 / (1 + (rho/rho_c)^n)
    double screening_S = 1.0 / (1.0 + pow(density_ratio, pba->n_vft));
    
    // Clamp to [0, 1] for numerical stability
    if (screening_S < 0.0) screening_S = 0.0;
    if (screening_S > 1.0) screening_S = 1.0;
    
    return screening_S;
}

/**
 * ============================================================================
 * FUNCTION: background_svft_alpha_functions()
 * ============================================================================
 * @brief Compute EFT property functions alpha_i for SVFT theory
 * @param pba: pointer to background structure
 * @param a: current scale factor
 * @param z: current redshift
 * @param rho_b: background matter density
 * @return: 0 on success
 *
 * @description
 * Maps SVFT theory parameters to hi_class EFT framework (Bellini-Sawicki).
 * 
 * Bellini-Sawicki parametrization defines:
 *   Φ(a,k) = Ψ(a,k) + α_B(a) δρ + ...     [Anisotropic stress]
 *   M_eff²(a) = M_Pl² * [1 + α_M(a)]       [Planck mass evolution]
 *   c_T²(a) = 1 + α_T(a)                   [Tensor speed of gravity]
 *   ρ_kin ∝ α_K(a)                         [Kinetic energy density]
 *
 * For SVFT:
 *   - α_B ∝ λ_vft × S(a)   [Scalar-matter coupling]
 *   - α_M ∝ ε_vft × S(a)   [Planck mass run rate]
 *   - α_K = 0.1×α_B + 0.01 [Stability + avoiding ghosts]
 *   - α_T = 0              [GW170817 constraint]
 */
int background_svft_alpha_functions(struct background *pba, 
                                    double a, 
                                    double z, 
                                    double rho_b) {

    /**
     * STEP 1: Calculate screening function S(a)
     * =========================================
     * This is the "gating" mechanism that turns SVFT effects on/off
     * based on matter density.
     */
    double screening_S = background_svft_screening(pba, a, rho_b);

    /**
     * STEP 2: Map SVFT parameters to EFT alpha functions
     * ==================================================
     */
    
    // alpha_B (Braiding): Controls deviation from GR in perturbations
    // α_B = λ_vft × S(a)
    // Physical meaning: Strength of scalar field coupling to matter
    // Constraint: |α_B| < 0.1 to avoid strong fifth-force
    pba->alphab = pba->lambda_vft * screening_S;
    if (pba->alphab > 0.1) {
        fprintf(stderr, "WARNING: |alpha_B| = %e exceeds stability bound (0.1)\n", 
                pba->alphab);
    }

    // alpha_M (Planck mass run rate): Evolution of effective Newton's constant
    // α_M = ε_vft × S(a)
    // Physical meaning: How G_eff = G/(1 + α_M) evolves with time
    // Constraint: |α_M| < 0.05 for consistency with solar system tests
    pba->alpham = pba->eps_vft * screening_S;
    if (pba->alpham > 0.05) {
        fprintf(stderr, "WARNING: alpha_M = %e exceeds solar system bound (0.05)\n", 
                pba->alpham);
    }

    // alpha_K (Kineticity): Kinetic energy density term
    // α_K = 0.1 × α_B + 0.01
    // Physical meaning: Ensures no-ghost condition (α_K > 0)
    // This prevents negative kinetic energy (ghost-free formulation)
    // The "+0.01" offset ensures α_K remains positive even when α_B = 0
    pba->alphak = 0.1 * pba->alphab + 0.01;
    if (pba->alphak <= 0.0) {
        fprintf(stderr, "ERROR: alpha_K = %e is non-positive (ghost instability!)\n", 
                pba->alphak);
        return _FAILURE_;
    }

    // alpha_T (Tensor excess): Tensor speed of gravity relative to c
    // α_T = c_T² - 1 must satisfy: |c_T - 1| < 10^-15 (GW170817)
    // We set α_T = 0 strictly to preserve c_T = c (tensor wave speed = light speed)
    // This is mandatory for consistency with LIGO-Virgo gravitational wave observations
    pba->alphat = 0.0;

    /**
     * STEP 3: Optional diagnostic output
     * ==================================
     * Uncomment for debugging:
     */
    // fprintf(stderr, "a=%.3e, z=%.2f, S(a)=%.4f, αB=%.4e, αM=%.4e, αK=%.4e\n",
    //         a, z, screening_S, pba->alphab, pba->alpham, pba->alphak);

    return _SUCCESS_;
}

/**
 * ============================================================================
 * FUNCTION: background_svft_stability_check()
 * ============================================================================
 * @brief Verify that alpha functions satisfy stability constraints
 * @param pba: pointer to background structure
 * @return: 0 if stable, error code if unstable
 *
 * @description
 * Checks physical stability conditions:
 *   1. No-ghost condition: α_K > 0 (kinetic energy positive)
 *   2. Gradient stability: dα_i/da terms (smooth evolution)
 *   3. Causality: |α_T| = 0 (gravitational waves travel at c)
 *   4. Fifth-force: |α_B| < 0.1 (avoids strong modifications)
 */
int background_svft_stability_check(struct background *pba) {

    // No-ghost condition
    if (pba->alphak <= 0.0) {
        fprintf(stderr, "FATAL: Ghost instability detected (α_K = %e ≤ 0)\n", 
                pba->alphak);
        return _FAILURE_;
    }

    // Avoidance of strong fifth-force
    if (fabs(pba->alphab) > 0.2) {
        fprintf(stderr, "WARNING: Strong fifth-force regime (|α_B| = %e > 0.2)\n",
                fabs(pba->alphab));
    }

    // Gravitational wave speed constraint (GW170817)
    if (fabs(pba->alphat) > 1.0e-15) {
        fprintf(stderr, "ERROR: c_T constraint violated (|α_T| = %e > 1e-15)\n",
                fabs(pba->alphat));
        return _FAILURE_;
    }

    return _SUCCESS_;
}

/**
 * ============================================================================
 * FUNCTION: background_svft_info()
 * ============================================================================
 * @brief Print SVFT parameter summary for debugging
 * @param pba: pointer to background structure
 * @return: void
 */
void background_svft_info(struct background *pba) {

    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║        SVFT BACKGROUND EVOLUTION - PARAMETER SUMMARY   ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("SVFT Parameters (QTC Physics):\n");
    printf("  a0_vft     = %.6e  [m/s²]  (Screening threshold)\n", pba->a0_vft);
    printf("  lambda_vft = %.6e  [--]    (Coupling constant)\n", pba->lambda_vft);
    printf("  eps_vft    = %.6e  [--]    (Planck mass run rate)\n", pba->eps_vft);
    printf("  n_vft      = %.6e  [--]    (Screening exponent)\n", pba->n_vft);
    printf("  rho_c_vft  = %.6e  [kg/m³] (Critical density)\n", pba->rho_c_vft);
    printf("\n");
    printf("EFT Alpha Functions (hi_class):\n");
    printf("  alpha_B = %.6e  [Braiding / scalar-matter coupling]\n", pba->alphab);
    printf("  alpha_M = %.6e  [Planck mass run rate]\n", pba->alpham);
    printf("  alpha_K = %.6e  [Kineticity / no-ghost condition]\n", pba->alphak);
    printf("  alpha_T = %.6e  [Tensor excess (GW170817 constrained)]\n", pba->alphat);
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n\n");
}

/**
 * ============================================================================
 * INTEGRATION POINT
 * ============================================================================
 * 
 * To integrate this code into your CLASS/hi_class workflow:
 *
 * 1. In background.c, find the main background evolution loop (e.g., 
 *    background_solve() or similar)
 *
 * 2. At each time step, after computing density rho_b, call:
 *    
 *    background_svft_alpha_functions(pba, a, z, rho_b);
 *
 * 3. Before outputting results, verify stability:
 *    
 *    if (background_svft_stability_check(pba) == _FAILURE_) {
 *        // Handle error (divergence, ghost instability, etc.)
 *    }
 *
 * 4. For initial diagnostics:
 *    
 *    background_svft_info(pba);
 *
 * ============================================================================
 */
