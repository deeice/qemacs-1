/*
 * Lisp Source mode for QEmacs.
 *
 * Copyright (c) 2000-2014 Charlie Gordon.
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

#include "qe.h"

/* TODO: lisp-indent = 2 */

#define LISP_LANG_LISP     1
#define LISP_LANG_ELISP    2
#define LISP_LANG_SCHEME   4
#define LISP_LANG_RACKET   8
#define LISP_LANG_CLOJURE  16
#define LISP_LANG_SANDBOX  32

/*---------------- Lisp colors ----------------*/

static const char lisp_keywords[] = {
    "defun|let|let*|set|setq|prog1|progn|cond|if|unless|when|while|"
    "defsubst|remove|otherwise|dolist|incf|decf|boundp|"
    "and|or|not|case|eq|"
    "cons|list|concat|car|cdr|setcar|setcdr|nth|nthcdr|length|"
    "reverse|sort|"
    "caar|cadr|cdar|cddr|caddr|cadddr|"
    "lambda|"
    "\xCE\xBB|"  /* greek character lambda in utf-8 */
    "mod|abs|max|min|log|logand|logior|logxor|ash|"
    "1+|1-|<|>|<=|>=|-|+|*|/|=|<>|/=|"
};

static const char elisp_keywords[] = {
    /* elisp primitives */
    "eval|load|read|null|get|put|catch|throw|unwind-protect|atom|delete|"
    "dotimes|aset|aref|setplist|equal|fset|setq-default|pcase-let|"
    "consp|vectorp|listp|symbolp|stringp|numberp|zerop|functionp|integerp|"
    "assq|apply|funcall|mapatoms|mapc|mapcar|mapconcat|plist-get|plist-put|"
    "function|intern|intern-soft|copy-sequence|delete-dups|fboundp|"
    "push|pop|append|nconc|nreverse|memq|delq|remq|pcase|assoc|member|"
    "defalias|defgroup|defmacro|type-of|condition-case|declare-function|"
    "make-obsolete-variable|define-obsolete-variable-alias|set-default|"
    "default-boundp|default-value|car-safe|cdr-safe|"
    "make-variable-buffer-local|make-local-variable|local-variable-p|"
    "symbol-value|symbol-name|symbol-function|symbol-plist|"
    "string-match|downcase|upcase|string<|string=|format|substring|"
    "string-lessp|string-as-unibyte|"
    "format-time-string|current-time-string|"
    "string-to-number|number-to-string|read-from-string|char-to-string|"
    "make-string|string-to-char|string-equal|split-string|"
    "regexp-quote|"
    "make-vector|vector|vconcat|add-to-list|"
    /* emacs specific stuff */
    "eval-when-compile|assert|cl-assert|cl-pushnew|require|provide|"
    "interactive|save-excursion|save-restriction|error|message|sit-for|"
    "save-match-data|narrow-to-region|widen|"
    "call-interactively|run-hooks|add-hook|remove-hook|run-hook-with-args|"
    "defcustom|defvar|defconst|featurep|defvar-local|commandp|macrop|"
    "define-minor-mode|define-derived-mode|define-major-mode|"
    "define-key|make-keymap|make-sparse-keymap|key-binding|keymapp|lookup-key|"
    /* editing primitives */
    "marker-position|move-marker|copy-marker|set-marker|markerp|point-marker|"
    "mark|point|goto-char|char-after|preceding-char|following-char|"
    "current-column|"
    "move-beginning-of-line|move-end-of-line|beginning-of-line|end-of-line|"
    "count-lines|line-move|open-line|newline|"
    "region-beginning|region-end|line-beginning-position|line-end-position|"
    "line-beginning|line-end|bobp|eobp|bolp|eolp|"
    "forward-word|backward-word|forward-line|backward-line|"
    "forward-char|backward-char|skip-chars-forward|"
    "push-mark|point-min|point-max|exchange-point-and-mark|"
    "match-beginning|match-end|match-data|set-match-data|replace-match|"
    "search-forward|search-backward|re-search-forward|re-search-backward|"
    "looking-at|looking-back|"
    "display-buffer|erase-buffer|current-buffer|"
    "buffer-modified-p|set-buffer-modified-p|"
    "save-current-buffer|buffer-file-name|buffer-name|buffer-list|buffer-size|"
    "set-buffer|"
    "switch-to-buffer|get-buffer-create|kill-buffer|"
    "buffer-substring|buffer-substring-no-properties|"
    "set-text-properties|remove-text-properties|"
    "get-char-property|get-text-property|put-text-property|"
    "read-char|read-string|read-file-name|y-or-n-p|yes-or-no-p|"
    "completing-read|"
    "expand-file-name|file-name-directory|"
    "with-current-buffer|with-temp-buffer|"
    "with-syntax-table|syntax-table|standard-syntax-table|"
    "skip-syntax-forward|skip-syntax-backward|skip-chars-backward|"
    "insert|delete-char|delete-region|prin1|princ|terpri|indent-to|"
    "insert-file-contents|"
    "downcase-region|upcase-region|upcase-initials-region|"
    "delete-horizontal-space|kill-line|kill-region|yank|yank-pop|"
    "move-to-column|align|align-column|align-region|indent-region|"
    "write-region|undo-boundary|undo-in-progress|window-minibuffer-p|"
    "make-overlay|delete-overlay|remove-overlays|move-overlay|"
    "overlay-start|overlay-end|overlays-at|overlay-get|overlay-put|"
    "overlay-buffer|overlay-properties|"
    "define-abbrev|abbrev-get|abbrev-put|"
    "defface|make-face|set-face-property|facep|"
};

