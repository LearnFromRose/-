#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

/* 
 *  定义方阵的数据结构
 */
typedef struct
{
    /* data */
    int **m; //二重指针指向矩阵
    int row; //矩阵的行
    int col; //矩阵的列数
} matrix;

/* 
 *  定义扩展欧几里得数据结构（模26）
 */
typedef struct
{
    int gcd;     //最大公因数
    int inverse; //乘法逆元
} Euclid;

/* 
 *  创建n维方阵
 */
matrix *create_matrix(int row, int col)
{
    int i, j;
    srand((unsigned)time(NULL));
    int **m = (int **)malloc(sizeof(int *) * row);
    for (i = 0; i < row; i++)
    {
        m[i] = (int *)malloc(sizeof(int) * col);
        for (j = 0; j < col; j++)
        {
            // 生成随机数
            m[i][j] = (int)(rand() % 26);
        }
    }
    matrix *mat = (matrix *)malloc(sizeof(matrix));
    mat->m = m;
    mat->row = row;
    mat->col = col;
    return mat;
}

/* 
 *  销毁矩阵
 */
void destory_matrix(matrix *mat)
{
    int i, j;
    for (i = 0; i < mat->row; i++)
    {
        free((mat->m)[i]);
    }
    free(mat);
}

/* 
 *  打印矩阵
 */
void print_matrix(matrix *mat)
{
    int i, j;
    printf("matrix:\n");
    if (mat == NULL)
    {
        printf("NULL");
    }
    else
    {
        for (i = 0; i < mat->row; i++)
        {
            for (j = 0; j < mat->col; j++)
            {
                printf("%d\t", (mat->m)[i][j]);
            }
            printf("\n");
        }
    }
}

/* 
 *  计算行列式,始终按第一行展开，递归计算(行列式必为方阵)
 */
int calculate_determinant(matrix *matr)
{
    int i, j, n, x, y;
    //复制一次参数，让在函数中创建的矩阵单元可以被销毁
    n = matr->row;
    int **ma = (int **)malloc(sizeof(int *) * n);
    for (i = 0; i < n; i++)
    {
        ma[i] = (int *)malloc(sizeof(int) * n);
        for (j = 0; j < n; j++)
        {
            ma[i][j] = matr->m[i][j];
        }
    }
    matrix *mat = (matrix *)malloc(sizeof(matrix));
    mat->m = ma;
    mat->row = mat->col = n;
    // 若为一维矩阵,直接得到行列式的值
    if (mat->row == 1)
    {
        return (mat->m)[0][0];
    }

    int det = 0.0;
    for (i = 0; i < mat->row; i++)
    {
        if ((mat->m)[0][i] == 0)
        {
            det = det + 0;
        }
        else
        {
            //构造新的余子式
            matrix *new_mat = (matrix *)malloc(sizeof(matrix));
            n = mat->row - 1;
            int **m = (int **)malloc(sizeof(int *) * n);
            for (x = 0; x < n; x++)
            {
                m[x] = (int *)malloc(sizeof(int) * n);
                for (y = 0; y < n; y++)
                {
                    if (y < i) //余子式该部分不需要变化
                    {
                        m[x][y] = (mat->m)[x + 1][y];
                    }
                    else //余子式该部分需要向前移一位
                    {
                        m[x][y] = (mat->m)[x + 1][y + 1];
                    }
                }
            }
            new_mat->m = m;
            new_mat->row = new_mat->col = n;

            //判断正负取值
            if ((1 + i + 1) % 2 == 0)
            {
                det = (det + (mat->m)[0][i] * calculate_determinant(new_mat)) % 26;
            }
            else
            {
                det = (det - (mat->m)[0][i] * calculate_determinant(new_mat)) % 26;
                if (det < 0)
                    det = det + 26;
            }
        }
    }
    //本次中创建的矩阵不再使用可以销毁
    for (i = 0; i < mat->row; i++)
    {
        free((mat->m)[i]);
    }
    free(mat->m);
    free(mat);

    return det;
}

/* 
 *  求伴随矩阵(必为方阵),注意转置
 */
