#include <stdio.h>
#include "matrix.h"
#include <math.h>

/*
 *  调用矩阵运算
 */
matrix *create_matrix(int row, int col);
void print_matrix(matrix *mat);
int calculate_determinant(matrix *matr);
matrix *calculate_adjoint_matrix(matrix *mat);
matrix *calculate_inverse_matrix(matrix *mat);
matrix *matrix_addition(matrix *a, matrix *b);
matrix *matrix_subtraction(matrix *a, matrix *b);
matrix *matrix_multiplication(matrix *a, matrix *b);

/*
 *  读入文档的数据结构
 */
typedef struct
{
    char *p;    //数组
    int length; //长度
} text;

/*
 *  中间计算时使用的数据结构
 */
typedef struct
{
    matrix **mat_arr; //矩阵数组
    int group_number; //分组个数
    int group_length; //每个分组长度
} intermediate_data;

/*
 *  密钥的数据结构
 */
typedef struct
{
    matrix *A;
    matrix *B;
} key;

/*
 *  读入
 */
text *get_plaintext()
{
    char ch, c[1000];
    int i, length = 0;
    printf("please input your plaintext(Less than 1000 characters and  letters only):\n");
    ch = getchar();
    while (ch != '\n')
    {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        {
            length++;
            c[length - 1] = ch;
            ch = getchar();
        }
        else
        {
            printf("\n*****error:Invalid characters entered*****\n");
            return NULL;
        }
    }
    char *p = (char *)malloc(sizeof(char) * length);
    for (i = 0; i < length; i++)
    {
        p[i] = c[i];
    }
    text *plain = (text *)malloc(sizeof(text));
    plain->p = p;
    plain->length = length;
    printf("your enter is:\n");
    for (i = 0; i < length; i++)
    {
        printf("%c", (plain->p)[i]);
    }
    printf("\n");
    return plain;
}

/*
 *  获得密钥，选择测试或者随机生成
 */
key *get_key()
{
    printf("\nchoose key for test or create random key(please input to make a choice [t/r])\n");
    char flag = getchar();
    key *k = (key *)malloc(sizeof(key));
    if (flag == 't' || flag == 'T')
    {
        matrix *a = (matrix *)malloc(sizeof(matrix));
        a->row = a->col = 3;
        int **m = (int **)malloc(sizeof(int *) * 3);
        m[0] = (int *)malloc(sizeof(int) * 3);
        m[1] = (int *)malloc(sizeof(int) * 3);
        m[2] = (int *)malloc(sizeof(int) * 3);
        m[0][0] = 11;
        m[0][1] = 2;
        m[0][2] = 19;
        m[1][0] = 5;
        m[1][1] = 23;
        m[1][2] = 25;
        m[2][0] = 20;
        m[2][1] = 7;
        m[2][2] = 17;
        a->m = m;

        matrix *b = (matrix *)malloc(sizeof(matrix));
        b->row = 3;
        b->col = 1;
        m = (int **)malloc(sizeof(int *) * 3);
        m[0] = (int *)malloc(sizeof(int));
        m[1] = (int *)malloc(sizeof(int));
        m[2] = (int *)malloc(sizeof(int));
        m[0][0] = 0.00;
        m[1][0] = 0.00;
        m[2][0] = 0.00;
        b->m = m;

        k->A = a;
        k->B = b;
    }
    else
    {
        int i, j, n;
        printf("\nplease enter the dimension of the key:");
        scanf("%d", &n);
        matrix *key_a = create_matrix(n, n);
        matrix *key_b = create_matrix(n, 1);
        while (get_gcd_inverse(calculate_determinant(key_a))->gcd != 1)
        {
            destory_matrix(key_a);
            key_a = create_matrix(n, n);
        }

        k->A = key_a;
        k->B = key_b;
    }
    printf("\nkey A ");
    print_matrix(k->A);
    printf("\nkey B ");
    print_matrix(k->B);
    return k;
}

/*
 *  分组,若不能完全分组则填充o
 */
