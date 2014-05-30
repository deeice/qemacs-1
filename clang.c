/*
 * C mode for QEmacs.
 *
 * Copyright (c) 2001-2002 Fabrice Bellard.
 * Copyright (c) 2002-2014 Charlie Gordon.
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

/* C mode flavors */
enum {
    CLANG_C,
    CLANG_CPP,
    CLANG_OBJC,
    CLANG_CSHARP,
    CLANG_CSS,
    CLANG_JS,
    CLANG_AS,
    CLANG_JAVA,
    CLANG_PHP,
    CLANG_GO,
    CLANG_D,
    CLANG_LIMBO,
    CLANG_CYCLONE,
    CLANG_CH,
    CLANG_SQUIRREL,
    CLANG_ICI,
    CLANG_JSX,
    CLANG_HAXE,
    CLANG_DART,
    CLANG_PIKE,
    CLANG_IDL,
    CLANG_CALC,
    CLANG_ENSCRIPT,
    CLANG_QSCRIPT,
    CLANG_FLAVOR = 0x1F,
};

/* C mode options */
#define CLANG_LEX         0x0200
#define CLANG_YACC        0x0400
#define CLANG_REGEX       0x0800
#define CLANG_WLITERALS   0x1000
#define CLANG_PREPROC     0x2000
#define CLANG_CC          0x3100  /* all C language features */

static const char c_keywords[] = {
    "auto|break|case|const|continue|default|do|else|enum|extern|for|goto|"
    "if|inline|register|restrict|return|sizeof|static|struct|switch|"
    "typedef|union|volatile|while|"
};

static const char c_types[] = {
    "char|double|float|int|long|unsigned|short|signed|void|va_list|"
    "_Bool|_Complex|_Imaginary|"
};

static const char cpp_keywords[] = {
    "asm|catch|class|delete|friend|inline|new|operator|"
    "private|protected|public|template|try|this|virtual|throw|"
    // XXX: many missing keywords
};

static const char cpp_types[] = {
    ""
};

static const char objc_keywords[] = {
    "self|super|class|nil|YES|NO|"
    "@class|@interface|@implementation|@public|@private|@protected|"
    "@try|@catch|@throw|@finally|@end|@protocol|@selector|@synchronized|"
    "@encode|@defs|@optional|@required|@property|@dynamic|@synthesize|"
    // context sensitive keywords
    "in|out|inout|bycopy|byref|oneway|"
    "getter|setter|readwrite|readonly|assign|retain|copy|nonatomic|"
};

static const char objc_types[] = {
    "id|BOOL|SEL|"
};

static const char csharp_keywords[] = {
    "abstract|as|base|break|case|catch|checked|class|const|continue|"
    "default|delegate|do|else|enum|event|explicit|extern|false|finally|"
    "fixed|for|foreach|goto|if|implicit|in|interface|internal|is|lock|"
    "namespace|new|null|operator|out|override|params|private|protected|"
    "public|readonly|ref|return|sealed|sizeof|stackalloc|static|"
    "struct|switch|template|this|throw|true|try|typeof|unchecked|unsafe|"
    "using|virtual|volatile|while|"

    /* contextual keywords */
    "add|remove|yield|partial|get|set|where|"
};

static const char csharp_types[] = {
    "bool|byte|char|decimal|double|float|int|long|object|sbyte|short|"
    "string|uint|ulong|ushort|void|"
    "Boolean|Byte|DateTime|Exception|Int32|Int64|Object|String|Thread|"
    "UInt32|UInt64|"
};

static const char java_keywords[] = {
    "abstract|assert|break|case|catch|class|const|continue|"
    "default|do|else|extends|false|final|finally|for|function|"
    "if|implements|import|in|instanceof|interface|native|new|null|"
    "package|private|protected|public|return|"
    "static|super|switch|synchronized|"
    "this|throw|throws|transient|true|try|var|while|with|"
    "@Override|@SuppressWarnings|@Deprecated|"
};

static const char java_types[] = {
    "boolean|byte|char|double|float|int|long|short|void|"
    "String|Object|List|Set|Exception|Thread|Class|HashMap|Integer|"
    "Collection|Block|"
};

static const char css_keywords[] = {
    "|"
};

static const char css_types[] = {
    "|"
};

static const char js_keywords[] = {
    "break|case|catch|continue|debugger|default|delete|do|"
    "else|finally|for|function|if|in|instanceof|new|"
    "return|switch|this|throw|try|typeof|while|with|"
    /* FutureReservedWord */
    "class|const|enum|import|export|extends|super|"
    /* The following tokens are also considered to be
     * FutureReservedWords when parsing strict mode code */
    "implements|interface|let|package|private|protected|"
    "public|static|yield|"
    /* constants */
    "undefined|null|true|false|Infinity|NaN|"
    /* strict mode quasi keywords */
    "eval|arguments|"
};

static const char js_types[] = {
    "void|var|"
};

static const char as_keywords[] = {
    "as|break|case|catch|class|continue|default|do|else|false|"
    "finally|for|function|if|import|interface|internal|is|new|null|"
    "package|private|protected|public|return|super|switch|this|throw|"
    "true|try|while|"
    // The following AS3 keywords are no longer in AS4:
    "delete|include|instanceof|namespace|typeof|use|with|in|const|"
    // other constants
    "undefined|Infinity|NaN|"
    // introduced in AS4 (spec abandoned in december 2012)
    //"let|defer|get|set|override|native|extends|implements|"
};

static const char as_types[] = {
    "void|var|bool|byte|int|uint|long|ulong|float|double|"
    "Array|Boolean|Number|Object|String|Function|Event|RegExp|"
    "Class|Interface|"
};

static const char jsx_keywords[] = {
    // literals shared with ECMA 262
    "null|true|false|NaN|Infinity|"
    // keywords shared with ECMA 262
    "break|case|const|do|else|finally|for|function|if|in|"
    "instanceof|new|return|switch|this|throw|try|typeof|var|while|"
    // JSX keywords
    "class|extends|super|import|implements|static|"
    "__FILE__|__LINE__|undefined|"
    // contextual keywords
    // "assert|log|catch|continue|default|delete|interface",
    // ECMA 262 literals but not used by JSX
    "debugger|with|"
    // ECMA 262 future reserved words
    "export|"
    // ECMA 262 strict mode future reserved words
    "let|private|public|yield|protected|"
    // JSX specific reserved words
    "extern|native|as|operator|"
};

static const char jsx_types[] = {
    "void|variant|boolean|int|number|string|Error|"
};

static const char haxe_keywords[] = {
    "abstract|break|case|cast|catch|class|continue|default|do|dynamic|else|"
    "enum|extends|extern|false|for|function|if|implements|import|inline|"
    "interface|in|macro|new|null|override|package|private|public|return|"
    "static|switch|this|throw|true|try|typedef|untyped|using|var|while|"
};

