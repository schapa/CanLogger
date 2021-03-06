/*
 * dbg_base.h
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#ifndef DBG_BASE_H_
#define DBG_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ANSI_ESC_DEFAULT "\x1b[0m"
#define ANSI_ESC_COLOR_BLACK "\x1b[30m"
#define ANSI_ESC_COLOR_RED "\x1b[31m"
#define ANSI_ESC_COLOR_GREEN "\x1b[32m"
#define ANSI_ESC_COLOR_YELLOW "\x1b[33m"
#define ANSI_ESC_COLOR_BLUE "\x1b[34m"
#define ANSI_ESC_COLOR_MAGENTA "\x1b[35m"
#define ANSI_ESC_COLOR_CYAN "\x1b[36m"
#define ANSI_ESC_COLOR_WHITE "\x1b[37m"
#define ANSI_ESC_STYLE_BOLD "\x1b[1m"
#define ANSI_ESC_STYLE_ITALIC "\x1b[3m"
#define ANSI_ESC_STYLE_UNDERLINE "\x1b[4m"
#define ANSI_ESC_STYLE_STRIKE "\x1b[9m"

#define DBGMSG_L(...)
#define DBGMSG_M(...)
#define DBGMSG_H(...)
#define DBGMSG_WARN(...)
#define DBGMSG_ERR(...)
#define DBG_ENTRY
#define DBG_EXIT

#define DBGMSG_INFO(fmt, ...) \
	dbgmsg(ANSI_ESC_COLOR_WHITE, "", "", "", 0, fmt, ##__VA_ARGS__)

void dbgmsg(const char *clr, const char *siv, const char *file, const char *func, int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* DBG_BASE_H_ */