intermediate_data *divide_into_groups(text *t, int group_length)
{
    int group_number = (int)ceil((double)(t->length) / (double)group_length);
    matrix **mat_arr = (matrix **)malloc(sizeof(matrix *) * group_number);
    int i, j, x, count = 0;
    for (i = 0; i < group_number; i++)
    {
        //初始化数组矩阵
        mat_arr[i] = (matrix *)malloc(sizeof(matrix));
        mat_arr[i]->col = 1;
        mat_arr[i]->row = group_length;
        int **m = (int **)malloc(sizeof(int *) * group_length);
        for (x = 0; x < group_length; x++)
        {
            m[x] = (int *)malloc(sizeof(int));
        }
        mat_arr[i]->m = m;

        for (x = 0; x < group_length; x++)
        {
            if (count < t->length) //正常分组
            {
                if ((t->p)[count] >= 'A' && (t->p)[count] <= 'Z')
                {
                    (mat_arr[i]->m)[x][0] = (int)((t->p)[count] - 'A');
                }
                else if ((t->p)[count] >= 'a' && (t->p)[count] <= 'z')
                {
                    (mat_arr[i]->m)[x][0] = (int)((t->p)[count] - 'a');
                }
                count++;
            }
            else //需要填充,直接填充z
            {
                (mat_arr[i]->m)[x][0] = 14;
            }
        }
    }
    intermediate_data *data = (intermediate_data *)malloc(sizeof(intermediate_data));
    data->mat_arr = mat_arr;
    data->group_length = group_length;
    data->group_number = group_number;
    // for (i = 0; i < group_number; i++)
    // {
    //     printf("NO.%d", i);
    //     print_matrix((data->mat_arr)[i]);
    // }
    return data;
}

/*
 *  解密变换
 */
text *decode(text *cliper, key *k)
{
    //分组
    intermediate_data *cipher_data = divide_into_groups(cliper, k->A->row);
    //解密
    intermediate_data *clear_data = (intermediate_data *)malloc(sizeof(intermediate_data));
    clear_data->group_length = cipher_data->group_length;
    clear_data->group_number = cipher_data->group_number;
    matrix **mat_arr = (matrix **)malloc(sizeof(matrix *) * clear_data->group_number);
    int i, j;
    for (i = 0; i < clear_data->group_number; i++)
    {
        //完成 A逆(M-B)
        matrix *temp = matrix_subtraction((cipher_data->mat_arr)[i], k->B);
        mat_arr[i] = matrix_multiplication(calculate_inverse_matrix(k->A), temp);
        free(temp);
    }
    clear_data->mat_arr = mat_arr;

    //转为明文
    text *clear_text = (text *)malloc(sizeof(text));
    clear_text->length = clear_data->group_length * clear_data->group_number;
    clear_text->p = (char *)malloc(sizeof(char) * clear_text->length);
    int count = 0;
    for (i = 0; i < clear_data->group_number; i++)
    {
        for (j = 0; j < clear_data->group_length; j++)
        {
            (clear_text->p)[count] = 'A' + (int)(((clear_data->mat_arr)[i])->m[j][0]);
            count++;
        }
    }
    // for (i = 0; i < clear_data->group_number; i++)
    // {
    //     printf("NO.%d", i);
    //     print_matrix((clear_data->mat_arr)[i]);
    // }

    printf("\n\ndecoding...\n\nclear text is:\n");
    for (i = 0; i < clear_text->length; i++)
    {
        printf("%c", (clear_text->p)[i]);
    }
    printf("\n\n");
    return clear_text;
}

/*
 *  加密变换
 */
text *encode(text *clear, key *k)
{
    //分组
    intermediate_data *clear_data = divide_into_groups(clear, k->A->row);
    //加密
    intermediate_data *cipher_data = (intermediate_data *)malloc(sizeof(intermediate_data));
    cipher_data->group_length = clear_data->group_length;
    cipher_data->group_number = clear_data->group_number;
    matrix **mat_arr = (matrix **)malloc(sizeof(matrix *) * cipher_data->group_number);
    int i, j;
    for (i = 0; i < cipher_data->group_number; i++)
    {
        //完成 A*M+B
        matrix *temp = matrix_multiplication(k->A, (clear_data->mat_arr)[i]);
        mat_arr[i] = matrix_addition(temp, k->B);
        free(temp);
        // print_matrix(mat_arr[i]);
    }
    cipher_data->mat_arr = mat_arr;

    //转为密文
    text *cipher_text = (text *)malloc(sizeof(text));
    cipher_text->length = cipher_data->group_length * cipher_data->group_number;
    cipher_text->p = (char *)malloc(sizeof(char) * cipher_text->length);
    int count = 0;
    for (i = 0; i < cipher_data->group_number; i++)
    {
        for (j = 0; j < cipher_data->group_length; j++)
        {
            (cipher_text->p)[count] = 'A' + (int)(((cipher_data->mat_arr)[i])->m[j][0]);
            count++;
        }
    }
    printf("\n\nencoding...\n\nCipher text is:\n");
    for (i = 0; i < cipher_text->length; i++)
    {
        printf("%c", (cipher_text->p)[i]);
    }
    printf("\n\n");
    return cipher_text;
}

int main(void)
{
    text *p = get_plaintext();
    key *k = get_key();

    decode(encode(p, k), k);

    system("pause");
    return 0;
}