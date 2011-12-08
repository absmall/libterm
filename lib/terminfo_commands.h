#ifndef __TERMINFO_COMMANDS_H__
#define __TERMINFO_COMMANDS_H__

#include "libterm_internal.h"

void escape_cup(term_t_i *term);
void escape_cuu(term_t_i *term);
void escape_cud(term_t_i *term);
void escape_cuf(term_t_i *term);
void escape_cub(term_t_i *term);
void escape_sc(term_t_i *term);
void escape_rc(term_t_i *term);
void escape_el(term_t_i *term);
void escape_clear(term_t_i *term);
void escape_sgr0(term_t_i *term);
void escape_smkx(term_t_i *term);
void escape_csr(term_t_i *term);
void escape_home(term_t_i *term);
void escape_rmul(term_t_i *term);
void escape_ed(term_t_i *term);
void escape_rmkx(term_t_i *term);
void escape_cuf1(term_t_i *term);
void escape_sgm(term_t_i *term);

#endif /* __TERMINFO_COMMANDS_H__ */
