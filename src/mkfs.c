#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "bwfs.h"

uint8_t block_bitmap[MAX_BLOCKS];

void inicializar_superblock(Superblock *sb, const char *passphrase) {
    sb->magic = BWFS_MAGIC;
    sb->total_blocks = MAX_BLOCKS;
    sb->total_inodes = MAX_INODES;
    sb->free_block_bitmap_offset = sizeof(Superblock);
    sb->inode_table_offset = sb->free_block_bitmap_offset + MAX_BLOCKS;
    sb->data_block_offset = sb->inode_table_offset + sizeof(Inode) * MAX_INODES;
    strncpy(sb->signature, passphrase, sizeof(sb->signature) - 1);
    sb->signature[sizeof(sb->signature) - 1] = '\0';
}

void guardar_bloque_pbm(const char *folder, int index, const void *data, size_t size) {
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "%s/block_%04d.pbm", folder, index);
    FILE *fp = fopen(ruta, "wb");
    if (!fp) {
        perror("Error escribiendo imagen PBM");
        exit(1);
    }

    fprintf(fp, "P4\n1000 1000\n");
    fwrite(data, 1, size, fp);
    fclose(fp);
}

void crear_fs(const char *folder, const char *passphrase) {
    mkdir(folder, 0755);

    Superblock sb;
    inicializar_superblock(&sb, passphrase);

    memset(block_bitmap, 0, sizeof(block_bitmap));
    Inode inodos[MAX_INODES] = {0};

    // Bloque 0 con metadata
    uint8_t *meta = calloc(1, BLOCK_SIZE);
    if (!meta) {
        perror("Error reservando memoria");
        exit(1);
    }

    memcpy(meta, &sb, sizeof(Superblock));
    memcpy(meta + sb.free_block_bitmap_offset, block_bitmap, sizeof(block_bitmap));
    memcpy(meta + sb.inode_table_offset, inodos, sizeof(inodos));
    guardar_bloque_pbm(folder, 0, meta, BLOCK_SIZE);
    free(meta);

    // Resto de bloques vacíos
    uint8_t bloque_vacio[BLOCK_SIZE] = {0};
    for (int i = 1; i < MAX_BLOCKS; i++) {
        guardar_bloque_pbm(folder, i, bloque_vacio, BLOCK_SIZE);
    }

    printf("FS creado exitosamente en carpeta %s\n", folder);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s carpeta_salida/\n", argv[0]);
        return 1;
    }

    char passphrase[32];
    printf("Ingrese una passphrase para el FS: ");
    fgets(passphrase, sizeof(passphrase), stdin);
    passphrase[strcspn(passphrase, "\n")] = 0;

    crear_fs(argv[1], passphrase);
    return 0;
}