#ifndef __TERMINFO_COMMANDS_H__
#define __TERMINFO_COMMANDS_H__

#include "libterm_internal.h"

void escape_acsc(term_t_i *term);
void escape_bel(term_t_i *term);
void escape_blink(term_t_i *term);
void escape_bold(term_t_i *term);
void escape_clear(term_t_i *term);
void escape_cr(term_t_i *term);
void escape_csr(term_t_i *term);
void escape_cub(term_t_i *term);
void escape_cub1(term_t_i *term);
void escape_cud(term_t_i *term);
void escape_cud1(term_t_i *term);
void escape_cuf(term_t_i *term);
void escape_cuf1(term_t_i *term);
void escape_cup(term_t_i *term);
void escape_cuu(term_t_i *term);
void escape_cuu1(term_t_i *term);
void escape_ed(term_t_i *term);
void escape_el(term_t_i *term);
void escape_el1(term_t_i *term);
void escape_enacs(term_t_i *term);
void escape_home(term_t_i *term);
void escape_ht(term_t_i *term);
void escape_hts(term_t_i *term);
void escape_ind(term_t_i *term);
void escape_ka1(term_t_i *term);
void escape_ka3(term_t_i *term);
void escape_kb2(term_t_i *term);
void escape_kbs(term_t_i *term);
void escape_kc1(term_t_i *term);
void escape_kc3(term_t_i *term);
void escape_kcub1(term_t_i *term);
void escape_kcud1(term_t_i *term);
void escape_kcuf1(term_t_i *term);
void escape_kcuu1(term_t_i *term);
void escape_kent(term_t_i *term);
void escape_kf0(term_t_i *term);
void escape_kf1(term_t_i *term);
void escape_kf10(term_t_i *term);
void escape_kf2(term_t_i *term);
void escape_kf3(term_t_i *term);
void escape_kf4(term_t_i *term);
void escape_kf5(term_t_i *term);
void escape_kf6(term_t_i *term);
void escape_kf7(term_t_i *term);
void escape_kf8(term_t_i *term);
void escape_kf9(term_t_i *term);
void escape_lf1(term_t_i *term);
void escape_lf2(term_t_i *term);
void escape_lf3(term_t_i *term);
void escape_lf4(term_t_i *term);
void escape_mc0(term_t_i *term);
void escape_mc4(term_t_i *term);
void escape_mc5(term_t_i *term);
void escape_rc(term_t_i *term);
void escape_rev(term_t_i *term);
void escape_ri(term_t_i *term);
void escape_rmacs(term_t_i *term);
void escape_rmam(term_t_i *term);
void escape_rmkx(term_t_i *term);
void escape_rmso(term_t_i *term);
void escape_rmul(term_t_i *term);
void escape_rs2(term_t_i *term);
void escape_sc(term_t_i *term);
void escape_sgr(term_t_i *term);
void escape_sgr0(term_t_i *term);
void escape_smacs(term_t_i *term);
void escape_smam(term_t_i *term);
void escape_smkx(term_t_i *term);
void escape_smso(term_t_i *term);
void escape_smul(term_t_i *term);
void escape_tbc(term_t_i *term);

void escape_sgm(term_t_i *term);

#endif /* __TERMINFO_COMMANDS_H__ */
