#!/bin/bash
set -e

echo "Creating project directory structure..."
mkdir -p include src jpeg

# Create header in include/
cat << 'HEADER_EOF' > include/generator_pq.h
#ifndef GENERATOR_PQ_H
#define GENERATOR_PQ_H

#include <stdint.h>

/* Synchronous Generator Parameters (PU / Real Units) */
typedef struct {
    double p_mw;        /* Active Power (MW) */
    double q_mvar;      /* Reactive Power (MVAR, >0 lagging, <0 leading) */
    double v_pu;        /* Terminal Voltage (pu) */
    double i_stator_pu; /* Stator Current (pu) */
} gen_telemetry_t;

void gen_pq_init(void);
void gen_pq_update(const gen_telemetry_t *telemetry);

#endif /* GENERATOR_PQ_H */
HEADER_EOF

#Create source in src/
cat << 'SRC_EOF' > src/main.c
#include <stdio.h>
#include "../include/generator_pq.h"

void gen_pq_init(void) {
    printf("[BBB PQ Engine] Initialized P-Q Telemetry Engine.\n");
}

void gen_pq_update(const gen_telemetry_t *telemetry) {
    printf("[Telemetry] P: %.2f MW | Q: %.2f MVAR | I_stator: %.3f pu\n",
           telemetry->p_mw, telemetry->q_mvar, telemetry->i_stator_pu);
}

int main(void) {
    gen_pq_init();
    
    gen_telemetry_t sample = {
        .p_mw = 100.0,
        .q_mvar = 25.0,
        .v_pu = 1.0,
        .i_stator_pu = 1.0
    };

    gen_pq_update(&sample);
    return 0;
}
SRC_EOF

# Create placeholder documentation in jpeg/
cat << 'IMG_EOF' > jpeg/README.md
# Visual Assets & Artifacts

Place P-Q plot exports, stator capability diagrams, and JPEG screen captures here.
IMG_EOF

chmod +x include/generator_pq.h src/main.c jpeg/README.md
echo "Project structure successfully created!"
tree . 2>/dev/null || ls -la R include src jpeg
