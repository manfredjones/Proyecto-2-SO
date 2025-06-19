#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "bwfs.h"

static char *fs_folder = NULL;
static Superblock superblock;
static Inode inodes[MAX_INODES];
uint8_t block_bitmap[MAX_BLOCKS];

void leer_bloque_pbm(const char *ruta, uint8_t *dest, size_t size) {
    FILE *fp = fopen(ruta, "rb");
    if (!fp) {
        perror("No se pudo abrir bloque PBM");
        exit(1);
    }

    char buffer[32];
    fgets(buffer, sizeof(buffer), fp); // P4
    fgets(buffer, sizeof(buffer), fp); // 1000 1000

    fread(dest, 1, size, fp);
    fclose(fp);
}

void cargar_superblock(const char *folder) {
    uint8_t bloque[BLOCK_SIZE] = {0};
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "%s/block_0000.pbm", folder);
    leer_bloque_pbm(ruta, bloque, BLOCK_SIZE);

    memcpy(&superblock, bloque, sizeof(Superblock));
    memcpy(block_bitmap, bloque + superblock.free_block_bitmap_offset, sizeof(block_bitmap));
    memcpy(inodes, bloque + superblock.inode_table_offset, sizeof(inodes));

    printf("FS montado. Firma: %s\n", superblock.signature);
}

static int bwfs_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    for (int i = 0; i < MAX_INODES; i++) {
        if (inodes[i].valid && strcmp(path + 1, inodes[i].filename) == 0) {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = inodes[i].size;
            return 0;
        }
    }

    return -ENOENT;
}

static int bwfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
    if (strcmp(path, "/") != 0) return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (int i = 0; i < MAX_INODES; i++) {
        if (inodes[i].valid) {
            filler(buf, inodes[i].filename, NULL, 0);
        }
    }

    return 0;
}

static int bwfs_open(const char *path, struct fuse_file_info *fi) {
    for (int i = 0; i < MAX_INODES; i++) {
        if (inodes[i].valid && strcmp(path + 1, inodes[i].filename) == 0) {
            return 0;
        }
    }
    return -ENOENT;
}

static int bwfs_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    for (int i = 0; i < MAX_INODES; i++) {
        if (inodes[i].valid && strcmp(path + 1, inodes[i].filename) == 0) {
            uint32_t block_index = inodes[i].blocks[0];
            if (block_index >= MAX_BLOCKS) return -EIO;

            char ruta[256];
            snprintf(ruta, sizeof(ruta), "%s/block_%04d.pbm", fs_folder, block_index);
            uint8_t contenido[BLOCK_SIZE] = {0};
            leer_bloque_pbm(ruta, contenido, BLOCK_SIZE);

            if (offset >= inodes[i].size) return 0;
            if (offset + size > inodes[i].size)
                size = inodes[i].size - offset;

            memcpy(buf, contenido + offset, size);
            return size;
        }
    }
    return -ENOENT;
}

static struct fuse_operations bwfs_oper = {
    .getattr = bwfs_getattr,
    .readdir = bwfs_readdir,
    .open    = bwfs_open,
    .read    = bwfs_read,
};

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s carpeta_fs punto_montaje\n", argv[0]);
        return 1;
    }

    fs_folder = argv[1];
    cargar_superblock(fs_folder);

    for (int i = 1; i < argc - 1; i++) argv[i] = argv[i + 1];
    argc--;

    return fuse_main(argc, argv, &bwfs_oper, NULL);
}