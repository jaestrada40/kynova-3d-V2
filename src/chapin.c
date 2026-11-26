#include "chapin.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Traductor Kynova -> Chapín (grupo 4): recorre el AST y escribe Chapin.gt.
   va pisto/quetzal/cuento = splice adam/eve/plasmid | cabal/sino = atlas/ryan | brega = vita
   aguas = radio (leer una variable) | shute = diary (salida) */

/* Comentarios del fuente, en orden. El AST no los guarda: el léxico los registra aquí
   y se reponen antes de la primera instrucción que empieza en una línea posterior.
   shortcut: un comentario al final de una línea de código sale antes de la siguiente instrucción, y uno al cierre de un bloque sale tras el bloque; mover si piden fidelidad exacta. */
typedef struct { int line; char *text; } Comment;
static Comment *comments;
static int ncomments, capcomments, next_comment;
static char *pending;
static int pending_line;

void chapin_comment(int line, const char *text, int open)
{
    if (!pending) { pending = strdup(""); pending_line = line; }
    pending = realloc(pending, strlen(pending) + strlen(text) + 1);
    if (!pending) { fprintf(stderr, "kynova: memoria insuficiente\n"); exit(2); }
    strcat(pending, text);
    if (open) return;
    if (ncomments == capcomments) {
        capcomments = capcomments ? capcomments * 2 : 16;
        comments = realloc(comments, capcomments * sizeof *comments);
        if (!comments) { fprintf(stderr, "kynova: memoria insuficiente\n"); exit(2); }
    }
    comments[ncomments++] = (Comment){ pending_line, pending };
    pending = NULL;
}

void chapin_comments_reset(void)
{
    for (int i = 0; i < ncomments; i++) free(comments[i].text);
    free(pending);
    pending = NULL;
    ncomments = next_comment = 0;
}

static void indent(FILE *o, int n) { fprintf(o, "%*s", n * 4, ""); }

static void expr(FILE *o, const AstNode *e)
{
    if (e->paren) fputc('(', o);
    switch (e->kind) {
    case NODE_INT: case NODE_REAL: case NODE_STR: case NODE_ID: fputs(e->text, o); break;
    case NODE_BINOP:
    case NODE_RELOP: {
        expr(o, e->child[0]);
        fprintf(o, " %s ", op_symbol(e->op));
        expr(o, e->child[1]);
        break;
    }
    default: break;
    }
    if (e->paren) fputc(')', o);
}

static void block(FILE *o, const AstNode *b, int lvl);

static void flush_comments(FILE *o, int before_line, int lvl)
{
    for (; next_comment < ncomments && comments[next_comment].line < before_line; next_comment++) {
        indent(o, lvl);
        fprintf(o, "%s\n", comments[next_comment].text);
    }
}

static void stmt(FILE *o, const AstNode *s, int lvl)
{
    flush_comments(o, s->line, lvl);
    indent(o, lvl);
    switch (s->kind) {
    case NODE_DECL:
        fprintf(o, "va %s%s", s->type == TYPE_ADAM ? "pisto " : s->type == TYPE_EVE ? "quetzal " : "cuento ", s->text);
        if (s->child[0]) { fputs(" = ", o); expr(o, s->child[0]); }
        fputs(";\n", o);
        break;
    case NODE_ASSIGN: fprintf(o, "%s = ", s->text); expr(o, s->child[0]); fputs(";\n", o); break;
    case NODE_READ:   fprintf(o, "aguas(%s);\n", s->text); break;
    case NODE_WRITE:  fputs("shute(", o); expr(o, s->child[0]); fputs(");\n", o); break;
    case NODE_IF:
        fputs("cabal (", o); expr(o, s->child[0]); fputs(") ", o);
        block(o, s->child[1], lvl);
        fputs(" sino ", o);
        block(o, s->child[2], lvl);
        fputc('\n', o);
        break;
    case NODE_WHILE:
        fputs("brega (", o); expr(o, s->child[0]); fputs(") ", o);
        block(o, s->child[1], lvl);
        fputc('\n', o);
        break;
    default: break;
    }
}

static void block(FILE *o, const AstNode *b, int lvl)
{
    fputs("{\n", o);
    for (int i = 0; i < b->count; i++) stmt(o, b->items[i], lvl + 1);
    indent(o, lvl);
    fputc('}', o);
}

void chapin_emit(FILE *out, const AstNode *program)
{
    next_comment = 0;
    for (int i = 0; i < program->count; i++) stmt(out, program->items[i], 0);
    flush_comments(out, INT_MAX, 0);
}
