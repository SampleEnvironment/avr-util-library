/*
 * fuse_config.h
 *
 * AVR fuse byte configuration for ATmega devices used across all implementations.
 *
 * IMPORTANT: Include this file in exactly ONE translation unit (main.c).
 *            Including it in multiple .c files will cause linker errors due to
 *            duplicate symbols in the .fuse section.
 *
 * Fuse low byte depends on clock source, selected at build time:
 *   -DINTERNAL_OSZ  ->  Internal RC oscillator, 8 MHz
 *   (default)       ->  External crystal/resonator, 3-8 MHz, 16K CK + 65ms startup
 */

#ifndef FUSE_CONFIG_H
#define FUSE_CONFIG_H

#include <avr/fuse.h>

#ifdef INTERNAL_OSZ
// ============================================================
// Clock: Internal RC oscillator 8 MHz
// ============================================================
#  define FUSE_LOW (0xE2)

#else
// ============================================================
// Clock: External crystal/resonator 3-8 MHz, 16K CK + 65ms
// ============================================================
#  define FUSE_LOW (0xFD)
#endif

FUSES = {
	.low      = FUSE_LOW,
	.high     = (0xD1),
	.extended = (0xFF),
};

#endif /* FUSE_CONFIG_H */