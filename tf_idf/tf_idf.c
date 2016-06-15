#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct idf_struct {
    char *word;
    int df;
    float idf_weight;
    struct idf_struct *next_addr;
} Idf;

typedef struct tf_struct {
    char *word;
    int tf;
    float tf_weight;
    float tf_idf_weight;
    struct tf_struct *next_addr;
} Tf;

int main(void) {
    FILE *df_idf_file;
    char df_idf_file_name[] = "../idf_result/idf_result.txt";
    FILE *tf_file;
    char tf_file_name[512] = "";
    FILE *output_file;
    char output_file_name[512] = "";
    char word[256];
    int df_volume;
    float idf_weight;
    int tf;
    float tf_weight;
    float tf_idf_weight;

    int file_num;
    int list_length = 0;
    int i;

    Idf *df_idf_list = NULL;
    Idf *df_idf_this;
    Idf *df_idf_new = NULL;

    Tf *tf_list = NULL;
    Tf *tf_this;
    Tf *tf_new = NULL;
    Tf *tf_pre;
    Tf *tf_tmp;

    if ((df_idf_file = fopen(df_idf_file_name, "r")) == NULL) {
        printf("cannot open a file\n");
        return -1;
    }

    while (fscanf(df_idf_file, "%s\t%d\t%f\n", word, &df_volume, &idf_weight) != EOF) {
        df_idf_new = (Idf *)malloc(sizeof(Idf));
        df_idf_new->word = (char*)malloc(sizeof(char) * (strlen(word) + 1));

        strcpy(df_idf_new->word, word);
        df_idf_new->df = df_volume;
        df_idf_new->idf_weight = idf_weight;

        df_idf_new->next_addr = NULL;
        if (df_idf_list == NULL) {
            df_idf_list = df_idf_new;
        } else {
            df_idf_this->next_addr = df_idf_new;
        }
        df_idf_this = df_idf_new;
    }
    fclose(df_idf_file);


    for (file_num = 1; file_num <= 100; file_num++) {
        tf_list = NULL;
        sprintf(tf_file_name, "../tf_result/tf_%03d.txt", file_num);

        if ((tf_file = fopen(tf_file_name, "r")) == NULL) {
            printf("cannot open a file\n");
            return -1;
        }

        while (fscanf(tf_file, "%s\t%d\t%f\n", word, &tf, &tf_weight) != EOF) {
            tf_new = (Tf *)malloc(sizeof(Tf));
            tf_new->word = (char*)malloc(sizeof(char) * (strlen(word) + 1));

            strcpy(tf_new->word, word);
            tf_new->tf = tf;
            tf_new->tf_weight = tf_weight;

            tf_new->next_addr = NULL;
            if (tf_list == NULL) {
                tf_list = tf_new;
            } else {
                tf_this->next_addr = tf_new;
            }
            tf_this = tf_new;
        }
        fclose(tf_file);

        tf_this = tf_list;
        while (tf_this != NULL) {
            df_idf_this = df_idf_list;
            while (strcmp(tf_this->word, df_idf_this->word) != 0) {
                df_idf_this = df_idf_this->next_addr;
            }
            tf_this->tf_idf_weight = tf_this->tf_weight * df_idf_this->idf_weight;
            tf_this = tf_this->next_addr;
        }

        if (tf_list->word != NULL) {
            tf_this = tf_list;
            while (1) {
                list_length++;
                if (tf_this->next_addr == NULL) break;

                tf_this = tf_this->next_addr;
            }
        }

        for (i = 0; i < list_length; i++) {
            if (tf_list == NULL) break;

            tf_this = tf_list;
            tf_pre = NULL;

            while (1) {
                if (tf_this->next_addr == NULL) break;
                if (tf_this->tf_idf_weight < tf_this->next_addr->tf_idf_weight) {
                    tf_tmp = tf_this;
                    tf_this = tf_tmp->next_addr;
                    tf_tmp->next_addr = tf_this->next_addr;
                    tf_this->next_addr = tf_tmp;
                    if (tf_pre == NULL) {
                        tf_list = tf_this;
                    } else {
                        tf_pre->next_addr = tf_this;
                    }
                }
                tf_pre = tf_this;
                tf_this = tf_this->next_addr;
            }
        }

        sprintf(output_file_name, "../tf_idf_result/tf_idf_%03d.txt", file_num);
        if ((output_file = fopen(output_file_name, "w")) == NULL) {
            printf("cannot open a file\n");
            return -1;
        }

        if (tf_list != NULL) {
            tf_this = tf_list;
            while (1) {
                fprintf(output_file, "%s\t%d\t%f\t%f\n",
                    tf_this->word,
                    tf_this->tf,
                    tf_this->tf_weight,
                    tf_this->tf_idf_weight);
                if (tf_this->next_addr == NULL) break;

                tf_this = tf_this->next_addr;
            }
        }
        fclose(output_file);

        tf_this = tf_list;
        while (tf_this != NULL) {
            Tf tmp = *tf_this;
            free(tf_this->word);
            free(tf_this);
            tf_this = tmp.next_addr;
        }
    }

    return 0;
}
