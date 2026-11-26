#ifndef KYNOVA_CHAPIN_H
#define KYNOVA_CHAPIN_H

#include "ast.h"

/* Escribe el programa traducido a Chapín (archivo .gt). */
void chapin_emit(FILE *out, const AstNode *program);

/* Registro de comentarios desde el léxico: open=1 si el comentario sigue abierto. */
void chapin_comment(int line, const char *text, int open);
void chapin_comments_reset(void);

#endif