static const char scheme_keywords[] = {
    ""
};

static const char racket_keywords[] = {
    ""
};

static const char clojure_keywords[] = {
    ""
};

static const char sandbox_keywords[] = {
    ""
};

static const char lisp_types[] = {
    "nil|t|"
};

static const char elisp_types[] = {
    "nil|t|&optional|"
};

enum {
    IN_LISP_LEVEL    = 0x1F,    /* for IN_LISP_SCOMMENT */
    IN_LISP_COMMENT  = 0x20,
    IN_LISP_STRING   = 0x40,
    IN_LISP_SCOMMENT = 0x80,
};

enum {
    LISP_STYLE_TEXT       = QE_STYLE_DEFAULT,
    LISP_STYLE_COMMENT    = QE_STYLE_COMMENT,
    LISP_STYLE_SCOMMENT   = QE_STYLE_COMMENT,
    LISP_STYLE_NUMBER     = QE_STYLE_NUMBER,
    LISP_STYLE_STRING     = QE_STYLE_STRING,
    LISP_STYLE_CHARCONST  = QE_STYLE_STRING_Q,
    LISP_STYLE_KEYWORD    = QE_STYLE_KEYWORD,
    LISP_STYLE_TYPE       = QE_STYLE_TYPE,
    LISP_STYLE_QSYMBOL    = QE_STYLE_PREPROCESS,
    LISP_STYLE_MACRO      = QE_STYLE_TAG,
    LISP_STYLE_PREPROCESS = QE_STYLE_PREPROCESS,
};

static int lisp_get_symbol(char *buf, int buf_size, unsigned int *p)
{
    buf_t outbuf, *out;
    unsigned int c;
    int i;

    out = buf_init(&outbuf, buf, buf_size);

    for (i = 0; (c = p[i]) != '\0'; i++) {
        if (qe_isblank(c) || qe_findchar(";(){}[]#'`,\"", c))
            break;
        buf_putc_utf8(out, c);
    }
    return i;
}

static int lisp_is_number(const char *str)
{
    int i;

    if (*str == 'b' && str[1]) {
        for (str++; qe_isbindigit(*str); str++)
            continue;
    } else
    if (*str == 'o' && str[1]) {
        for (str++; qe_isoctdigit(*str); str++)
            continue;
    } else
    if (*str == 'x' && str[1]) {
        for (str++; qe_isxdigit(*str); str++)
            continue;
    } else {
        if ((*str == '-' || *str == 'd') && str[1])
            str++;
        if (qe_isdigit(*str)) {
            for (; qe_isdigit(*str); str++)
                continue;
            if (*str == '.') {
                for (str++; qe_isdigit(*str); str++)
                    continue;
            }
            if (qe_tolower(*str) == 'e') {
                i = 1;
                if (str[i] == '+' || str[i] == '-')
                    i++;
                if (qe_isdigit(str[i])) {
                    for (str += i + 1; qe_isdigit(*str); str++)
                        continue;
                }
            }
        }
    }
    return (*str) ? 0 : 1;
}

