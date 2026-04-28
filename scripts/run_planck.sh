#!/bin/bash

# ================================================================
# SVFT / QTC PARALLEL EXECUTION SCRIPT
# Optimized for HP Victus (WSL2) - Project by Bùi Đình Hoàng
# ================================================================

set -e  # Exit on any error

# Configuration and setup
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="$PROJECT_ROOT/output"
LOG_DIR="$PROJECT_ROOT/logs"
CLASS_CONFIG="$PROJECT_ROOT/class_svft/input/svft_planck.ini"

# Create necessary directories
mkdir -p "$OUTPUT_DIR"
mkdir -p "$LOG_DIR"

# ================================================================
# 1. RESOURCE CONFIGURATION
# ================================================================
# Auto-detect max CPU threads (e.g., Ryzen 7 = 16 threads)
THREADS=$(nproc)
export OMP_NUM_THREADS=$THREADS

# Memory optimization
export OMP_STACKSIZE=256M

# Current timestamp for logging
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
LOG_FILE="$LOG_DIR/run_svft_${TIMESTAMP}.log"

echo "================================================================" | tee "$LOG_FILE"
echo "🚀 SVFT / QTC COSMOLOGICAL SIMULATION - PARALLEL EXECUTION" | tee -a "$LOG_FILE"
echo "================================================================" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
echo "📊 System Configuration:" | tee -a "$LOG_FILE"
echo "  CPU Threads:       $THREADS" | tee -a "$LOG_FILE"
echo "  Stack Size:        $OMP_STACKSIZE" | tee -a "$LOG_FILE"
echo "  Project Root:      $PROJECT_ROOT" | tee -a "$LOG_FILE"
echo "  Config File:       $CLASS_CONFIG" | tee -a "$LOG_FILE"
echo "  Output Directory:  $OUTPUT_DIR" | tee -a "$LOG_FILE"
echo "  Log File:          $LOG_FILE" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# ================================================================
# 2. COMPILATION STEP
# ================================================================
echo "🛠 Compilation Phase:" | tee -a "$LOG_FILE"
echo "  Checking and compiling source code..." | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

cd "$PROJECT_ROOT"

# Clean previous builds (optional - comment out if you want incremental builds)
# make clean

# Compile with all available threads
if make -j"$THREADS" 2>&1 | tee -a "$LOG_FILE"; then
    echo "✅ Compilation successful!" | tee -a "$LOG_FILE"
else
    echo "❌ Compilation failed!" | tee -a "$LOG_FILE"
    echo "Please check the errors above and verify:" | tee -a "$LOG_FILE"
    echo "  - background.h (parameter declarations)" | tee -a "$LOG_FILE"
    echo "  - source/input.c (parameter reading)" | tee -a "$LOG_FILE"
    echo "  - source/background.c (screening logic)" | tee -a "$LOG_FILE"
    exit 1
fi

echo "" | tee -a "$LOG_FILE"

# ================================================================
# 3. PRE-SIMULATION CHECKS
# ================================================================
echo "📋 Pre-simulation Checks:" | tee -a "$LOG_FILE"

if [ ! -f "$CLASS_CONFIG" ]; then
    echo "❌ ERROR: Config file not found: $CLASS_CONFIG" | tee -a "$LOG_FILE"
    exit 1
fi
echo "✓ Config file found: $CLASS_CONFIG" | tee -a "$LOG_FILE"

if [ ! -f "class" ]; then
    echo "❌ ERROR: Executable 'class' not found in $PROJECT_ROOT" | tee -a "$LOG_FILE"
    exit 1
fi
echo "✓ Executable 'class' found" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"

# ================================================================
# 4. MAIN SIMULATION EXECUTION
# ================================================================
echo "================================================================" | tee -a "$LOG_FILE"
echo "🌌 MAIN SIMULATION: SVFT CMB + Matter Power Spectrum Calculation" | tee -a "$LOG_FILE"
echo "================================================================" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

START_TIME=$(date +%s)

# Run CLASS with SVFT configuration
echo "Executing: ./class $CLASS_CONFIG" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

if ./class "$CLASS_CONFIG" 2>&1 | tee -a "$LOG_FILE"; then
    SIMULATION_STATUS="SUCCESS"
    echo "" | tee -a "$LOG_FILE"
    echo "✅ SIMULATION COMPLETED SUCCESSFULLY!" | tee -a "$LOG_FILE"
