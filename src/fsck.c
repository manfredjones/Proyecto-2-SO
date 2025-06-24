#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "bwfs.h"

void leer_bloque_0(const char *folder, uint8_t *buffer) {
    char path[256];
    snprintf(path, sizeof(path), "%s/block_0000.pbm", folder);
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        perror("Error abriendo bloque 0");
        exit(1);
    }

    char header[64];
    fgets(header, sizeof(header), fp); // P4
    fgets(header, sizeof(header), fp); // 1000 1000

    fread(buffer, 1, BLOCK_SIZE, fp);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s carpeta_fs/\n", argv[0]);
        return 1;
    }

    const char *folder = argv[1];
    uint8_t bloque0[BLOCK_SIZE];
    leer_bloque_0(folder, bloque0);

    Superblock *sb = (Superblock *)bloque0;
    if (sb->magic != BWFS_MAGIC) {
        fprintf(stderr, "Error: magic incorrecto (esperado: 0x%X, encontrado: 0x%X)\n", BWFS_MAGIC, sb->magic);
        return 1;
    }

    printf("✔ Magic válido: 0x%X\n", sb->magic);
    printf("✔ Signature: %s\n", sb->signature);

    uint8_t *bitmap = bloque0 + sb->free_block_bitmap_offset;
    Inode *inodos = (Inode *)(bloque0 + sb->inode_table_offset);

    int errores = 0;

    for (int i = 0; i < MAX_INODES; i++) {
        if (!inodos[i].valid) continue;
        for (int j = 0; j < MAX_BLOCKS; j++) {
            uint32_t blk = inodos[i].blocks[j];
            if (blk == 0xFFFFFFFF) continue;
            if (blk >= MAX_BLOCKS) {
                printf("❌ Error: Inodo %d usa bloque inválido %u\n", i, blk);
                errores++;
            } else if (bitmap[blk] != 1) {
                printf("❌ Error: Bloque %u usado por inodo %d no está marcado en bitmap\n", blk, i);
                errores++;
            }
        }
    }

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (bitmap[i] == 1) {
            int referenciado = 0;
            for (int j = 0; j < MAX_INODES && !referenciado; j++) {
                if (!inodos[j].valid) continue;
                for (int k = 0; k < MAX_BLOCKS; k++) {
                    if (inodos[j].blocks[k] == i) {
                        referenciado = 1;
                        break;
                    }
                }
            }
            if (!referenciado) {
                printf("❌ Error: Bloque %d marcado como usado pero no referenciado por ningún inodo\n", i);
                errores++;
            }
        }
    }

    if (errores == 0) {
        printf("✅ FS consistente. No se encontraron errores.\n");
    } else {
        printf("❌ FS inconsistente. Se encontraron %d error(es).\n", errores);
    }

    return errores ? 1 : 0;
}