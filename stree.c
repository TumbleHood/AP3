#include "stree.h"

// [{file type}{owner's permissions}{group's permissions}{other's permissions} {owner name} {group name} {size}] {file name}

static int dirTree(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

    if (ftwbuf->level != 0 && (&fpath[ftwbuf->base])[0] == '.') { // hidden directory
        return FTW_SKIP_SUBTREE;
    }
    
    info_t info;

    // file type
    if (typeflag == FTW_NS) {
        info.file_type = '?';
    } else {
        switch (sb->st_mode & S_IFMT) {
            case S_IFREG:  info.file_type = '-'; break;
            case S_IFDIR:  info.file_type = 'd'; break;
            case S_IFCHR:  info.file_type = 'c'; break;
            case S_IFBLK:  info.file_type = 'b'; break;
            case S_IFLNK:  info.file_type = 'l'; break;
            case S_IFIFO:  info.file_type = 'p'; break;
            case S_IFSOCK: info.file_type = 's'; break;
            default:       info.file_type = '?'; break;
        }
    }

    // count number of directories and files (don't count given path)
    if (ftwbuf->level != 0) {
        if (info.file_type == 'd') {
            dir_num += 1;
        } else {
            file_num += 1;
        }
    }

    // owner's permissions
    info.owner_per[0] = (sb->st_mode & S_IRUSR) ? 'r' : '-';
    info.owner_per[1] = (sb->st_mode & S_IWUSR) ? 'w' : '-';
    info.owner_per[2] = (sb->st_mode & S_IXUSR) ? 'x' : '-';
    info.owner_per[3] = '\0';
    
    // group's permissions
    info.group_per[0] = (sb->st_mode & S_IRGRP) ? 'r' : '-';
    info.group_per[1] = (sb->st_mode & S_IWGRP) ? 'w' : '-';
    info.group_per[2] = (sb->st_mode & S_IXGRP) ? 'x' : '-';
    info.group_per[3] = '\0';
    
    // other's permissions
    info.other_per[0] = (sb->st_mode & S_IROTH) ? 'r' : '-';
    info.other_per[1] = (sb->st_mode & S_IWOTH) ? 'w' : '-';
    info.other_per[2] = (sb->st_mode & S_IXOTH) ? 'x' : '-';
    info.other_per[3] = '\0';
    
    // owner name
    info.owner_name = getpwuid(sb->st_uid)->pw_name;

    // group name
    info.group_name = getgrgid(sb->st_gid)->gr_name;

    // size
    int size = (int)sb->st_size;
    char size_str[16] = "                ";
    int position = sizeof(size_str) - snprintf(NULL, 0, "%d", size);
    memset(size_str + position, ' ', sizeof(size_str) - position + 1);
    snprintf(size_str + position, sizeof(size_str) - position + 1, "%d", size);
    info.size_str = (char*)malloc(sizeof(size_str));
    strcpy(info.size_str, size_str);

    // file name
    if (info.file_type == 'l') { //if it's a symlink
        char *path = (char*)malloc(sb->st_size + 1);
        ssize_t bytesRead = readlink(fpath, path, sb->st_size + 1);
        path[bytesRead] = '\0';

        info.file_name = (char*)malloc(strlen(&fpath[ftwbuf->base]) + 4 + strlen(path));
        strncpy(info.file_name, &fpath[ftwbuf->base], strlen(&fpath[ftwbuf->base]));
        strncpy(info.file_name + strlen(&fpath[ftwbuf->base]), " -> ", 4);
        strncpy(info.file_name + strlen(&fpath[ftwbuf->base]) + 4, path, strlen(path));
        strncpy(info.file_name + strlen(&fpath[ftwbuf->base]) + 4 + strlen(path), "\0", 1);

        free(path);
    } else {
        info.file_name = (char*)malloc(sizeof(&fpath[ftwbuf->base]));
        strcpy(info.file_name, &fpath[ftwbuf->base]);
    }

    // add info to linked list
    if (head == NULL){
        head = (node_t*)malloc(sizeof(node_t));
        head->info = info;
        head->level = ftwbuf->level;
        head->next = NULL;
        temp = head;
    } else {
        temp->next = (node_t*)malloc(sizeof(node_t));
        temp->next->info = info;
        temp->next->level = ftwbuf->level;
        temp->next->next = NULL;
        temp = temp->next;
    }

    return 0;
}

int hasSibling(node_t **curr) {
    // check if there are more files in current directory
    node_t *curr_temp = (*curr)->next;
    while (curr_temp != NULL) {
        if (curr_temp->level == (*curr)->level){
            return TRUE;
        }
        curr_temp = curr_temp->next;
    }
    return FALSE;
}

int hasUncle(node_t **curr, int uncle_level) {
    // check if there are parent directories in that level
    node_t *curr_temp = (*curr)->next;
    while (curr_temp != NULL) {
        if (curr_temp->level == uncle_level){
            return TRUE;
        }
        curr_temp = curr_temp->next;
    }
    return FALSE;
}

void printTree() {
    // print the tree just like "tree -pugs"
    temp = head;
    while (temp != NULL){    
        // ├ ─ └ │
        if (temp->level != 0){
            for (int i = 0; i < temp->level - 1; i++){
                if (hasUncle(&temp, i + 1)) {
                    printf("│   ");
                } else {
                    printf("    ");
                }
            }
            if (!temp->next || temp->next->level < temp->level || !hasSibling(&temp)) {
                printf("└── ");
            } else {
                printf("├── ");
            }
        }
        printf("[%c%s%s%s %s     %s%s]  %s\n", temp->info.file_type, temp->info.owner_per, temp->info.group_per, temp->info.other_per, temp->info.owner_name, temp->info.group_name, temp->info.size_str, temp->info.file_name);
        temp = temp->next;
    }
    printf("\n");
    if (dir_num == 1) {
        printf("%i directory, ", dir_num);
    } else {
        printf("%i directories, ", dir_num);
    }
    if (file_num == 1) {
        printf("%i file", file_num);
    } else {
        printf("%i files", file_num);
    }
    printf("\n");
}

int main(int argc, char *argv[]){
    if (argc == 1) {
        nftw(".", dirTree, 10, FTW_ACTIONRETVAL | FTW_PHYS);
    }
    else if (argc == 2) {
        nftw(argv[1], dirTree, 10, FTW_ACTIONRETVAL | FTW_PHYS);
    }
    else {
        printf("Usage: \n\t%s", argv[0]);
        printf("\n\t%s directory-path\n", argv[0]);
    }

    printTree();
    
    return 0;
}