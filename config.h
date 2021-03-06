#include "tcl.h"



/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "DejaVu Sans Mono:size=9:style=Bold" };
static const char dmenufont[]       = "DejaVu Sans Mono:size=9:style=Bold";
static const char col_gray1[]       = "#0f0f0f";
static const char col_gray2[]       = "#1d1f21";
static const char col_gray3[]       = "#a2a2a2";
static const char col_gray4[]       = "#e5e5e5";
static const char col_gray5[]       = "#31363b";
static const char col_yellow[]      = "#ffd866";
static const char col_cyan[]        = "#021E57";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray5 },
	[SchemeSel]  = { col_gray4, col_gray2, col_yellow  },
};

static const char *const autostart[] = {
	"/home/andrew/bin/core.sh", NULL,
	"/home/andrew/bin/dwm/autostart.sh", NULL,
	NULL /* terminate */
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 * WM_CLASS(STRING) = instance, class
	 * WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	/* { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 }, */
	/* { "mpv",      NULL,       NULL,       0,            1,           -1 }, */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Guvcview", NULL,       NULL,       0,            1,           -1 },
	{ "guvcview", NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[D]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* static const char *dmenucmd[] = { "/home/andrew/bin/dmenu_run_history.sh", "dmenu", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_gray2, "-sf", col_gray4, topbar ? NULL : "-b", NULL }; */
static const char *dmenucmd[] = { "/home/andrew/bin/dmenu_run_history.sh", "dmenu", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_gray2, "-sf", col_gray4, NULL };
/* static const char *termcmd[]  = { "st", NULL }; */
static const char *termcmd[]  = { "urxvtc", NULL };
/* static const char *termcmd[] = { "sh", "-c", "urxvtc || urxvt"}; */

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY|ShiftMask,             XK_equal,  incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_minus,  incnmaster,     {.i = -1 } },

	{ MODKEY,                       XK_grave,  focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_grave,  focusstack,     {.i = -1 } },

	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      switchcol,      {0} },
	{ MODKEY,                       XK_l,      switchcol,      {0} },
	{ Mod4Mask|Mod1Mask,            XK_h,      setmfact,       {.f = -0.05} },
	{ Mod4Mask|Mod1Mask,            XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Down,   focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Up,     focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_Left,   switchcol,      {0} },
	{ MODKEY,                       XK_Right,  switchcol,      {0} },
	{ Mod4Mask|Mod1Mask,            XK_Left,   setmfact,       {.f = -0.05} },
	{ Mod4Mask|Mod1Mask,            XK_Right,  setmfact,       {.f = +0.05} },

	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_e,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_d,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_r,      setlayout,      {.v = &layouts[4]} },
	/* { MODKEY,                       XK_o,      setlayout,      {.v = &layouts[5]} }, */
	{ MODKEY,                       XK_o,      setlayout,      {0} },
	{ MODKEY,                       XK_t,      togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_m,      focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_m,      tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[1]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