static const char haxe_types[] = {
    "Void|Array|Bool|Int|Float|Class|Enum|Dynamic|String|Date|Null|"
    "Iterator|"
};

static const char php_keywords[] = {
    "abstract|assert|break|case|catch|class|clone|const|continue|"
    "declare|default|elseif|else|enddeclare|endif|endswitch|end|exit|"
    "extends|false|final|foreach|for|function|goto|if|implements|"
    "include_once|include|instanceof|interface|list|namespace|new|"
    "overload|parent|private|public|require_once|require|return|"
    "self|sizeof|static|switch|throw|trait|true|try|use|var|while|"
    "NULL|"
};

static const char php_types[] = {
    "array|boolean|bool|double|float|integer|int|object|real|string|"
};

static const char go_keywords[] = {
    /* keywords */
    "break|case|chan|const|continue|default|defer|else|fallthrough|"
    "for|func|go|goto|if|import|interface|map|package|range|"
    "return|select|struct|switch|type|var|"
    /* builtins */
    "append|cap|close|complex|copy|delete|imag|len|make|new|panic|"
    "print|println|real|recover|"
    /* Constants */
    "false|iota|nil|true|"
};

static const char go_types[] = {
    "bool|byte|complex128|complex64|error|float32|float64|"
    "int|int16|int32|int64|int8|rune|string|"
    "uint|uint16|uint32|uint64|uint8|uintptr|"
};

static const char d_keywords[] = {
    "abstract|alias|align|asm|assert|auto|body|break|"
    "case|cast|catch|class|const|continue|debug|default|"
    "delegate|deprecated|do|else|enum|export|extern|false|"
    "final|finally|for|foreach|foreach_reverse|function|goto|"
    "if|immutable|import|in|inout|int|interface|invariant|is|"
    "lazy|mixin|module|new|nothrow|null|out|override|package|"
    "pragma|private|protected|public|pure|ref|return|scope|shared|"
    "static|struct|super|switch|synchronized|template|this|throw|"
    "true|try|typeid|typeof|union|unittest|version|while|with|"
    "delete|typedef|volatile|"  /* deprecated */
    "macro|"    /* reserved, unused */
    "__FILE__|__MODULE__|__LINE__|__FUNCTION__|__PRETTY_FUNCTION__|"
    "__gshared|__traits|__vector|__parameters|"
    "__DATE__|__EOF__|__TIME__|__TIMESPAMP__|__VENDOR__|__VERSION__|"
};

static const char d_types[] = {
    "bool|byte|ubyte|short|ushort|int|uint|long|ulong|char|wchar|dchar|"
    "float|double|real|ifloat|idouble|ireal|cfloat|cdouble|creal|void|"
    "|cent|ucent|string|wstring|dstring|size_t|ptrdiff_t|"
};

static const char limbo_keywords[] = {
    "adt|alt|array|break|case|chan|con|continue|cyclic|do|else|exit|"
    "fn|for|hd|if|implement|import|include|len|list|load|module|nil|"
    "of|or|pick|ref|return|self|spawn|tagof|tl|to|type|while|"
};

static const char limbo_types[] = {
    "big|byte|int|real|string|"
};

static const char cyclone_keywords[] = {
    "auto|break|case|const|continue|default|do|else|enum|extern|for|goto|"
    "if|inline|register|restrict|return|sizeof|static|struct|switch|"
    "typedef|union|volatile|while|"
    "abstract|alias|as|catch|datatype|export|fallthru|inject|let|"
    "namespace|new|numelts|offsetof|region|regions|reset_region|rnew|"
    "tagcheck|throw|try|using|valueof|"
    "calloc|malloc|rcalloc|rmalloc|"
    "NULL|"
};

static const char cyclone_types[] = {
    "char|double|float|int|long|unsigned|short|signed|void|"
    "_Bool|_Complex|_Imaginary|"
    "bool|dynregion_t|region_t|tag_t|valueof_t|"
    "@numelts|@region|@thin|@fat|@zeroterm|@nozeroterm|@notnull|@nullable|"
    "@extensible|@tagged"
};

static const char ch_keywords[] = {
    "local|offsetof|Inf|NaN|"
};

static const char ch_types[] = {
    "complex|"
};

static const char squirrel_keywords[] = {
    "base|break|continue|const|extends|for|null|throw|try|instanceof|true|"
    "case|catch|class|clone|default|delete|else|enum|foreach|function|if|in|"
    "resume|return|switch|this|typeof|while|yield|constructor|false|static|"
};

static const char squirrel_types[] = {
    "local|"
};

static const char ici_keywords[] = {
    "array|break|case|class|continue|default|do|else|extern|float|"
    "for|forall|func|if|in|module|NULL|onerror|return|set|static|struct|"
    "switch|try|while|"
};

static const char ici_types[] = {
    "auto|"
};

static const char dart_keywords[] = {
    "abstract|as|assert|break|call|case|catch|class|const|continue|default|do|"
    "else|equals|extends|external|factory|false|final|finally|for|"
    "get|if|implements|in|interface|is|negate|new|null|on|operator|return|"
    "set|show|static|super|switch|this|throw|true|try|typedef|while|"
    // should match only at line start
    "import|include|source|library|"
    "@observable|@published|@override|@runTest|"
    // XXX: should colorize is! as a keyword
};

static const char dart_types[] = {
    "bool|double|dynamic|int|num|var|void|"
    "String|StringBuffer|Object|RegExp|Function|"
    "Date|DateTime|TimeZone|Duration|Stopwatch|DartType|"
    "Collection|Comparable|Completer|Future|Match|Options|Pattern|"
    "HashMap|HashSet|Iterable|Iterator|LinkedHashMap|List|Map|Queue|Set|"
    "Dynamic|Exception|Error|AssertionError|TypeError|FallThroughError|" 
};

static const char pike_keywords[] = {
    "break|case|catch|class|constant|continue|default|do|else|enum|extern|"
    "final|for|foreach|gauge|global|if|import|inherit|inline|"
    "lambda|local|nomask|optional|predef|"
    "private|protected|public|return|sscanf|static|switch|typedef|typeof|"
    "while|__attribute__|__deprecated__|__func__|"
};

static const char pike_types[] = {
    "array|float|int|string|function|mapping|multiset|mixed|object|program|"
    "variant|void|"
};

static const char idl_keywords[] = {
    "abstract|attribute|case|component|const|consumes|context|custom|"
    "default|emits|enum|eventtype|exception|factory|false|FALSE|finder|"
    "fixed|getraises|home|import|in|inout|interface|local|module|multiple|"
    "native|oneway|out|primarykey|private|provides|public|publishes|raises|"
    "readonly|sequence|setraises|struct|supports|switch|TRUE|true|"
    "truncatable|typedef|typeid|typeprefix|union|uses|ValueBase|valuetype|"
};

