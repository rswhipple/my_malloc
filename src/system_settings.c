
#include "../inc/main.h"

void system_settings() {
    size_t pointer_size = check_system_pointer();
    if (pointer_size == 4) {
        min_chunk_size = 16;
        log_info("Minimum chunk size is 16 bytes (12 free).");
        <<<<<< < HEAD
    }
    else {
        ====== =
    }
    else {
        min_chunk_size = 24;
        log_info("Minimum chunk size is 24 bytes (20 free).");
        >>>>>> > e7bf7f8(fix: add undefined check_system_size_t fn and include main.h)
            size_t size_t_size = check_system_size_t();
        if (size_t_size == 4) {
            min_chunk_size = 24;
            log_info("Minimum chunk size is 24 bytes (20 free).");
        }
        else {
            min_chunk_size = 24;
            log_info("Minimum chunk size is 32 bytes (24 free).");
        }
    }
}

size_t check_system_pointer() {
    size_t pointer_size = sizeof(void*);
    printf("Pointer size: %zu bytes\n", pointer_size);

    if (pointer_size == 4) {
        log_info("System has 4-byte pointers.\n");
    }
    else if (pointer_size == 8) {
        log_info("System has 8-byte pointers.\n");
    }
    else {
        printf("Unexpected pointer size: %zu bytes\n", pointer_size);
        // add error exit
    }

    return pointer_size;
}

size_t check_system_size_t() {
    size_t size_t_size = sizeof(size_t);
    printf("Size_t size: %zu bytes\n", size_t_size);

    if (size_t_size == 4) {
        log_info("System has 4-byte size_t.\n");
    }
    else if (size_t_size == 8) {
        log_info("System has 8-byte size_t.\n");
    }
    else {
        printf("Unexpected size_t size: %zu bytes\n", size_t_size);
        // add error exit
    }

    return size_t_size;
}
