/* Copyright 2013 Gregoire Delannoy
 * 
 * This file is a part of TCPeP.
 * 
 * TCPeP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "galois_field.h"
#include "matrix.h"
#include "utils.h"


matrix* mCreate(int rows, int columns){
    int i;
    matrix* resultMatrix;
    resultMatrix = malloc(sizeof(matrix));
    resultMatrix->nRows = rows;
    resultMatrix->nColumns = columns;
    // In case of a 0 matrix, do not allocate
    if(rows != 0){
        resultMatrix->data = malloc(rows * sizeof(uint8_t**));
        for(i = 0; i < resultMatrix->nRows; i++){
            resultMatrix->data[i] = calloc(resultMatrix->nColumns, sizeof(uint8_t));
        }
    }
    return resultMatrix;
}

matrix* getIdentityMatrix(int rows){ // Returns square identity matrix
    int i, j;
    matrix* resultMatrix = mCreate(rows, rows);
    for(i = 0; i < resultMatrix->nRows; i++){
        for(j = 0; j < resultMatrix->nColumns; j++){
            if(i==j){
                resultMatrix->data[i][j] = 1;
            } else {
                resultMatrix->data[i][j] = 0;
            }
        }
    }
    return resultMatrix;
}

matrix* getRandomMatrix(int rows, int columns){
    int i, j;
    matrix* resultMatrix = mCreate(rows, columns);

    for(i = 0; i < resultMatrix->nRows; i++){
        for(j = 0; j < resultMatrix->nColumns; j++){
            resultMatrix->data[i][j] = getRandom();
        }
    }

    return resultMatrix;
}

void mPrint(matrix m){
    int i, j;
    printf("rows = %d, columns = %d\n", m.nRows, m.nColumns);
    if(m.nColumns > MAX_PRINT){
        printf("Yo Matrix so fat I not ev' gonna print it.\n");
        return;
    }

    for(i = 0; i < m.nRows; i++){
        printf("|");
        for(j = 0; j < m.nColumns; j++){
            printf(" %2x ", m.data[i][j]);
        }
        printf("|\n");
    }
}

void mFree(matrix* m){
    int i;
    for(i = 0; i < m->nRows; i++){
        free(m->data[i]);
    }
    if(m->nRows > 0){
        free(m->data);
    }
    free(m);
}

// mMul memory-wise
matrix* mMul(matrix a, matrix b){
    int i, j, k;
    matrix* resultMatrix;
    uint8_t factor;
    uint8_t *aVector, *bVector, *resVector;

    // Check dimension correctness
    if(a.nColumns != b.nRows){
        printf("mMul : Error in Matrix dimensions. Cannot continue\n");
        exit(1);
    }

    resultMatrix = mCreate(a.nRows, b.nColumns);

    for(i = 0; i < resultMatrix->nRows; i++){
        aVector = a.data[i];
        resVector = resultMatrix->data[i];
        for(j = 0; j < a.nColumns; j++){
            factor = aVector[j];
            if(factor != 0x00){
                bVector = b.data[j];
                for(k = 0; k < b.nColumns; k++){
                    resVector[k] = gadd(resVector[k], gmul(factor, bVector[k]));
                } 
            }
        }
    }
    return resultMatrix;
}


matrix* mCopy(matrix orig){
    int i;
    matrix* resultMatrix = mCreate(orig.nRows, orig.nColumns);

    for(i = 0; i < resultMatrix->nRows; i++){
        memcpy(resultMatrix->data[i], orig.data[i], resultMatrix->nColumns * sizeof(uint8_t));
    }

    return resultMatrix;
}

int mEqual(matrix a, matrix b){
    int i,j;
    // Test dimensions
    if((a.nRows != b.nRows) || (a.nColumns != b.nColumns)){
    return false;
    }

    // Test each cell
    for(i = 0; i < a.nRows; i++){
        for(j = 0; j < a.nColumns; j++){
            if(a.data[i][j] != b.data[i][j]){
                return false;
            }
        }
    }

    // No difference has been found, return true.
    return true;
}

void rowReduce(uint8_t* row, uint8_t factor, int size){
    // Reduce a row st row[i] = row[i] / factor
    if(factor != 0x01){
        int i;
        for(i = 0; i < size ; i++){
            row[i] = gdiv(row[i], factor);
        }
    }
}

void rowMulSub(uint8_t* a, uint8_t* b, uint8_t coeff, int size){
    if(coeff != 0x00){
        // a = a - b*c
        int i;
        for(i = 0; i < size ; i++){
            a[i] = gsub(a[i], gmul(coeff, b[i]));
        }
    }
}