static const char idl_types[] = {
    "unsigned|short|long|float|double|char|wchar|string|wstring|octet|any|void|"
    "boolean|Boolean|object|Object|"
};

static const char calc_keywords[] = {
    "if|else|for|while|do|continue|break|goto|return|local|global|static|"
    "switch|case|default|quit|exit|define|read|show|help|write|mat|obj|"
    "print|cd|undefine|abort|"
};

static const char calc_types[] = {
    "|"
};

static const char enscript_keywords[] = {
    "if|else|return|state|extends|BEGIN|END|forever|continue|do|"
    "not|and|or|orelse|switch|case|default|true|false|"
};

static const char enscript_types[] = {
    "|"
};

static const char qs_keywords[] = {
    "break|case|class|continue|def|default|del|delete|do|else|for|"
    "function|if|module|new|return|self|string|struct|switch|this|"
    "typeof|while|"
};

static const char qs_types[] = {
    "char|int|var|void|Array|Char|Function|Number|Object|String|"
};

static const char c_extensions[] = {
    "c|h|i|C|H|I|"      /* C language */
    /* Other C flavors */
    "e|"                /* EEL */
    "ecp|"              /* Informix embedded C */
    "pgc|"              /* Postgres embedded C */
    "pcc|"              /* Oracle C++ */
};

/* grab a C identifier from a uint buf, stripping color.
 * return char count.
 */
static int get_c_identifier(char *buf, int buf_size, unsigned int *p,
                            int flavor)
{
    unsigned int c;
    int i, j;

    i = j = 0;
    c = p[i] & CHAR_MASK;
    if (qe_isalpha_(c) || c == '$' || (c == '@' && flavor != CLANG_PIKE)) {
        for (;;) {
            if (j < buf_size - 1)
                buf[j++] = c;
            i++;
            c = p[i] & CHAR_MASK;
            if (c == '-' && flavor == CLANG_CSS)
                continue;
            if (!qe_isalnum_(c))
                break;
        }
    }
    buf[j] = '\0';
    return i;
}

enum {
    C_STYLE_DEFAULT    = 0,
    C_STYLE_PREPROCESS = QE_STYLE_PREPROCESS,
    C_STYLE_COMMENT    = QE_STYLE_COMMENT,
    C_STYLE_REGEX      = QE_STYLE_STRING_Q,
    C_STYLE_STRING     = QE_STYLE_STRING,
    C_STYLE_STRING_Q   = QE_STYLE_STRING_Q,
    C_STYLE_STRING_BQ  = QE_STYLE_STRING,
    C_STYLE_NUMBER     = QE_STYLE_NUMBER,
    C_STYLE_KEYWORD    = QE_STYLE_KEYWORD,
    C_STYLE_TYPE       = QE_STYLE_TYPE,
    C_STYLE_FUNCTION   = QE_STYLE_FUNCTION,
    C_STYLE_VARIABLE   = QE_STYLE_VARIABLE,
};

/* c-mode colorization states */
enum {
    IN_C_COMMENT    = 0x01,  /* multiline comment */
    IN_C_COMMENT1   = 0x02,  /* single line comment with \ at EOL */
    IN_C_STRING     = 0x04,  /* double-quoted string */
    IN_C_STRING_Q   = 0x08,  /* single-quoted string */
    IN_C_STRING_BQ  = 0x10,  /* back-quoted string (go's multi-line string) */
    IN_C_PREPROCESS = 0x20,  /* preprocessor directive with \ at EOL */
    IN_C_REGEX      = 0x40,  /* regex */
    IN_C_CHARCLASS  = 0x80,  /* regex char class */
    IN_C_COMMENT_D  = 0x700, /* nesting D comment level (max 7 deep) */
    IN_C_COMMENT_D_SHIFT = 8,
};