else
    SIMULATION_STATUS="FAILED"
    echo "" | tee -a "$LOG_FILE"
    echo "❌ SIMULATION FAILED!" | tee -a "$LOG_FILE"
    echo "" | tee -a "$LOG_FILE"
    echo "Common issues:" | tee -a "$LOG_FILE"
    echo "  1. Physical parameters out of bounds (check svft_planck.ini)" | tee -a "$LOG_FILE"
    echo "  2. Ghost instability (α_K ≤ 0 - check background.c screening)" | tee -a "$LOG_FILE"
    echo "  3. Numerical divergence (try smaller epsilon values)" | tee -a "$LOG_FILE"
    exit 1
fi

END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

# ================================================================
# 5. POST-SIMULATION ANALYSIS
# ================================================================
echo "" | tee -a "$LOG_FILE"
echo "================================================================" | tee -a "$LOG_FILE"
echo "📊 POST-SIMULATION ANALYSIS" | tee -a "$LOG_FILE"
echo "================================================================" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# Check output files
echo "Output Files Generated:" | tee -a "$LOG_FILE"
ls -lh "$OUTPUT_DIR"/svft_planck_v1_* 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' | tee -a "$LOG_FILE"

# Display key results
if [ -f "$OUTPUT_DIR/svft_planck_v1_cl.dat" ]; then
    echo "" | tee -a "$LOG_FILE"
    echo "✓ CMB Power Spectrum: $(wc -l < "$OUTPUT_DIR/svft_planck_v1_cl.dat") multipole moments" | tee -a "$LOG_FILE"
fi

if [ -f "$OUTPUT_DIR/svft_planck_v1_pk.dat" ]; then
    echo "✓ Matter Power Spectrum: $(wc -l < "$OUTPUT_DIR/svft_planck_v1_pk.dat") k-bins" | tee -a "$LOG_FILE"
fi

if [ -f "$OUTPUT_DIR/svft_planck_v1_parameters.ini" ]; then
    echo "✓ Parameter Log: Saved" | tee -a "$LOG_FILE"
fi

echo "" | tee -a "$LOG_FILE"
echo "⏱ Execution Time: ${ELAPSED}s" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# ================================================================
# 6. VISUALIZATION INSTRUCTIONS
# ================================================================
echo "================================================================" | tee -a "$LOG_FILE"
echo "🎨 VISUALIZATION & NEXT STEPS" | tee -a "$LOG_FILE"
echo "================================================================" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
echo "To visualize CMB spectrum (Python):" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
cat << 'EOF' | tee -a "$LOG_FILE"
  python3 << 'PYTHON_EOF'
  import matplotlib.pyplot as plt
  import numpy as np

  # Load CMB power spectrum
  try:
      data = np.loadtxt('output/svft_planck_v1_cl.dat')
      l = data[:, 0]
      cl_tt = data[:, 1]

      plt.figure(figsize=(12, 7))
      plt.plot(l, l*(l+1)*cl_tt/(2*np.pi)*1e12, 'b-', linewidth=1.5, 
               label='SVFT/QTC Model')
      plt.title('CMB Temperature Power Spectrum - SVFT Theory', fontsize=14)
      plt.xlabel('Multipole Moment $\ell$', fontsize=12)
      plt.ylabel(r'$D_\ell^{TT}$ [$\mu K^2$]', fontsize=12)
      plt.legend(fontsize=11)
      plt.grid(True, alpha=0.3)
      plt.xscale('log')
      plt.tight_layout()
      plt.savefig('svft_cmb_spectrum.png', dpi=150)
      print("✓ CMB spectrum saved to: svft_cmb_spectrum.png")
      plt.show()
  except Exception as e:
      print(f"Error reading data: {e}")
  PYTHON_EOF
EOF

echo "" | tee -a "$LOG_FILE"
echo "To check Matter Power Spectrum:" | tee -a "$LOG_FILE"
echo "  tail -20 output/svft_planck_v1_pk.dat" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
echo "📚 Full log available at: $LOG_FILE" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"

# ================================================================
# 7. FINAL STATUS
# ================================================================
echo "================================================================" | tee -a "$LOG_FILE"
if [ "$SIMULATION_STATUS" = "SUCCESS" ]; then
    echo "🎉 PIPELINE COMPLETE - SVFT SIMULATION SUCCESSFUL!" | tee -a "$LOG_FILE"
    echo "" | tee -a "$LOG_FILE"
    echo "🌌 Your SVFT/QTC theory is now running on cosmological scales!" | tee -a "$LOG_FILE"
    echo "" | tee -a "$LOG_FILE"
    echo "Next: Compare with Planck 2018 data using MontePython MCMC" | tee -a "$LOG_FILE"
else
    echo "⚠️  SIMULATION ENCOUNTERED ISSUES - CHECK LOGS" | tee -a "$LOG_FILE"
fi
echo "================================================================" | tee -a "$LOG_FILE"

exit 0
