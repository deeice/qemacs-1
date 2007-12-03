/*
 * Convert Unicode 8-bit code page files to QEmacs format
 *
 * Copyright (c) 2002 Fabrice Bellard.
 * Copyright (c) 2007 Charlie Gordon.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static char module_init[4096];
static char *module_init_p = module_init;

#define add_init(s)  (module_init_p += snprintf(module_init_p, \
                     module_init + sizeof(module_init) - module_init_p, \
                     "%s", s))

static char *get_basename(const char *pathname)
{
    const char *base = pathname;

    while (*pathname) {
        if (*pathname++ == '/')
            base = pathname;
    }
    return (char *)base;
}

static char *get_extension(const char *pathname)
{
    const char *p, *ext;

    for (ext = p = pathname + strlen(pathname); p > pathname; p--) {
        if (p[-1] == '/')
            break;
        if (*p == '.') {
            ext = p;
            break;
        }
    }
    return (char *)ext;
}

static char *getline(char *buf, int buf_size, FILE *f, int strip_comments)
{
    for (;;) {
        char *str;
        int len;

        str = fgets(buf, buf_size, f);
        if (!str)
            return NULL;
        len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
        if (buf[0] == 26) {
            /* handle obsolete DOS ctrl-Z marker */
            return NULL;
        }
        if (strip_comments && (buf[0] == '\0' || buf[0] == '#'))
            continue;

        return str;
    }
}

static void handle_cp(FILE **fp, const char *name, const char *filename)
{
    char line[1024];
    char *p, *q;
    int table[256];
    int min_code, max_code, c1, c2, i, nb, j;
    char name1[256];
    char includename[256];
    int eol_char = 10;

    /* name1 is name with _ changed into - */
    strcpy(name1, name);
    for (p = name1; *p != '\0'; p++) {
        if (*p == '_')
            *p = '-';
    }
    
    /* skip ISO name */
    getline(line, sizeof(line), *fp, 1);

    printf("/*-- file: %s, id: %s, name: %s, ISO name: %s --*/\n\n",
           filename, name, name1, line);

    /* get alias list */
    getline(line, sizeof(line), *fp, 1);

    /* Parse alias list and remove duplicates of name */
    printf("static const char * const aliases_%s[] = {\n"
           "    ", name);

    for (q = line;;) {
        if ((p = strchr(q, '"')) == NULL
        ||  (q = strchr(++p, '"')) == NULL)
            break;

        *q++ = '\0';
        if (strcmp(name1, p)) {
            printf("\"%s\", ", p);
        }
    }
    printf("NULL\n"
           "};\n\n");

    for (i = 0; i < 256; i++) {
        table[i] = i;
    }

    nb = 0;
    for (;;) {
        if (!getline(line, sizeof(line), *fp, 0))
            break;
        if (*line == '[')
            break;
        if (!memcmp(line, "include ", 8)) {
            fclose(*fp);
            strcpy(includename, filename);
            strcpy(get_basename(includename), line + 8);
            filename = includename;
            *fp = fopen(filename, "r");
            if (*fp == NULL) {
                fprintf(stderr, "%s: cannot open %s\n", name, filename);
                break;
            }
            continue;
        }
        if (!strcasecmp(line, "# compatibility"))
            break;
        if (line[0] == '\0' || line[0] == '#')
            continue;
        p = line;
        c1 = strtol(p, (char **)&p, 16);
        if (!isspace(*p)) {
            /* ignore ranges such as "0x20-0x7e       idem" */
            continue;
        }
        c2 = strtol(p, (char **)&p, 16);
        if (c1 >= 256) {
            fprintf(stderr, "%s: ERROR %d %d\n", filename, c1, c2);
            continue;
        }
        table[c1] = c2;
        nb++;
    }
    
    if (table[10] != 10) {
        if (table[0x25] == 0x0A) {
            /* EBCDIC file */
            eol_char = 0x25;
        } else {
            fprintf(stderr, "%s: warning: newline is not preserved\n",
                    filename);
        }
    }
    
    min_code = 0x7fffffff;
    max_code = -1;
    for (i = 0; i < 256; i++) {
        if (table[i] != i) {
            if (i < min_code)
                min_code = i;
            if (i > max_code)
                max_code = i;
        }
    }
    //    fprintf(stderr, "%s: %3d %02x %02x\n", name, nb, min_code, max_code);
    
    if (max_code != -1) {
        printf("static const unsigned short table_%s[%d] = {\n",
               name, max_code - min_code + 1);
        j = 0;
        for (i = min_code; i <= max_code; i++) {
            if ((j & 7) == 0)
                printf("    ");
            printf("0x%04x, ", table[i]);
            if ((j++ & 7) == 7)
                printf("\n");
        }
        if ((j & 7) != 0)
            printf("\n");
        printf("};\n\n");
    }

    printf("QECharset charset_%s = {\n"
           "    \"%s\",\n"
           "    aliases_%s,\n"
           "    decode_8bit_init,\n"
           "    NULL,\n"
           "    encode_8bit,\n"
           "    table_alloc: 1,\n"
           "    eol_char: %d,\n"
           "    min_char: %d,\n"
           "    max_char: %d,\n"
           "    private_table: table_%s,\n"
           "};\n\n",
           name, name1, name, eol_char,
           min_code, max_code, name);

    add_init("    qe_register_charset(&charset_");
    add_init(name);
    add_init(");\n");
}