static void c_colorize_line(QEColorizeContext *cp,
                            unsigned int *str, int n, ModeDef *syn)
{
    const char *keywords = NULL;
    const char *types = NULL;
    int i = 0, start, i1, i2, indent, level;
    int c, state, style, style0, style1, type_decl, klen, delim, flavor;
    int mode_flags;
    char kbuf[32];

    mode_flags = syn ? syn->colorize_flags : 0;
    flavor = (mode_flags & CLANG_FLAVOR);
    keywords = syn ? syn->keywords : NULL;
    types = syn ? syn->types : NULL;

    for (indent = 0; qe_isspace(str[indent]); indent++)
        continue;

    state = cp->colorize_state;
    start = i;
    type_decl = 0;

    if (i >= n)
        goto the_end;

    c = 0;      /* turn off stupid egcs-2.91.66 warning */
    style0 = style = C_STYLE_DEFAULT;

    if (state) {
        /* if already in a state, go directly in the code parsing it */
        if (state & IN_C_PREPROCESS)
            style0 = style = C_STYLE_PREPROCESS;
        if (state & IN_C_COMMENT)
            goto parse_comment;
        if (state & IN_C_COMMENT1)
            goto parse_comment1;
        if (state & IN_C_COMMENT_D)
            goto parse_comment_d;
        if (state & IN_C_STRING)
            goto parse_string;
        if (state & IN_C_STRING_Q)
            goto parse_string_q;
        if (state & IN_C_STRING_BQ)
            goto parse_string_bq;
        if (state & IN_C_REGEX) {
            delim = '/';
            goto parse_regex;
        }
    }

    while (i < n) {
        start = i;
        c = str[i++];

        switch (c) {
        case '/':
            if (str[i] == '*') {
                /* normal comment */
                i++;
            parse_comment:
                style = C_STYLE_COMMENT;
                state |= IN_C_COMMENT;
                for (; i < n; i++) {
                    if (str[i] == '*' && str[i + 1] == '/') {
                        i += 2;
                        state &= ~IN_C_COMMENT;
                        style = style0;
                        break;
                    }
                }
                SET_COLOR(str, start, i, C_STYLE_COMMENT);
                continue;
            } else
            if (str[i] == '/') {
                /* line comment */
            parse_comment1:
                style = C_STYLE_COMMENT;
                state |= IN_C_COMMENT1;
                i = n;
                SET_COLOR(str, start, i, C_STYLE_COMMENT);
                continue;
            }
            /* XXX: should use more context to tell regex from divide */
            if ((mode_flags & CLANG_REGEX)
            &&  (start == indent
            ||   (str[i] != ' ' && (str[i] != '=' || str[i + 1] != ' ')
            &&    !qe_isalnum(str[start - 1] & CHAR_MASK)
            &&    str[start - 1] != ')'))) {
                /* parse regex */
                state = IN_C_REGEX;
                delim = '/';
            parse_regex:
                style = C_STYLE_REGEX;
                while (i < n) {
                    c = str[i++];
                    if (c == '\\') {
                        if (i < n) {
                            i += 1;
                        }
                    } else
                    if (state & IN_C_CHARCLASS) {
                        if (c == ']') {
                            state &= ~IN_C_CHARCLASS;
                        }
                        /* ECMA 5: ignore '/' inside char classes */
                    } else {
                        if (c == '[') {
                            state |= IN_C_CHARCLASS;
                        } else
                        if (c == delim) {
                            while (qe_isalnum_(str[i])) {
                                i++;
                            }
                            state = 0;
                            style = style0;
                            break;
                        }
                    }
                }
                SET_COLOR(str, start, i, C_STYLE_REGEX);
                continue;
            }
            if (flavor == CLANG_D && (str[i] == '+')) {
                /* D language nesting long comment */
                i++;
                state |= (1 << IN_C_COMMENT_D_SHIFT);
            parse_comment_d:
                style = C_STYLE_COMMENT;
                level = (state & IN_C_COMMENT_D) >> IN_C_COMMENT_D_SHIFT;
                while (i < n) {
                    if (str[i] == '/' && str[i + 1] == '+') {
                        i += 2;
                        level++;
                    } else
                    if (str[i] == '+' && str[i + 1] == '/') {
                        i += 2;
                        level--;
                        if (level == 0) {
                            state &= ~IN_C_COMMENT;
                            style = style0;
                            break;
                        }
                    } else {
                        i++;
                    }
                }
                state = (state & ~IN_C_COMMENT_D) |
                        (min(level, 7) << IN_C_COMMENT_D_SHIFT);
                SET_COLOR(str, start, i, C_STYLE_COMMENT);
                continue;
            }
            break;
        case '#':       /* preprocessor */
            if (mode_flags & CLANG_PREPROC) {
                state = IN_C_PREPROCESS;
                style = style0 = C_STYLE_PREPROCESS;
            }
            if (flavor == CLANG_D) {
                /* only #line is supported, but can occur anywhere */
                state = IN_C_PREPROCESS;
                style = style0 = C_STYLE_PREPROCESS;
            }
            if (flavor == CLANG_PHP || flavor == CLANG_LIMBO || flavor == CLANG_SQUIRREL) {
                goto parse_comment1;
            }
            if (flavor == CLANG_ICI) {
                delim = '#';
                goto parse_regex;
            }
            if (flavor == CLANG_HAXE) {
                i += get_c_identifier(kbuf, countof(kbuf), str + i, flavor);
                // XXX: check for proper preprocessor directive?
                SET_COLOR(str, start, i, C_STYLE_PREPROCESS);
                continue;
            }
            if (flavor == CLANG_PIKE) {
                if (str[i] == '\"') {
                    i++;
                    goto parse_string;
                }
                state = IN_C_PREPROCESS;
                style = style0 = C_STYLE_PREPROCESS;
            }
            break;
        case 'L':       /* wide character and string literals */
            if (mode_flags & CLANG_WLITERALS) {
                if (str[i] == '\'') {
                    i++;
                    goto parse_string_q;
                }
                if (str[i] == '\"') {
                    i++;
                    goto parse_string;
                }
            }
            goto normal;
        // case 'r':
            /* XXX: D language r" wysiwyg chars " */
        // case 'X':
            /* XXX: D language X" hex string chars " */
        // case 'q':
            /* XXX: D language q" delim wysiwyg chars delim " */
            /* XXX: D language q{ tokens } */
        case '\'':      /* character constant */
        parse_string_q:
            state |= IN_C_STRING_Q;
            style1 = C_STYLE_STRING_Q;
            delim = '\'';
            goto string;
        case '`':
            if (flavor == CLANG_GO || flavor == CLANG_D) {
                /* go language multi-line string, no escape sequences */
            parse_string_bq:
                state |= IN_C_STRING_BQ;
                style1 = C_STYLE_STRING_BQ;
                delim = '`';
                while (i < n) {
                    c = str[i++];
                    if (c == delim) {
                        state &= ~IN_C_STRING_BQ;
                        break;
                    }
                }
                if (state & IN_C_PREPROCESS)
                    style1 = C_STYLE_PREPROCESS;
                SET_COLOR(str, start, i, style1);
                continue;
            }
            break;
        case '@':
            if (flavor == CLANG_CSHARP || flavor == CLANG_SQUIRREL) {
                if (str[i] == '\"') {
                    /* Csharp and Squirrel Verbatim strings */
                    /* ignore escape sequences and newlines */
                    state |= IN_C_STRING;   // XXX: IN_RAW_STRING 
                    style1 = C_STYLE_STRING;
                    delim = str[i];
                    style = style1;
                    for (i++; i < n;) {
                        c = str[i++];
                        if (c == delim) {
                            if (str[i] == (unsigned int)c) {
                                i++;
                                continue;
                            }
                            state &= ~(IN_C_STRING | IN_C_STRING_Q | IN_C_STRING_BQ);
                            style = style0;
                            break;
                        }
                    }
                    SET_COLOR(str, start, i, style1);
                    continue;
                }
            }
            goto normal;

        case '\"':      /* string literal */
        parse_string:
            state |= IN_C_STRING;
            style1 = C_STYLE_STRING;
            delim = '\"';
        string:
            // XXX: should handle triple quoted strings (Dart)
            style = style1;
            while (i < n) {
                c = str[i++];
                if (c == '\\') {
                    if (i >= n)
                        break;
                    i++;
                } else
                if (c == delim) {
                    state &= ~(IN_C_STRING | IN_C_STRING_Q | IN_C_STRING_BQ);
                    style = style0;
                    break;
                }
            }
            if (flavor == CLANG_D) {
                /* ignore optional string postfix */
                if (qe_findchar("cwd", str[i]))
                    i++;
            }
            if (state & IN_C_PREPROCESS)
                style1 = C_STYLE_PREPROCESS;
            SET_COLOR(str, start, i, style1);
            continue;
        case '=':
            /* exit type declaration */
            /* does not handle this: int i = 1, j = 2; */
            type_decl = 0;
            break;
        case '<':       /* JavaScript extension */
            if (flavor == CLANG_JS) {
                if (str[i] == '!' && str[i + 1] == '-' && str[i + 2] == '-')
                    goto parse_comment1;
            }
            break;
        default:
        normal:
            if (state & IN_C_PREPROCESS)
                break;
            if (qe_isdigit(c)) {
                /* XXX: should parse actual number syntax */
                /* XXX: D lang ignores embedded '_' and accepts 'l'
                 * 'u' or 'U' 'f' or 'F', 'i' suffixes */
                while (qe_isalnum(str[i]) || str[i] == '.') {
                    i++;
                }
                SET_COLOR(str, start, i, C_STYLE_NUMBER);
                continue;
            }
            if (qe_isalpha_(c) || c == '$' || (c == '@' && flavor != CLANG_PIKE)) {
                /* XXX: should support :: */
                klen = get_c_identifier(kbuf, countof(kbuf), str + start, flavor);
                i = start + klen;

                if ((keywords && strfind(keywords, kbuf))
                ||  ((mode_flags & CLANG_CC) && strfind(c_keywords, kbuf))
                ||  ((flavor == CLANG_CSS) && str[i] == ':')
                   ) {
                    SET_COLOR(str, start, i, C_STYLE_KEYWORD);
                    continue;
                }

                i1 = i;
                while (qe_isblank(str[i1]))
                    i1++;
                i2 = i1;
                while (str[i2] == '*' || qe_isblank(str[i2]))
                    i2++;

                if ((start == 0 || str[start - 1] != '.')
                &&  (!qe_findchar(".(:", str[i]) || flavor == CLANG_PIKE)
                &&  ((types && strfind(types, kbuf))
                ||   ((mode_flags & CLANG_CC) && strfind(c_types, kbuf))
                ||   (((mode_flags & CLANG_CC) || (flavor == CLANG_D)) &&
                     strend(kbuf, "_t", NULL))
                ||   (flavor == CLANG_HAXE && qe_isupper(kbuf[0]) &&
                     qe_islower(kbuf[1]) && 
                     (start == 0 || !qe_findchar("(", str[start - 1]))))) {
                    /* if not cast, assume type declaration */
                    if (str[i2] != ')') {
                        type_decl = 1;
                    }
                    style1 = C_STYLE_TYPE;
                    if (str[i1] == '(' && flavor != CLANG_PIKE) {
                        /* function style cast */
                        style1 = C_STYLE_KEYWORD;
                    }
                    SET_COLOR(str, start, i, style1);
                    continue;
                }

                if (str[i] == '(' || (str[i] == ' ' && str[i + 1] == '(')) {
                    /* function call */
                    /* XXX: different styles for call and definition */
                    SET_COLOR(str, start, i, C_STYLE_FUNCTION);
                    continue;
                }
                if ((mode_flags & CLANG_CC) || flavor == CLANG_JAVA) {
                    /* assume typedef if starting at first column */
                    if (start == 0 && qe_isalpha_(str[i]))
                        type_decl = 1;

                    if (type_decl) {
                        if (start == 0) {
                            /* assume type if first column */
                            SET_COLOR(str, start, i, C_STYLE_TYPE);
                        } else {
                            SET_COLOR(str, start, i, C_STYLE_VARIABLE);
                        }
                    }
                }
                continue;
            }
            break;
        }
        SET_COLOR1(str, start, style);
    }
 the_end:
    if (state & (IN_C_COMMENT | IN_C_COMMENT1 | IN_C_PREPROCESS | 
                 IN_C_STRING | IN_C_STRING_Q | IN_C_STRING_BQ)) {
        /* set style on eol char */
        SET_COLOR1(str, n, style);
    }

    /* strip state if not overflowing from a comment */
    if (!(state & IN_C_COMMENT) && n > 0 && ((str[n - 1] & CHAR_MASK) != '\\'))
        state &= ~(IN_C_COMMENT1 | IN_C_PREPROCESS);

    cp->colorize_state = state;
}

