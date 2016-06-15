#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mecab.h>

typedef struct word_struct {
    char *word;
    int cnt;
    struct word_struct *next_addr;
} Word;

int main(void) {
    int file_num = 1;
    int list_length = 0;
    int words_volume = 0;
    int i;

    FILE *output_file;
    char output_file_name[] = "../idf_result/idf_result.txt";

    Word *words_list = NULL;
    Word *words_this;
    Word *words_new = NULL;
    Word *words_tmp;
    Word *words_pre;

    for (file_num = 1; file_num <= 100; file_num++) {
        FILE *file;
        char file_name[512] = "";

        char word[256];
        float tf_volume = 0;
        float tf_weight = 0;
        int found_flg = 0;

        sprintf(file_name, "../tf_result/tf_%03d.txt", file_num);

        if ((file = fopen(file_name, "r")) == NULL) {
            printf("can't open a file");
            return -1;
        }

        while (fscanf(file, "%s\t%f\t%f\n", word, &tf_volume, &tf_weight) != EOF) {
            // printf("%03d:%s\n", file_num, word);
            found_flg = 0;
            words_this = words_list;
            while (1) {
                if (words_this == NULL) break;
                if (strcmp(words_this->word, word) == 0) {
                    found_flg = 1;
                    break;
                }
                if (words_this->next_addr == NULL) break;

                words_this = words_this->next_addr;
            }

            if (found_flg) {
                words_this->cnt++;
                continue;
            }

            words_new = malloc(sizeof(Word));
            words_new->word = (char*)malloc(sizeof(char) * (strlen(word) + 1));
            strcpy(words_new->word, word);
            words_new->cnt = 1;
            words_new->next_addr = NULL;
            if (words_list == NULL) {
                words_list = words_new;
            } else {
                words_this->next_addr = words_new;
            }
        }
        fclose(file);
    }

    if (words_list->word != NULL) {
        words_this = words_list;
        while (1) {
            // printf("%s:%d\n", words_this->word, words_this->cnt);
            list_length++;
            words_volume += words_this->cnt;
            // printf("%s %d\n", words_this->word, words_this->cnt);
            if (words_this->next_addr == NULL) break;

            words_this = words_this->next_addr;
        }
    }

    for (i = 0; i < list_length; i++) {
        if (words_list == NULL) break;

        words_this = words_list;
        words_pre = NULL;

        while (1) {
            if (words_this->next_addr == NULL) break;
            if (words_this->cnt < words_this->next_addr->cnt) {
                words_tmp = words_this;
                words_this = words_tmp->next_addr;
                words_tmp->next_addr = words_this->next_addr;
                words_this->next_addr = words_tmp;
                if (words_pre == NULL) {
                    words_list = words_this;
                } else {
                    words_pre->next_addr = words_this;
                }
            }
            words_pre = words_this;
            words_this = words_this->next_addr;
        }
    }

    if ((output_file = fopen(output_file_name, "w")) == NULL) {
        printf("can't open a file");
        return -1;
    }

    if (words_list != NULL) {
        words_this = words_list;
        while (1) {
            // printf("%s:%d:%f\n",
            //     words_this->word,
            //     words_this->cnt,
            //     log10(100 / words_this->cnt) + 1);
            fprintf(output_file, "%s\t%d\t%f\n",
                words_this->word,
                words_this->cnt,
                log10(100 / words_this->cnt) + 1);
            if (words_this->next_addr == NULL) break;

            words_this = words_this->next_addr;
        }
    }
    fclose(output_file);

    return 0;
}
