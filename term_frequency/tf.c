#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mecab.h>

#define CHECK(eval) \
    if (!eval) { \
        fprintf(stderr, "Exception:%s\n", mecab_strerror(mecab)); \
        mecab_destroy(mecab); \
        return -1; \
    }

struct WordStruct {
    char *word;
    int cnt;
    struct WordStruct *next_addr;
};

int main(int argc, char **argv) {
    mecab_t *mecab;
    const mecab_node_t *node;
    int i;
    FILE *file;
    int file_size;
    char file_name[512] = "";
    char *string;
    char ch;
    char *word;
    struct WordStruct *words_list, *word_this, *word_new;
    int found_flg = 0;

    strcpy(file_name, "../data/001.txt");

    if ((file = fopen(file_name, "r")) == NULL) {
        printf("can't open a file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    printf("%sファイルのサイズ:%dバイト\n", file_name, file_size);
    fseek(file, 0, SEEK_SET);

    string = (char *)malloc(sizeof(char) * (file_size + 1));
    for (i = 0; (ch = fgetc(file)) != -1; i++) {
        string[i] = ch;
    }
    string[i] = '\0';

    // Create tagger object
    mecab = mecab_new(argc, argv);
    CHECK(mecab);

    // Gets node object
    node = mecab_sparse_tonode(mecab, string);
    CHECK(node);
    words_list = NULL;
    for (; node; node = node->next) {
        if (node->stat == MECAB_NOR_NODE || node->stat == MECAB_UNK_NODE) {
            fwrite(node->surface, sizeof(char), node->length, stdout);
            printf("\t%s\n", node->feature);

            word = (char *)malloc(sizeof(char) * (node->length + 1));
            strncpy(word, node->surface, node->length);
            word[node->length] = '\0';
        }

        word_this = words_list;
        while (1) {
            if (word_this == NULL) break;
            if (strcmp(word_this->word, word) == 0) {
                found_flg = 1;
                free(word);
                break;
            }
            if (word_this->next_addr == NULL) break;

            word_this = word_this->next_addr;

            if (found_flg) {
                word_this->cnt++;
                free(word);
                continue;
            }

            word_new = malloc(sizeof(struct WordStruct));
            word_new->word = word;
            word_new->cnt = 1;
            word_new->next_addr = NULL;
            if (words_list == NULL) {
                words_list = word_new;
            } else {
                word_this->next_addr = word_new;
            }
        }
    }

    mecab_destroy(mecab);

    return 0;
}