static void lisp_colorize_line(QEColorizeContext *cp,
                               unsigned int *str, int n, ModeDef *syn)
{
    int colstate = cp->colorize_state;
    int i = 0, start = i, len, level, style, has_expr;
    int mode_flags = syn->colorize_flags;
    char kbuf[32];

    level = colstate & IN_LISP_LEVEL;
    style = 0;
    has_expr = 0;

    if (colstate & IN_LISP_SCOMMENT)
        style = LISP_STYLE_SCOMMENT;
    if (colstate & IN_LISP_STRING)
        goto parse_string;
    if (colstate & IN_LISP_COMMENT)
        goto parse_comment;

    while (i < n) {
        has_expr = 0;
        start = i;
        switch (str[i++]) {
        case ',':
            if (str[i] == '@')
                i++;
            /* FALL THRU */
        case '`':
            if (style)
                break;
            SET_COLOR(str, start, i, LISP_STYLE_MACRO);
            continue;
        case ';':
            i = n;
            SET_COLOR(str, start, i, LISP_STYLE_COMMENT);
            continue;
        case '(':
            if (colstate & IN_LISP_SCOMMENT)
                level++;
            break;
        case ')':
            if (colstate & IN_LISP_SCOMMENT) {
                if (level-- <= 1) {
                    SET_COLOR(str, start, i - (level < 0), style);
                    level = 0;
                    style = 0;
                    colstate &= ~IN_LISP_SCOMMENT;
                    continue;
                }
            }
            break;
        case '#':
            if (str[i] == '|') {
                /* #| ... |# -> block comment */
                colstate |= IN_LISP_COMMENT;
                i++;
            parse_comment:
                for (; i < n; i++) {
                    if (str[i] == '|' && str[i + 1] == '#') {
                        i += 2;
                        colstate &= ~IN_LISP_COMMENT;
                        break;
                    }
                }
                SET_COLOR(str, start, i, LISP_STYLE_COMMENT);
                continue;
            }
            if (str[i] == ';') {
                /* #; sexpr -> comment out sexpr */
                i++;
                colstate |= IN_LISP_SCOMMENT;
                style = LISP_STYLE_SCOMMENT;
                break;
            }
            if (str[i] == '"') {
                i++;
                colstate |= IN_LISP_STRING;
                goto parse_string;
            }
            if (str[i] == ':'
            &&  (str[i + 1] == '-' || qe_isalnum_(str[i + 1]))) {
                len = lisp_get_symbol(kbuf, sizeof(kbuf), str + i + 1);
                i += 1 + len;
                goto has_symbol;
            }
            if (qe_isalpha_(str[i])) {
                len = lisp_get_symbol(kbuf, sizeof(kbuf), str + i);
                i += len;
                if (!strcmp(kbuf, "t") || !strcmp(kbuf, "f")) {
                    /* #f -> false, #t -> true */
                    goto has_qsymbol;
                }
                if (mode_flags & LISP_LANG_RACKET) {
                    if (start == 0 && !strcmp(kbuf, "lang")) {
                        i = n;
                        SET_COLOR(str, start, i, LISP_STYLE_PREPROCESS);
                        continue;
                    }
                    if (!strcmp(kbuf, "rx") || !strcmp(kbuf, "px")) {
                        if (str[i] == '"') {
                            /* #rx"regex" */
                            i += 1;
                            colstate |= IN_LISP_STRING;
                            goto parse_string;
                        }
                        if (str[i] == '#' && str[i + 1] == '"') {
                            /* #rx#"regex" */
                            i += 2;
                            colstate |= IN_LISP_STRING;
                            goto parse_string;
                        }
                    }
                }
                /* #b[01]+  -> binary constant */
                /* #o[0-7]+  -> octal constant */
                /* #d[0-9]+  -> decimal constant */
                /* #x[0-9a-fA-F]+  -> hex constant */
                goto has_symbol;
            }
            if (str[i] == '\\') {
                if (qe_isalnum_(str[i + 1])) {
                    /* #\x[0-9a-fA-F]+  -> hex char constant */
                    /* #\[a-zA-Z0-9]+  -> named char constant */
                    len = lisp_get_symbol(kbuf, sizeof(kbuf), str + i + 1);
                    i += 1 + len;
                    goto has_char_const;
                }
                if (i + 1 < n) {
                    i += 2;
                    goto has_char_const;
                }
            }
            {
                /* #( ... )  -> vector object */
                /* #! ... \n  -> line comment */
                /* # SPC  -> NIL ? */
            }
            break;
        case '"':
            /* parse string const */
            colstate |= IN_LISP_STRING;
        parse_string:
            while (i < n) {
                if (str[i] == '\\' && ++i < n) {
                    i++;
                } else
                if (str[i++] == '"') {
                    colstate &= ~IN_LISP_STRING;
                    has_expr = 1;
                    break;
                }
            }
            if (style)
                break;
            SET_COLOR(str, start, i, LISP_STYLE_STRING);
            continue;
        case '?':
            /* parse char const */
            /* XXX: Should parse keys syntax */
            if (str[i] == '\\' && i + 1 < n) {
                i += 2;
            } else
            if (i < n) {
                i += 1;
            }
        has_char_const:
            has_expr = 1;
            if (style)
                break;
            SET_COLOR(str, start, i, LISP_STYLE_CHARCONST);
            continue;
        case '\'':
            len = lisp_get_symbol(kbuf, sizeof(kbuf), str + i);
            if (len > 0) {
                i += len;
            has_qsymbol:
                has_expr = 1;
                if (style)
                    break;
                SET_COLOR(str, start, i, LISP_STYLE_QSYMBOL);
                continue;
            }
            break;
        default:
            len = lisp_get_symbol(kbuf, sizeof(kbuf), str + i - 1);
            if (len > 0) {
                i += len - 1;
            has_symbol:
                has_expr = 1;
                if (style)
                    break;
                if (lisp_is_number(kbuf)) {
                    SET_COLOR(str, start, i, LISP_STYLE_NUMBER);
                    continue;
                }
                if (strfind(lisp_keywords, kbuf)
                ||  strfind(syn->keywords, kbuf)) {
                    SET_COLOR(str, start, i, LISP_STYLE_KEYWORD);
                    continue;
                }
                if (strfind(syn->types, kbuf)) {
                    SET_COLOR(str, start, i, LISP_STYLE_TYPE);
                    continue;
                }
                /* skip other symbol */
                continue;
            }
            break;
        }
        if (style) {
            SET_COLOR(str, start, i, style);
            if (has_expr) {
                if ((colstate & IN_LISP_SCOMMENT) && level == 0) {
                    colstate &= ~IN_LISP_SCOMMENT;
                    style = 0;
                }
            }
        }
    }
    colstate = (colstate & ~IN_LISP_LEVEL) | (level & IN_LISP_LEVEL);
    cp->colorize_state = colstate;
}

