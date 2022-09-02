/*
Copyright 2012-2018 Jun Wako, Jack Humbert, Yiancar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"
#include "config.h"

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

static void set_mux(uint8_t col) {
    setPinOutput(MUX_A);
    setPinOutput(MUX_B);
    setPinOutput(MUX_C);

    switch (col) {
        case 0:
            writePin(MUX_A, 0);
            writePin(MUX_B, 0);
            writePin(MUX_C, 0);
            break;
        case 1:
            writePin(MUX_A, 0);
            writePin(MUX_B, 0);
            writePin(MUX_C, 1);
            break;
        case 2:
            writePin(MUX_A, 0);
            writePin(MUX_B, 1);
            writePin(MUX_C, 0);
            break;
        case 3:
            writePin(MUX_A, 0);
            writePin(MUX_B, 1);
            writePin(MUX_C, 1);
            break;
        case 4:
            writePin(MUX_A, 1);
            writePin(MUX_B, 0);
            writePin(MUX_C, 0);
            break;
        case 5:
            writePin(MUX_A, 1);
            writePin(MUX_B, 0);
            writePin(MUX_C, 1);
            break;
        case 6:
            writePin(MUX_A, 1);
            writePin(MUX_B, 1);
            writePin(MUX_C, 0);
            break;
        case 7:
            writePin(MUX_A, 1);
            writePin(MUX_B, 1);
            writePin(MUX_C, 1);
            break;
    }
}

static void select_row(uint8_t row) {
    setPinOutput(row_pins[row]);
    writePinLow(row_pins[row]); // pins are active low
}

static void deselect_row(uint8_t row) {
    setPinInputHigh(row_pins[row]);
}

static void deselect_rows(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        deselect_row(row);
    }
}

static bool readMux(uint8_t col) {
    set_mux(col);
    return readPin(MUX_OUT);
}

static void matrix_init_pins(void) {
    for (int col = 0; col < MATRIX_COLS; col++) {
        pin_t pin = col_pins[col];
        if (pin != NO_PIN) {
            setPinInputHigh(pin);
        }
    }
}

void matrix_init_custom(void) {
    matrix_init_pins();
    debounce_init(MATRIX_ROWS);
    deselect_rows();

    wait_ms(50);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;
    xprintf("custom matrix scanning...\n");

    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        matrix_row_t last_row_val = current_matrix[current_row];
        select_row(current_row);

        for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++) {
            bool pinVal = 0;

            if (current_col < MUX_PINS) {
                pinVal = readMux(current_col);
            } else {
                pinVal = readPin(col_pins[current_col - MUX_PINS]);
            }

            current_matrix[current_row] |= pinVal ? 0 : MATRIX_ROW_SHIFTER << current_col;
        }

        matrix_has_changed |= last_row_val != current_matrix[current_row];

        deselect_rows();
    }

    return matrix_has_changed;
}