#define MAX_STACK_SIZE  64

/* gives the position of the first non white space character in
   buf. TABs are counted correctly */
static int find_indent1(EditState *s, unsigned int *buf)
{
    unsigned int *p;
    int pos, c;

    p = buf;
    pos = 0;
    for (;;) {
        c = *p++ & CHAR_MASK;
        if (c == '\t')
            pos += s->b->tab_width - (pos % s->b->tab_width);
        else if (c == ' ')
            pos++;
        else
            break;
    }
    return pos;
}

static int find_pos(EditState *s, unsigned int *buf, int size)
{
    int pos, c, i;

    pos = 0;
    for (i = 0; i < size; i++) {
        c = buf[i] & CHAR_MASK;
        if (c == '\t') {
            pos += s->b->tab_width - (pos % s->b->tab_width);
        } else {
            /* simplistic case: assume single width characters */
            pos++;
        }
    }
    return pos;
}

enum {
    INDENT_NORM,
    INDENT_FIND_EQ,
};

/* Check if indentation is already what it should be */
static int check_indent(EditState *s, int offset, int i, int *offset_ptr)
{
    int tw, col, ntabs, nspaces, bad;
    int offset1;

    tw = s->b->tab_width > 0 ? s->b->tab_width : 8;
    col = ntabs = nspaces = bad = 0;

    for (;;) {
        int c = eb_nextc(s->b, offset1 = offset, &offset);
        if (c == '\t') {
            col += tw - col % tw;
            bad |= nspaces;
            ntabs += 1;
        } else
        if (c == ' ') {
            col += 1;
            nspaces += 1;
        } else {
            break;
        }
    }

    *offset_ptr = offset1;

    if (col != i || bad)
        return 0;

    /* check tabs */
    if (s->indent_tabs_mode) {
        return (nspaces >= tw) ? 0 : 1;
    } else {
        return (ntabs > 0) ? 0 : 1;
    }
}

/* Insert n spaces at beginning of line at <offset>.
 * Store new offset after indentation to <*offset_ptr>.
 * Tabs are inserted if s->indent_tabs_mode is true.
 */
static void insert_indent(EditState *s, int offset, int i, int *offset_ptr)
{
    /* insert tabs */
    if (s->indent_tabs_mode) {
        int tw = s->b->tab_width > 0 ? s->b->tab_width : 8;
        while (i >= tw) {
            offset += eb_insert_uchar(s->b, offset, '\t');
            i -= tw;
        }
    }

    /* insert needed spaces */
    offset += eb_insert_spaces(s->b, offset, i);

    *offset_ptr = offset;
}

