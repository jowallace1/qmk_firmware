/*
Copyright 2021 0xC7

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
#include "gpio.h"
#include "matrix.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"
#include "config.h"

/* matrix state(1:on, 0:off) */
extern matrix_row_t matrix[MATRIX_ROWS];     // debounced values
extern matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
#define MATRIX_ROW_SHIFTER ((matrix_row_t)1)

static void setPinOutput_writeLow(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinOutput(pin);
        writePinLow(pin);
    }
}

static void setPinInputHigh_atomic(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        setPinInputHigh(pin);
    }
}

static uint8_t readMatrixPin(pin_t pin) {
    if (pin != NO_PIN) {
        return readPin(pin);
    } else {
        return 1;
    }
}

static bool select_row(uint8_t row) {
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
        setPinOutput_writeLow(pin);
        return true;
    }
    return false;
}

static void unselect_row(uint8_t row) { // taken from quantum/matrix.c
    pin_t pin = row_pins[row];
    if (pin != NO_PIN) {
#            ifdef MATRIX_UNSELECT_DRIVE_HIGH
        setPinOutput_writeHigh(pin);
#            else
        setPinInputHigh_atomic(pin);
#            endif
    }
}

void set_mux(uint8_t col) {
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

void matrix_init_custom(void) {
    // TODO: initialize hardware here
    matrix_init();
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    if (!select_row(current_row)) { // Select row
        return;                     // skip NO_PIN row
    }
    matrix_output_select_delay();

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        
        uint8_t pin_state = 0;

        if (col_index > 7) {    // read direct columns
            pin_state = readMatrixPin(col_pins[col_index]);
        } else {                // read columns from multiplexer
            set_mux(col_index);
            pin_state = readPin(MUX_OUT);
        }
        

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(current_row, current_row_value != 0); // wait for all Col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        matrix_read_cols_on_row(current_matrix, current_row);
    }

    return changed;
}