matrix *calculate_adjoint_matrix(matrix *mat)
{
    int i, j, n, x, y;
    //初始化伴随矩阵
    n = mat->row;
    int **ma = (int **)malloc(sizeof(int *) * n);
    for (i = 0; i < n; i++)
    {
        ma[i] = (int *)malloc(sizeof(int) * n);
        for (j = 0; j < n; j++)
        {
            ma[i][j] = 0;
        }
    }
    matrix *matr = (matrix *)malloc(sizeof(matrix));
    matr->m = ma;
    matr->row = matr->col = n;

    //计算伴随矩阵
    for (i = 0; i < mat->row; i++)
    {
        for (j = 0; j < mat->row; j++)
        {
            //构造出余子式矩阵
            matrix *new_mat = (matrix *)malloc(sizeof(matrix));
            n = mat->row - 1;
            int **m = (int **)malloc(sizeof(int *) * n);
            for (x = 0; x < n; x++)
            {
                m[x] = (int *)malloc(sizeof(int) * n);
                for (y = 0; y < n; y++)
                {
                    if (y < j && x < i) //余子式该部分不需要变化
                    {
                        m[x][y] = (mat->m)[x][y];
                    }
                    else if (y < j) //在十字线的右上
                    {
                        m[x][y] = (mat->m)[x + 1][y];
                    }
                    else if (x < i) //在十字线的左下
                    {
                        m[x][y] = (mat->m)[x][y + 1];
                    }
                    else //在十字线的右下
                    {
                        m[x][y] = (mat->m)[x + 1][y + 1];
                    }
                }
            }
            new_mat->m = m;
            new_mat->row = new_mat->col = n;
            //计算矩阵，并负值
            //判断正负取值，并转置
            if ((i + j) % 2 == 0)
            {
                (matr->m)[j][i] = calculate_determinant(new_mat);
            }
            else
            {
                (matr->m)[j][i] = -calculate_determinant(new_mat);
                if ((matr->m)[j][i] < 0)
                    (matr->m)[j][i] += 26;
            }
        }
    }
    return matr;
}
/* 
 *  整数求逆
 */
Euclid *get_gcd_inverse(int b)
{
    int a = 26, x, y, d, x1, x2, y1, y2, q, r;
    Euclid *e = (Euclid *)malloc(sizeof(Euclid));
    if (b == 0)
    {
        d = a;
        x = 1;
        y = 0;
        e->gcd = d;
        e->inverse = y;
        return e;
    }
    x2 = y1 = 1;
    x1 = y2 = 0;
    while (b > 0)
    {
        q = a / b;
        r = a - q * b;
        x = x2 - q * x1;
        y = y2 - q * y1;

        a = b;
        b = r;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
        d = a;
        x = x;
        y = y2;
    }
    d = a;
    x = x2;
    y = y2;
    e->gcd = d;
    e->inverse = y;
    return e;
}
/* 
 *  求逆矩阵(只有方阵)
 */
matrix *calculate_inverse_matrix(matrix *mat)
{
    int i, j, n;
    matrix *matr = calculate_adjoint_matrix(mat);
    int det = calculate_determinant(mat);
    n = matr->row;
    int **m = (int **)malloc(sizeof(int *) * n);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            matr->m[i][j] = (matr->m[i][j] * get_gcd_inverse(det)->inverse) % 26;
        }
    }
    return matr;
}

/* 
 *  求矩阵加法
 */
matrix *matrix_addition(matrix *a, matrix *b)
{
    int i, j;
    matrix *mat = NULL;
    if (a->row == b->row && a->col == b->col)
    {
        //初始化矩阵
        int **m = (int **)malloc(sizeof(int *) * a->row);
        for (i = 0; i < a->row; i++)
        {
            m[i] = (int *)malloc(sizeof(int) * a->col);
            for (j = 0; j < a->col; j++)
            {
                m[i][j] = ((a->m)[i][j] + (b->m)[i][j]) % 26;
            }
        }
        mat = (matrix *)malloc(sizeof(matrix));
        mat->m = m;
        mat->row = a->row;
        mat->col = a->col;
    }
    return mat;
}

/* 
 *  求矩阵减法
 */
matrix *matrix_subtraction(matrix *a, matrix *b)
{
    int i, j;
    matrix *mat = NULL;
    if (a->row == b->row && a->col == b->col)
    {
        //初始化矩阵
        int **m = (int **)malloc(sizeof(int *) * a->row);
        for (i = 0; i < a->row; i++)
        {
            m[i] = (int *)malloc(sizeof(int) * a->col);
            for (j = 0; j < a->col; j++)
            {
                m[i][j] = ((a->m)[i][j] - (b->m)[i][j]) % 26;
            }
        }
        mat = (matrix *)malloc(sizeof(matrix));
        mat->m = m;
        mat->row = a->row;
        mat->col = a->col;
    }
    return mat;
}

/* 
 *  求矩阵乘法
 */
matrix *matrix_multiplication(matrix *a, matrix *b)
{
    int i, j, n;
    matrix *mat = NULL;
    if (a->col == b->row)
    {
        int **m = (int **)malloc(sizeof(int *) * a->row);
        for (i = 0; i < a->row; i++)
        {
            m[i] = (int *)malloc(sizeof(int) * b->col);
            for (j = 0; j < b->col; j++)
            {
                m[i][j] = 0.0;
                for (n = 0; n < a->col; n++)
                {
                    m[i][j] = (m[i][j] + (a->m)[i][n] * (b->m)[n][j]) % 26;
                    if (m[i][j] < 0)
                    {
                        m[i][j] += 26;
                    }
                }
            }
        }
        mat = (matrix *)malloc(sizeof(matrix));
        mat->m = m;
        mat->row = a->row;
        mat->col = b->col;
    }
    return mat;
}

#endif
