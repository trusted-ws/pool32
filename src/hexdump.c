/* * Royal Rand - Crypto utility for Royal Bingo
 * Copyright (C) 2021-2022 Murilo Augusto <murilo@bad1337.com>
 *
 * This file is part of Royal Rand.
 *
 * Royal Rand is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Royal Rand is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Royal Rand.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

void hexdump(const char * desc, const void * addr, const int len, int perLine) {
    /*
    * hexdump(desc, addr, len, perLine);
    *
    *     desc:    if non-NULL, printed as a description before hex dump.
    *     addr:    the address to start dumping from.
    *     len:     the number of bytes to dump.
    *     perLine: number of bytes on each output line.
    */

    // Silently ignore silly per-line values.
    if (perLine < 4 || perLine > 64) perLine = 16;

    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.

    if (desc != NULL) printf ("%s:\n", desc);

    // Length checks.

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).

        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.
            if (i != 0) printf("  %s\n", buff);

            // Output the offset of current line.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }

    // Pad out last line if not exactly perLine characters.

    while ((i % perLine) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII buffer.
    printf("  %s\n", buff);
}
