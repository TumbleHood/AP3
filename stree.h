#define _GNU_SOURCE 1
#define _XOPEN_SOURCE 600
#include <ftw.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#define TRUE 1
#define FALSE 0

typedef struct info {
    char file_type;
    char owner_per[4];
    char group_per[4];
    char other_per[4];
    char *owner_name;
    char *group_name;
    char *size_str;
    char *file_name;
} info_t;

typedef struct node {
    info_t info;
    int level;
    struct node *next;
} node_t;

static int dirTree(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

void printTree();

node_t *head = NULL;
node_t *temp = NULL;
int dir_num = 0;
int file_num = 0;