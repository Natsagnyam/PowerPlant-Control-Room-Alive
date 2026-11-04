#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define PI 3.14159265358979323846

typedef struct {
    double p_mw;           /* MW (Vertical Axis) */
    double q_mvar;         /* MVAR (Horizontal Axis) */
    double v_pu;           /* Terminal Voltage (pu) */
    double s_base_mva;     /* MVA Base */
    double xd_pu;          /* Direct Axis Reactance (pu) */
    double xq_pu;          /* Quadrature Axis Reactance (pu) */
    
    /* Calculated */
    double i_stator_pu;
    double i_field_pu;
    double power_factor;
    double load_angle_deg;
    double mvar_margin_pu; /* Margin to SSSL */
    int within_sssl;
} gen_state_t;

void calculate_machine_state(gen_state_t *st) {
    double p_pu = st->p_mw / st->s_base_mva;
    double q_pu = st->q_mvar / st->s_base_mva;
    double v_pu = st->v_pu;

    /* Stator Current */
    double s_pu = sqrt(p_pu * p_pu + q_pu * q_pu);
    st->i_stator_pu = s_pu / v_pu;
    st->power_factor = (s_pu > 0) ? (p_pu / s_pu) : 1.0;

    /* Load Angle (Delta) calculation considering saliency */
    double num = p_pu * st->xq_pu;
    double den = (v_pu * v_pu) + (q_pu * st->xq_pu);
    double delta_rad = atan2(num, den);
    st->load_angle_deg = delta_rad * (180.0 / PI);

    /* Field Current (If) excitation voltage model */
    double id_pu = st->i_stator_pu * sin(delta_rad + acos(st->power_factor));
    st->i_field_pu = (v_pu * cos(delta_rad) + id_pu * st->xd_pu) / st->xd_pu;

    /* Steady-State Stability Limit (SSSL at delta = 90 deg) */
    double q_limit_pu = -(v_pu * v_pu) / st->xd_pu; 
    st->mvar_margin_pu = q_pu - q_limit_pu;
    st->within_sssl = (q_pu >= q_limit_pu) ? 1 : 0;
}

void export_json(const char *filename, const gen_state_t *st) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "{\n");
    fprintf(f, "  \"p_mw\": %.2f,\n", st->p_mw);
    fprintf(f, "  \"q_mvar\": %.2f,\n", st->q_mvar);
    fprintf(f, "  \"v_pu\": %.3f,\n", st->v_pu);
    fprintf(f, "  \"i_stator_pu\": %.3f,\n", st->i_stator_pu);
    fprintf(f, "  \"i_field_pu\": %.3f,\n", st->i_field_pu);
    fprintf(f, "  \"power_factor\": %.3f,\n", st->power_factor);
    fprintf(f, "  \"load_angle_deg\": %.2f,\n", st->load_angle_deg);
    fprintf(f, "  \"xd_pu\": %.2f,\n", st->xd_pu);
    fprintf(f, "  \"xq_pu\": %.2f,\n", st->xq_pu);
    fprintf(f, "  \"mvar_margin_pu\": %.3f,\n", st->mvar_margin_pu);
    fprintf(f, "  \"within_sssl\": %d\n", st->within_sssl);
    fprintf(f, "}\n");

    fclose(f);
}

int main(void) {
    gen_state_t state = {
        .p_mw = 400.0,
        .q_mvar = 150.0,
        .v_pu = 1.0,
        .s_base_mva = 500.0,
        .xd_pu = 3.0,
        .xq_pu = 1.0
    };

    printf("[BBB C Telemetry Server] Started. Updating www/telemetry.json...\n");

    while (1) {
        calculate_machine_state(&state);
        export_json("www/telemetry.json", &state);
        usleep(200000); /* Update every 200ms */
    }

    return 0;
}