/* indent a line of C code starting at <offset> */
static void c_indent_line(EditState *s, int offset0)
{
    int offset, offset1, offsetl, c, pos, line_num, col_num;
    int i, j, eoi_found, len, pos1, lpos, style, line_num1, state;
    unsigned int buf[COLORED_MAX_LINE_SIZE], *p;
    unsigned char stack[MAX_STACK_SIZE];
    char buf1[64], *q;
    int stack_ptr;

    /* find start of line */
    eb_get_pos(s->b, &line_num, &col_num, offset0);
    line_num1 = line_num;
    offset = eb_goto_bol(s->b, offset0);
    /* now find previous lines and compute indent */
    pos = 0;
    lpos = -1; /* position of the last instruction start */
    offsetl = offset;
    eoi_found = 0;
    stack_ptr = 0;
    state = INDENT_NORM;
    for (;;) {
        if (offsetl == 0)
            break;
        line_num--;
        offsetl = eb_prev_line(s->b, offsetl);
        offset1 = offsetl;
        len = s->get_colorized_line(s, buf, countof(buf), &offset1, line_num);
        /* store indent position */
        pos1 = find_indent1(s, buf);
        p = buf + len;
        while (p > buf) {
            p--;
            c = *p;
            /* skip strings or comments */
            style = c >> STYLE_SHIFT;
            if (style == C_STYLE_COMMENT
            ||  style == C_STYLE_STRING
            ||  style == C_STYLE_PREPROCESS) {
                continue;
            }
            c = c & CHAR_MASK;
            if (state == INDENT_FIND_EQ) {
                /* special case to search '=' or ; before { to know if
                   we are in data definition */
                if (c == '=') {
                    /* data definition case */
                    pos = lpos;
                    goto end_parse;
                } else if (c == ';') {
                    /* normal instruction case */
                    goto check_instr;
                }
            } else {
                switch (c) {
                case '}':
                    if (stack_ptr >= MAX_STACK_SIZE)
                        return;
                    stack[stack_ptr++] = c;
                    goto check_instr;
                case '{':
                    if (stack_ptr == 0) {
                        if (lpos == -1) {
                            pos = pos1 + s->indent_size;
                            eoi_found = 1;
                            goto end_parse;
                        } else {
                            state = INDENT_FIND_EQ;
                        }
                    } else {
			if (stack[--stack_ptr] != '}') {
			    /* XXX: syntax check ? */
			    goto check_instr;
			}
			goto check_instr;
                    }
                    break;
                case ')':
                case ']':
                    if (stack_ptr >= MAX_STACK_SIZE)
                        return;
                    stack[stack_ptr++] = c;
                    break;
                case '(':
                case '[':
                    if (stack_ptr == 0) {
                        pos = find_pos(s, buf, p - buf) + 1;
                        goto end_parse;
                    } else {
			if (stack[--stack_ptr] != (c == '(' ? ')' : ']')) {
			    /* XXX: syntax check ? */
			}
                    }
                    break;
                case ' ':
                case '\t':
                case '\n':
                    break;
                case ';':
                    /* level test needed for 'for(;;)' */
                    if (stack_ptr == 0) {
                        /* ; { or } are found before an instruction */
                    check_instr:
                        if (lpos >= 0) {
                            /* start of instruction already found */
                            pos = lpos;
                            if (!eoi_found)
                                pos += s->indent_size;
                            goto end_parse;
                        }
                        eoi_found = 1;
                    }
                    break;
                case ':':
                    /* a label line is ignored */
                    /* XXX: incorrect */
                    if (style == C_STYLE_DEFAULT)
                        goto prev_line;
                    break;
                default:
                    if (stack_ptr == 0) {
                        if (style == C_STYLE_KEYWORD) {
                            unsigned int *p1, *p2;
                            /* special case for if/for/while */
                            p1 = p;
                            while (p > buf &&
                                   (p[-1] >> STYLE_SHIFT) == C_STYLE_KEYWORD)
                                p--;
                            p2 = p;
                            q = buf1;
                            while (q < buf1 + countof(buf1) - 1 && p2 <= p1) {
                                *q++ = *p2++ & CHAR_MASK;
                            }
                            *q = '\0';

                            if (!eoi_found && strfind("if|for|while", buf1)) {
                                pos = pos1 + s->indent_size;
                                goto end_parse;
                            }
                        }
                        lpos = pos1;
                    }
                    break;
                }
            }
        }
    prev_line: ;
    }
  end_parse:
    /* compute special cases which depend on the chars on the current line */
    offset1 = offset;
    len = s->get_colorized_line(s, buf, countof(buf), &offset1, line_num1);

    if (stack_ptr == 0) {
        if (!pos && lpos >= 0) {
            /* start of instruction already found */
            pos = lpos;
            if (!eoi_found)
                pos += s->indent_size;
        }
    }

    for (i = 0; i < len; i++) {
        c = buf[i];
        style = c >> STYLE_SHIFT;
        if (qe_isblank(c & CHAR_MASK))
            continue;
        /* if preprocess, no indent */
        if (style == C_STYLE_PREPROCESS) {
            pos = 0;
            break;
        }
        if (qe_isalpha_(c & CHAR_MASK)) {
            j = get_c_identifier(buf1, countof(buf1), buf + i, CLANG_C);

            if (style == C_STYLE_KEYWORD) {
                if (strfind("case|default", buf1))
                    goto unindent;
            }
            for (j += i; qe_isblank(buf[j] & CHAR_MASK); j++)
                continue;
            if (buf[j] == ':')
                goto unindent;
        }
        /* NOTE: strings & comments are correctly ignored there */
        if ((c == '&' || c == '|') && buf[i + 1] == (unsigned int)c)
            goto unindent;

        if (c == '}') {
        unindent:
            pos -= s->indent_size;
            if (pos < 0)
                pos = 0;
            break;
        }
        if (c == '{' && pos == s->indent_size && !eoi_found) {
            pos = 0;
            break;
        }
        break;
    }

    /* the computed indent is in 'pos' */
    /* if on a blank line, reset indent to 0 unless point is on it */
    if (eb_is_blank_line(s->b, offset, &offset1)
    &&  !(s->offset >= offset && s->offset < offset1)) {
        pos = 0;
    }
    /* Do not modify buffer if indentation in correct */
    if (!check_indent(s, offset, pos, &offset1)) {
        /* simple approach to normalization of indentation */
        eb_delete_range(s->b, offset, offset1);
        insert_indent(s, offset, pos, &offset1);
    }
    /* move to the indentation if point was in indent space */
    if (s->offset >= offset && s->offset < offset1) {
        s->offset = offset1;
    }
}

static void do_c_indent(EditState *s)
{
    if (eb_is_in_indentation(s->b, s->offset)
    &&  s->qe_state->last_cmd_func != (CmdFunc)do_c_indent) {
        c_indent_line(s, s->offset);
    } else {
        do_tab(s, 1);
    }
}

static void do_c_electric(EditState *s, int key)
{
    int offset = s->offset;

    do_char(s, key, 1);
    /* reindent line at original point */
    if (s->mode->indent_func)
        (s->mode->indent_func)(s, eb_goto_bol(s->b, offset));
}

static void do_c_return(EditState *s)
{
    int offset = s->offset;

    do_return(s, 1);
    /* reindent line to remove indent on blank line */
    if (s->mode->indent_func) {
        (s->mode->indent_func)(s, eb_goto_bol(s->b, offset));
        if (s->mode->auto_indent)
            (s->mode->indent_func)(s, s->offset);
    }
}