static FILE *open_index(const char *filename, const char *name)
{
    char line[1024];
    char indexname[256];
    FILE *f;
    int len = strlen(name);

    strcpy(indexname, filename);
    strcpy(get_basename(indexname), "index.cp");
    f = fopen(indexname, "r");
    if (f != NULL) {
        while (getline(line, sizeof(line), f, 1)) {
            if (*line == '[' && line[1 + len] == ']'
            &&  !memcmp(line + 1, name, len)) {
                return f;
            }
        }
        fclose(f);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int i;
    const char *filename;
    char name[256];
    FILE *f;

    printf("/* This file was generated automatically by cptoqe */\n");

    printf("\n" "/*"
           "\n" " * More Charsets and Tables for QEmacs"
           "\n" " *"
           "\n" " * Copyright (c) 2002 Fabrice Bellard."
           "\n" " * Copyright (c) 2007 Charlie Gordon."
           "\n" " *"
           "\n" " * This library is free software; you can redistribute it and/or"
           "\n" " * modify it under the terms of the GNU Lesser General Public"
           "\n" " * License as published by the Free Software Foundation; either"
           "\n" " * version 2 of the License, or (at your option) any later version."
           "\n" " *"
           "\n" " * This library is distributed in the hope that it will be useful,"
           "\n" " * but WITHOUT ANY WARRANTY; without even the implied warranty of"
           "\n" " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU"
           "\n" " * Lesser General Public License for more details."
           "\n" " *"
           "\n" " * You should have received a copy of the GNU Lesser General Public"
           "\n" " * License along with this library; if not, write to the Free Software"
           "\n" " * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA"
           "\n" " */"
           "\n" ""
           "\n" "#include \"qe.h\""
           "\n" "");

    add_init("int charset_more_init(void)\n{\n");

    for (i = 1; i < argc; i++) {
        filename = argv[i];

        strcpy(name, get_basename(filename));
        *get_extension(name) = '\0';
        
        f = fopen(filename, "r");
        if (!f) {
            f = open_index(filename, name);
            if (!f) {
                perror(filename);
                exit(1);
            }
        }

        handle_cp(&f, name, filename);

        fclose(f);
    }

    add_init("\n    return 0;\n}\n\n"
             "qe_module_init(charset_more_init);\n");

    printf("%s", module_init);

    return 0;
}
