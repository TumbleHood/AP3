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

// [{file type}{owner's permissions}{group's permissions}{other's permissions} {owner name} {group name} {size}] {file name}

static int dirTree(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    
    if (typeflag == FTW_D && ftwbuf->level != 0 && (&fpath[ftwbuf->base])[0] == '.') { // hidden directory
        return 0660;
    }

    // ├─ ─ └─ │
    if (ftwbuf->level != 0){
        for (int i = 0; i < ftwbuf->level - 1; i++){
            printf("│  ");
        }
        printf("├── ");
    }
    printf("[");
    
    // file type
    if (typeflag == FTW_NS) {
        printf("?");
    } else {
        switch (sb->st_mode & S_IFMT) {
            case S_IFREG:  printf("-"); break;
            case S_IFDIR:  printf("d"); break;
            case S_IFCHR:  printf("c"); break;
            case S_IFBLK:  printf("b"); break;
            case S_IFLNK:  printf("l"); break;
            case S_IFIFO:  printf("p"); break;
            case S_IFSOCK: printf("s"); break;
            default:       printf("?"); break;
        }
    }

    // owner's permissions
    printf("%c%c%c", (sb->st_mode & S_IRUSR) ? 'r' : '-',
                     (sb->st_mode & S_IWUSR) ? 'w' : '-',
                     (sb->st_mode & S_IXUSR) ? 'x' : '-');
    
    // group's permissions
    printf("%c%c%c", (sb->st_mode & S_IRGRP) ? 'r' : '-',
                     (sb->st_mode & S_IWGRP) ? 'w' : '-',
                     (sb->st_mode & S_IXGRP) ? 'x' : '-');
    
    // other's permissions
    printf("%c%c%c", (sb->st_mode & S_IROTH) ? 'r' : '-',
                     (sb->st_mode & S_IWOTH) ? 'w' : '-',
                     (sb->st_mode & S_IXOTH) ? 'x' : '-');
    
    // owner name
    printf(" %s", getpwuid(sb->st_uid)->pw_name);

    // group name a           54350a
    //            b            4096b
    printf("     %s", getgrgid(sb->st_gid)->gr_name);

    // size
    int size = (int)sb->st_size;
    char sizeStr[16] = "                ";
    int position = sizeof(sizeStr) - snprintf(NULL, 0, "%d", size);
    memset(sizeStr + position, ' ', sizeof(sizeStr) - position + 1);
    snprintf(sizeStr + position, sizeof(sizeStr) - position + 1, "%d", size);
    printf("%s]", sizeStr);

    // file name
    printf("  %s", &fpath[ftwbuf->base]);
    
    printf("\n");
    return 0;
}

int main(int argc, char *argv[]){
    if (argc == 1) {
        nftw(".", dirTree, 10, 0);
    }
    else if (argc == 2) {
        nftw(argv[1], dirTree, 10, 0);
    }
    else {
        printf("Usage: \n\t%s", argv[0]);
        printf("\n\t%s directory-path\n", argv[0]);
    }
    
    return 0;
}