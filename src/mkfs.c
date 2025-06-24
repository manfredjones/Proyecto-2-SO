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
        perror("Error escribiendo PBM");
        exit(1);
    }
    fprintf(fp, "P4\n1000 1000\n");
    fwrite(data, 1, size, fp);
    fclose(fp);
}

void crear_fs(const char *folder, const char *passphrase) {
    mkdir(folder, 0755);

    // Inicializar estructuras
    Superblock sb;
    inicializar_superblock(&sb, passphrase);

    memset(block_bitmap, 0, sizeof(block_bitmap));
    Inode inodos[MAX_INODES];
    memset(inodos, 0, sizeof(inodos));

    // Ejemplo de archivo de prueba
    const char *contenido = "Hola mundo\n";
    size_t len = strlen(contenido);
    int data_block = 1;

    block_bitmap[data_block] = 1;

    inodos[0].valid = 1;
    strncpy(inodos[0].filename, "test.txt", MAX_FILENAME - 1);
    inodos[0].size = len;
    for (int i = 0; i < MAX_BLOCKS; i++) inodos[0].blocks[i] = 0xFFFFFFFF;
    inodos[0].blocks[0] = data_block;

    guardar_bloque_pbm(folder, data_block, contenido, len);

    // Guardar bloque 0 (superblock + bitmap + inodos)
    uint8_t *bloque_meta = calloc(1, BLOCK_SIZE);
    if (!bloque_meta) {
        perror("Error reservando memoria");
        exit(1);
    }

    memcpy(bloque_meta, &sb, sizeof(Superblock));
    memcpy(bloque_meta + sb.free_block_bitmap_offset, block_bitmap, MAX_BLOCKS);
    memcpy(bloque_meta + sb.inode_table_offset, inodos, sizeof(inodos));
    guardar_bloque_pbm(folder, 0, bloque_meta, BLOCK_SIZE);
    free(bloque_meta);

    // Bloques vacíos
    uint8_t vacio[BLOCK_SIZE] = {0};
    for (int i = 2; i < MAX_BLOCKS; i++) {
        guardar_bloque_pbm(folder, i, vacio, BLOCK_SIZE);
    }

    printf("FS creado exitosamente en '%s'\n", folder);
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