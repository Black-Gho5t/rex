#ifndef REX_ELF_TRAMPOLINE_H
#define REX_ELF_TRAMPOLINE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Trampoline implementation for legacy Bionic __system_property_get
 */
int rex_system_property_get(const char* name, char* value);

/**
 * Trampoline implementation for legacy bsd_signal
 */
typedef void (*rex_sighandler_t)(int);
rex_sighandler_t rex_bsd_signal(int signum, rex_sighandler_t handler);

/**
 * Hook initializer for legacy ELF symbol redirection
 */
bool init_elf_trampoline_layer();

#ifdef __cplusplus
}
#endif

#endif // REX_ELF_TRAMPOLINE_H
