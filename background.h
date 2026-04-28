#ifndef BACKGROUND_H
#define BACKGROUND_H

/**
 * @file background.h
 * @brief Background cosmology structure and SVFT parameters
 * @description Contains definitions for cosmological background evolution
 *              and Screened Vector-Tensor (SVFT) parameters
 */

struct Background {
    /**
     * Standard cosmological parameters
     */
    double Omega_m;        // Tham số mật độ vật chất
    double Omega_lambda;   // Tham số mật độ năng lượng tối
    double H0;             // Hằng số Hubble (km/s/Mpc)
    double a_initial;      // Hệ số tỷ lệ ban đầu
    
    /**
     * SVFT (Screened Vector-Tensor) Parameters
     * Added by Bùi Đình Hoàng for QTC validation
     * Screening mechanisms for modified gravity theories
     */
    double a0_vft;         // Ngưỡng gia tốc sàng lọc (acceleration/density screening threshold)
                           // Controls the transition between screened and unscreened regimes
    
    double lambda_vft;     // Hệ số liên kết (coupling constant)
                           // Determines the strength of interaction between the scalar field and matter
    
    double eps_vft;        // Planck mass run rate (epsilon)
                           // Characterizes how the effective Planck mass evolves with scale or time
    
    double n_vft;          // Số mũ của hàm sàng lọc (screening function exponent)
                           // Power-law index for the screening mechanism
    
    double rho_c_vft;      // Mật độ tới hạn cho screening (critical density for screening)
                           // Threshold density value for activating the screening mechanism
    
    /**
     * Constructor với giá trị mặc định
     */
    Background() 
        : Omega_m(0.3), 
          Omega_lambda(0.7), 
          H0(67.4),
          a_initial(1.0),
          a0_vft(1.0),
          lambda_vft(1.0),
          eps_vft(1e-2),
          n_vft(2.0),
          rho_c_vft(1e-27)
    {}
};

#endif // BACKGROUND_H
