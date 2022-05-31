/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx     = 3;  /* border pixel of windows */
static unsigned int snap         = 32; /* snap pixel */
static const int swallowfloating = 0;  /* 1 means swallow floating windows */
static const unsigned int gappih = 4;  /* horizontal inner gap */
static const unsigned int gappiv = 4;  /* vertical inner gap */
static const unsigned int gappoh = 4;  /* horizontal outer gap */
static const unsigned int gappov = 4;  /* vertical outer gap */
static const int smartgaps    = 0; /* 1 means remove gaps for sole windows */
static int showbar            = 1; /* 0 means no bar */
static int topbar             = 1; /* 0 means bottom bar */
static char font[]            = "monospace:size=10"; /* font for dwm */
static char dmenufont[]       = "monospace:size=10"; /* font for dmenu */
static const char *fonts[]    = { font };
static char normbgcolor[]     = "#222222";
static char normbordercolor[] = "#444444";
static char normfgcolor[]     = "#bbbbbb";
static char selfgcolor[]      = "#eeeeee";
static char selbordercolor[]  = "#005577";
static char selbgcolor[]      = "#005577";
static char *colors[][3]      = {
	/* scheme        fg           bg           border  */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", NULL,     NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrangement function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle }, /* monocle symbol overwritten in function */
};

/* key definitions */
#define MODKEY Mod4Mask
#define STACKKEYS(MOD, ACTION) \
	{ MOD,    XK_j,    ACTION##stack, {.i = +1 } }, \
	{ MOD,    XK_k,    ACTION##stack, {.i = -1 } },
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      comboview,      {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      combotag,       {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn",
	dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb",
	selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "st", NULL };

/* Xresources preferences to load at start */
ResourcePref resources[] = {
		{ "font",               STRING,    &font },
		{ "dmenufont",          STRING,    &dmenufont },
		{ "normbgcolor",        STRING,    &normbgcolor },
		{ "normbordercolor",    STRING,    &normbordercolor },
		{ "normfgcolor",        STRING,    &normfgcolor },
		{ "selbgcolor",         STRING,    &selbgcolor },
		{ "selbordercolor",     STRING,    &selbordercolor },
		{ "selfgcolor",         STRING,    &selfgcolor },
		{ "borderpx",           INTEGER,   &borderpx },
		{ "snap",               INTEGER,   &snap },
		{ "showbar",            INTEGER,   &showbar },
		{ "topbar",             INTEGER,   &topbar },
		{ "nmaster",            INTEGER,   &nmaster },
		{ "resizehints",        INTEGER,   &resizehints },
		{ "mfact",              FLOAT,     &mfact },
};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_BackSpace, spawn,       SHCMD("slock") },
	{ MODKEY|ShiftMask,             XK_BackSpace, spawn,       SHCMD("slock xset dpms force off") },
	{ MODKEY|ShiftMask|ControlMask, XK_BackSpace, quit,        {0} },
	{ MODKEY,                       XK_equal,  view,           {0} },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	/* { MODKEY|ShiftMask,             XK_q,      spawn,          SHCMD("") }, */
	{ MODKEY|ShiftMask|ControlMask, XK_q,      quit,           {0} },
	{ MODKEY,                       XK_w,      spawn,          SHCMD("$BROWSER") },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("st -e sudo nmtui") },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("$MAILCLIENT") },
	/* { MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("") }, */
	/* { MODKEY,                       XK_r,      spawn,          SHCMD("") }, */
	/* { MODKEY|ShiftMask,             XK_r,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          SHCMD("st -e htop") },
	{ MODKEY,                       XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrohgaps,     {.i = -1 } },
	{ MODKEY,                       XK_u,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_u,      incrovgaps,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incrivgaps,     {.i = -1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	/* { MODKEY|ShiftMask,             XK_p,      spawn,          SHCMD("") }, */
	/* { MODKEY,                       XK_a,      spawn,          SHCMD("") }, */
	/* { MODKEY|ShiftMask,             XK_a,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_s,      togglesticky,   {0} },
	/* { MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	/* { MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_f,      fullscreen,     {0} },
	{ MODKEY,                       XK_g,      togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_g,      defaultgaps,    {0} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05 } },
	/* { MODKEY|ShiftMask,             XK_h,      spawn,          SHCMD("") }, */
	/* See STACKKEYS below for XK_j and XK_k */
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05 } },
	/* { MODKEY|ShiftMask,             XK_l,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_z,      incrgaps,       {.i = +3 } },
	/* { MODKEY|ShiftMask,             XK_z,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_x,      incrgaps,       {.i = -3 } },
	/* { MODKEY|ShiftMask,             XK_x,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_c,      spawn,          SHCMD("st -e bc -lq") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("st -e bc -lq") },
	{ MODKEY,                       XK_v,      spawn,          SHCMD("st -e vim") },
	/* { MODKEY|ShiftMask,             XK_v,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	/* { MODKEY|ShiftMask,             XK_b,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_n,      spawn,          SHCMD("st -e rss") },
	/* { MODKEY|ShiftMask,             XK_n,      spawn,          SHCMD("") }, */
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	/* { MODKEY|ShiftMask,             XK_m,      spawn,          SHCMD("") }, */
	{ MODKEY|ControlMask,           XK_m,      focusmaster,    {0} },
	{ 0,                            XK_Print,  spawn,          SHCMD("maim ~/dox/img/screenshot-$(date '+%y%m%d-%H%M-%S').png") },
	{ MODKEY,                       XK_Insert, spawn,          SHCMD("notify-send -a \"Clipboard contents:\" -t 500 -- \"$(xclip -o -selection clipboard)\"") },
	{ MODKEY,                       XK_F1,     spawn,          SHCMD("notify-send -a \"Help Menu:\" -t 500 -- \"You should know, Professor. You programmed me.\"") },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ 0,                XF86XK_AudioMute,      spawn,          SHCMD("pamixer -t") },
	{ 0,         XF86XK_AudioRaiseVolume,      spawn,          SHCMD("pamixer --allow-boost -i 3") },
	{ 0,         XF86XK_AudioLowerVolume,      spawn,          SHCMD("pamixer --allow-boost -d 3") },
	/* { 0,             XF86XK_AudioMicMute,      spawn,          SHCMD("") },*/
};

/* mouse click button definitions:
 * ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or
 * ClkRootWin */
static Button buttons[] = {
	/* click            event mask     button      function        argument */
	{ ClkLtSymbol,      0,             Button1,    setlayout,      {0} },
	{ ClkLtSymbol,      0,             Button3,    setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,      0,             Button2,    zoom,           {0} },
	{ ClkStatusText,    0,             Button2,    spawn,          {.v = termcmd } },
	{ ClkClientWin,     MODKEY,        Button1,    movemouse,      {0} },
	{ ClkClientWin,     MODKEY,        Button2,    togglefloating, {0} },
	{ ClkClientWin,     MODKEY,        Button3,    resizemouse,    {0} },
	{ ClkTagBar,        0,             Button1,    view,           {0} },
	{ ClkTagBar,        0,             Button3,    toggleview,     {0} },
	{ ClkTagBar,        MODKEY,        Button1,    tag,            {0} },
	{ ClkTagBar,        MODKEY,        Button3,    toggletag,      {0} },
};
