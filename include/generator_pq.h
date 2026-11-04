#ifndef GENERATOR_PQ_H
#define GENERATOR_PQ_H

#include <stdint.h>

/* 
 * Synchronous Generator P-Q Telemetry & Capability Engine
 * Coordinate System: 
 *   - Vertical Axis (Y): Active Power P (MW)
 *   - Horizontal Axis (X): Reactive Power Q (MVAR) 
 *     - Q > 0: Lagging PF (Exporting / Overexcited)
 *     - Q < 0: Leading PF (Importing / Underexcited)
 */
typedef struct {
    double p_mw;          /* Active Power (MW, Vertical Axis) */
    double q_mvar;        /* Reactive Power (MVAR, Horizontal Axis) */
    double v_pu;          /* Terminal Voltage (pu) */
    double s_base_mva;    /* MVA Base (e.g., 100.0 MVA) */
    
    /* Machine Limits */
    double p_turbine_max_mw; /* Prime Mover Turbine Power Limit (MW) */
    double i_stator_limit_pu;/* Max Stator Current limit (e.g., 1.0 pu) */
    double i_field_limit_pu; /* Max Rotor Field Current limit (e.g., 2.0 pu) */

    /* Computed Telemetry */
    double i_stator_pu;   /* Computed Stator Current (pu) */
    double i_field_pu;    /* Computed Rotor Field Current (pu) */
    
    /* Status Flags */
    int stator_ok;        /* 1 if within stator current limit */
    int rotor_ok;         /* 1 if within rotor field current limit */
    int turbine_ok;       /* 1 if within prime mover turbine limit */
} gen_telemetry_t;

/* Modbus Holding Register Map (Scaled integers: e.g., value * 100 for 2 decimal places) */
typedef struct {
    uint16_t reg_p_mw;         /* Offset 0: Active Power (MW * 100) */
    uint16_t reg_q_mvar;       /* Offset 1: Reactive Power (MVAR * 100, signed as uint16) */
    uint16_t reg_stator_amps;  /* Offset 2: Stator Current (pu * 1000) */
    uint16_t reg_field_amps;   /* Offset 3: Rotor Field Current (pu * 1000) */
    uint16_t reg_status_flags; /* Offset 4: Bit 0: Stator OK, Bit 1: Rotor OK, Bit 2: Turbine OK */
} modbus_holding_registers_t;

void gen_pq_init(void);
void gen_pq_evaluate(gen_telemetry_t *telemetry);
void modbus_map_telemetry(const gen_telemetry_t *telemetry, modbus_holding_registers_t *regs);

#endif /* GENERATOR_PQ_H */