static int ustr_match_mask(const unsigned int *buf, const char *str)
{
    while (*str) {
        if ((*buf++ & CHAR_MASK) != *str++)
            return 0;
    }
    return 1;
}

/* forward / backward preprocessor */
static void do_c_forward_conditional(EditState *s, int dir)
{
    unsigned int buf[COLORED_MAX_LINE_SIZE], *p;
    int line_num, col_num, sharp, level;
    int offset, offset0, offset1;

    offset = offset0 = eb_goto_bol(s->b, s->offset);
    eb_get_pos(s->b, &line_num, &col_num, offset);
    level = 0;
    for (;;) {
        offset1 = offset;
        s->get_colorized_line(s, buf, countof(buf), &offset1, line_num);
        sharp = 0;
        for (p = buf; *p; p++) {
            int c = (*p & CHAR_MASK);
            int style = (*p >> STYLE_SHIFT);
            if (qe_isblank(c))
                continue;
            if (c == '#' && style == C_STYLE_PREPROCESS)
                sharp++;
            else
                break;
        }
        if (sharp == 1) {
            if (ustr_match_mask(p, dir < 0 ? "endif" : "if")) {
                if (level || offset == offset0)
                    level++;
                else
                    break;
            } else
            if (ustr_match_mask(p, "el")) {
                if (offset == offset0)
                    level++;
                else
                if (level <= 1)
                    break;
            } else
            if (ustr_match_mask(p, dir > 0 ? "endif" : "if")) {
                if (level)
                    level--;
                if (!level && offset != offset0)
                    break;
            }
        }
        if (dir > 0) {
            line_num++;
            offset = offset1;
            if (offset >= s->b->total_size)
                break;
        } else {
            if (offset <= 0)
                break;
            line_num--;
            offset = eb_prev_line(s->b, offset);
        }
    }
    s->offset = offset;
}

static void do_c_list_conditionals(EditState *s)
{
    unsigned int buf[COLORED_MAX_LINE_SIZE], *p;
    int line_num, col_num, sharp, level;
    int offset, offset1;
    EditBuffer *b;

    b = eb_scratch("Preprocessor conditionals", BF_UTF8);
    if (!b)
        return;

    offset = eb_goto_bol(s->b, s->offset);
    eb_get_pos(s->b, &line_num, &col_num, offset);
    level = 0;
    while (offset > 0) {
        line_num--;
        offset = eb_prev_line(s->b, offset);
        offset1 = offset;
        s->get_colorized_line(s, buf, countof(buf), &offset1, line_num);
        sharp = 0;
        for (p = buf; *p; p++) {
            int c = (*p & CHAR_MASK);
            int style = (*p >> STYLE_SHIFT);
            if (qe_isblank(c))
                continue;
            if (c == '#' && style == C_STYLE_PREPROCESS)
                sharp++;
            else
                break;
        }
        if (sharp == 1) {
            if (ustr_match_mask(p, "endif")) {
                level++;
            } else
            if (ustr_match_mask(p, "el")) {
                if (level == 0) {
                    eb_insert_buffer_convert(b, 0, s->b, offset, offset1 - offset);
                }
            } else
            if (ustr_match_mask(p, "if")) {
                if (level) {
                    level--;
                } else {
                    eb_insert_buffer_convert(b, 0, s->b, offset, offset1 - offset);
                }
            }
        }
    }
    if (b->total_size > 0) {
        show_popup(b);
    } else {
        eb_free(&b);
        put_status(s, "Not in a #if conditional");
    }
}

/* C mode specific commands */
static CmdDef c_commands[] = {
    CMD2( KEY_CTRL('i'), KEY_NONE,
          "c-indent-command", do_c_indent, ES, "*")
            /* should map to KEY_META + KEY_CTRL_LEFT ? */
    CMD3( KEY_META('['), KEY_NONE,
          "c-backward-conditional", do_c_forward_conditional, ESi, -1, "*v")
    CMD3( KEY_META(']'), KEY_NONE,
          "c-forward-conditional", do_c_forward_conditional, ESi, 1, "*v")
    CMD2( KEY_META('i'), KEY_NONE,
          "c-list-conditionals", do_c_list_conditionals, ES, "")
    CMD2( '{', '}',
          "c-electric-key", do_c_electric, ESi, "*ki")
    CMD2( KEY_RET, KEY_NONE,
          "c-newline", do_c_return, ES, "*v")
    CMD_DEF_END,
};

static int c_mode_probe(ModeDef *mode, ModeProbeData *p)
{
    /* trust the file extension */
    if (match_extension(p->filename, mode->extensions))
        return 80;

    /* weaker match on C comment start */
    if (p->buf[0] == '/' && p->buf[1] == '*')
        return 60;

    /* even weaker match on C++ comment start */
    if (p->buf[0] == '/' && p->buf[1] == '/')
        return 50;

    if (p->buf[0] == '#') {
        /* same for file starting with `#include` or `#pragma`*/
        if (strstart(cs8(p->buf), "#include", NULL)
        ||  strstart(cs8(p->buf), "#pragma", NULL)) {
            return 50;
        }
    }

    return 1;
}