static int elisp_mode_probe(ModeDef *mode, ModeProbeData *p)
{
    /* check file name or extension */
    if (match_extension(p->filename, mode->extensions)
    ||  match_shell_handler(cs8(p->buf), mode->shell_handlers)
    ||  strstart(p->filename, ".emacs", NULL))
        return 80;

    return 1;
}

ModeDef lisp_mode = {
    .name = "Lisp",
    .extensions = "ll|li|lh|lo|lm|lisp",
    .keywords = NULL,
    .shell_handlers = "lisp",
    .types = lisp_types,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_LISP,
};

ModeDef elisp_mode = {
    .name = "ELisp",
    .extensions = "el",
    .keywords = elisp_keywords,
    .types = elisp_types,
    .mode_probe = elisp_mode_probe,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_ELISP,
    .fallback = &lisp_mode,
};

ModeDef scheme_mode = {
    .name = "Scheme",
    .extensions = "scm|sch|ss",
    .keywords = scheme_keywords,
    .types = lisp_types,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_SCHEME,
    .fallback = &lisp_mode,
};

ModeDef racket_mode = {
    .name = "Racket",
    .extensions = "rkt|rktd",
    .keywords = racket_keywords,
    .types = lisp_types,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_RACKET,
    .fallback = &lisp_mode,
};

ModeDef clojure_mode = {
    .name = "Clojure",
    .extensions = "clj",
    .keywords = clojure_keywords,
    .types = lisp_types,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_CLOJURE,
    .fallback = &lisp_mode,
};

ModeDef sandbox_mode = {
    .name = "Sandbox",  /* MacOS, Tinyscheme based? */
    .extensions = "sb",
    .keywords = sandbox_keywords,
    .types = lisp_types,
    .colorize_func = lisp_colorize_line,
    .colorize_flags = LISP_LANG_SANDBOX,
    .fallback = &lisp_mode,
};

static int lisp_init(void)
{
    qe_register_mode(&lisp_mode, MODEF_SYNTAX);
    qe_register_mode(&elisp_mode, MODEF_SYNTAX);
    qe_register_mode(&scheme_mode, MODEF_SYNTAX);
    qe_register_mode(&racket_mode, MODEF_SYNTAX);
    qe_register_mode(&clojure_mode, MODEF_SYNTAX);
    qe_register_mode(&sandbox_mode, MODEF_SYNTAX);

    return 0;
}

qe_module_init(lisp_init);