ModeDef c_mode = {
    .name = "C",
    .extensions = c_extensions,
    .mode_probe = c_mode_probe,
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_C | CLANG_CC,
    .keywords = c_keywords,
    .types = c_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef yacc_mode = {
    .name = "Yacc",
    .extensions = "y|yacc",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_C | CLANG_CC | CLANG_YACC,
    .keywords = c_keywords,
    .types = c_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef lex_mode = {
    .name = "Lex",
    .extensions = "l|lex",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_C | CLANG_CC | CLANG_LEX,
    .keywords = c_keywords,
    .types = c_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef cpp_mode = {
    .name = "C++",
    .mode_name = "cpp",
    .extensions = "cc|hh|cpp|hpp|cxx|hxx|CPP|CC|c++",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CPP | CLANG_CC,
    .keywords = cpp_keywords,
    .types = cpp_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

static int objc_mode_probe(ModeDef *mode, ModeProbeData *p)
{
    if (match_extension(p->filename, mode->extensions)) {
        /* favor Objective C over Limbo for .m extension
         * if file is empty, starts with a comment or a #import */
        if (p->buf[0] == '/' || p->buf[0] == '\0'
        ||  strstart(cs8(p->buf), "#import", NULL)) {
            return 81;
        } else {
            return 80;
        }
    }
    return 1;
}

ModeDef objc_mode = {
    .name = "ObjC", /* Objective C */
    .extensions = "m|mm",
    .mode_probe = objc_mode_probe,
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_OBJC | CLANG_CC,
    .keywords = objc_keywords,
    .types = objc_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef csharp_mode = {
    .name = "C#",   /* C Sharp */
    .mode_name = "csharp",
    .extensions = "cs",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CSHARP | CLANG_PREPROC,
    .keywords = csharp_keywords,
    .types = csharp_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef css_mode = {
    .name = "CSS",
    .extensions = "css",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CSS,
    .keywords = css_keywords,
    .types = css_types,
    .indent_func = c_indent_line,
};

ModeDef js_mode = {
    .name = "Javascript",
    .extensions = "js|json",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_JS | CLANG_REGEX,
    .keywords = js_keywords,
    .types = js_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef as_mode = {
    .name = "Actionscript",
    .extensions = "as",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_AS | CLANG_REGEX,
    .keywords = as_keywords,
    .types = as_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef java_mode = {
    .name = "Java",
    .extensions = "jav|java",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_JAVA,
    .keywords = java_keywords,
    .types = java_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef php_mode = {
    .name = "PHP",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_PHP | CLANG_REGEX,
    .keywords = php_keywords,
    .types = php_types,
};

ModeDef go_mode = {
    .name = "Go",
    .extensions = "go",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_GO,
    .keywords = go_keywords,
    .types = go_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef d_mode = {
    .name = "D",
    .extensions = "d|di",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_D,
    .keywords = d_keywords,
    .types = d_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef limbo_mode = {
    .name = "Limbo",
    .extensions = "m",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_LIMBO,
    .keywords = limbo_keywords,
    .types = limbo_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef cyclone_mode = {
    .name = "Cyclone",
    .extensions = "cyc|cyl|cys",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CYCLONE | CLANG_CC,
    .keywords = cyclone_keywords,
    .types = cyclone_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef ch_mode = {
    .name = "Ch",
    .extensions = "chf",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CH | CLANG_CC,
    .keywords = ch_keywords,
    .types = ch_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef squirrel_mode = {
    .name = "Squirrel",
    .extensions = "nut",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_SQUIRREL,
    .keywords = squirrel_keywords,
    .types = squirrel_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef ici_mode = {
    .name = "ICI",
    .extensions = "ici",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_ICI,
    .keywords = ici_keywords,
    .types = ici_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef jsx_mode = {
    .name = "JSX",
    .extensions = "jsx",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_JSX | CLANG_REGEX,
    .keywords = jsx_keywords,
    .types = jsx_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef haxe_mode = {
    .name = "Haxe",
    .extensions = "hx",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_HAXE | CLANG_REGEX,
    .keywords = haxe_keywords,
    .types = haxe_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef dart_mode = {
    .name = "Dart",
    .extensions = "dart",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_DART,
    .keywords = dart_keywords,
    .types = dart_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef pike_mode = {
    .name = "Pike",
    .extensions = "pike",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_PIKE,
    .keywords = pike_keywords,
    .types = pike_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

static ModeDef idl_mode = {
    .name = "IDL",
    .extensions = "idl",
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_IDL | CLANG_PREPROC | CLANG_WLITERALS | CLANG_REGEX,
    .keywords = idl_keywords,
    .types = idl_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

static int calc_mode_probe(ModeDef *mode, ModeProbeData *p)
{
    if (match_extension(p->filename, mode->extensions))
        return 80;
    
    if (p->buf[0] == '#' && p->buf[1] == '!'
    &&  memstr(p->buf, p->line_len, "/calc")) {
        /* GNU Calc script */
        return 80;
    }
    return 1;
}

ModeDef calc_mode = {
    .name = "calc", /* GNU Calc */
    .extensions = "cal|calc",
    .mode_probe = calc_mode_probe,
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_CALC | CLANG_CC,
    .keywords = calc_keywords,
    .types = calc_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

ModeDef enscript_mode = {
    .name = "Enscript", /* GNU Enscript */
    .extensions = "st", /* syntax files */
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_ENSCRIPT | CLANG_REGEX,
    .keywords = enscript_keywords,
    .types = enscript_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

static int qs_mode_probe(ModeDef *mode, ModeProbeData *p)
{
    if (match_extension(p->filename, mode->extensions))
        return 80;
    
    if (!strcmp(p->filename, ".qerc")
    ||  strstr(p->real_filename, "/.qe/config"))
        return 80;

    return 1;
}

ModeDef qscript_mode = {
    .name = "QScript",
    .extensions = "qe|qs",
    .mode_probe = qs_mode_probe,
    .colorize_func = c_colorize_line,
    .colorize_flags = CLANG_QSCRIPT | CLANG_REGEX,
    .keywords = qs_keywords,
    .types = qs_types,
    .indent_func = c_indent_line,
    .auto_indent = 1,
};

static int c_init(void)
{
    const char *p;

    qe_register_mode(&c_mode, MODEF_SYNTAX);
    qe_register_cmd_table(c_commands, &c_mode);
    for (p = ";:#&|"; *p; p++) {
        qe_register_binding(*p, "c-electric-key", &c_mode);
    }

    qe_register_mode(&idl_mode, MODEF_SYNTAX);
    qe_register_mode(&yacc_mode, MODEF_SYNTAX);
    qe_register_mode(&lex_mode, MODEF_SYNTAX);
    qe_register_mode(&cpp_mode, MODEF_SYNTAX);
    qe_register_mode(&objc_mode, MODEF_SYNTAX);
    qe_register_mode(&csharp_mode, MODEF_SYNTAX);
    qe_register_mode(&css_mode, MODEF_SYNTAX);
    qe_register_mode(&js_mode, MODEF_SYNTAX);
    qe_register_mode(&as_mode, MODEF_SYNTAX);
    qe_register_mode(&java_mode, MODEF_SYNTAX);
    qe_register_mode(&php_mode, MODEF_SYNTAX);
    qe_register_mode(&go_mode, MODEF_SYNTAX);
    qe_register_mode(&d_mode, MODEF_SYNTAX);
    qe_register_mode(&limbo_mode, MODEF_SYNTAX);
    qe_register_mode(&cyclone_mode, MODEF_SYNTAX);
    qe_register_mode(&ch_mode, MODEF_SYNTAX);
    qe_register_mode(&squirrel_mode, MODEF_SYNTAX);
    qe_register_mode(&ici_mode, MODEF_SYNTAX);
    qe_register_mode(&jsx_mode, MODEF_SYNTAX);
    qe_register_mode(&haxe_mode, MODEF_SYNTAX);
    qe_register_mode(&dart_mode, MODEF_SYNTAX);
    qe_register_mode(&pike_mode, MODEF_SYNTAX);
    qe_register_mode(&idl_mode, MODEF_SYNTAX);
    qe_register_mode(&calc_mode, MODEF_SYNTAX);
    qe_register_mode(&enscript_mode, MODEF_SYNTAX);
    qe_register_mode(&qscript_mode, MODEF_SYNTAX);

    return 0;
}

qe_module_init(c_init);
