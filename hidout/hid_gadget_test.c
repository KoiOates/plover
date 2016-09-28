/* hid_gadget_test
 * Copied from kernel documentation at:
 * www.kernel.org/doc/Documentation/usb/gadget_hid.txt
 * and modified slightly to make it easier to use
 * with Plover.
 */

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_LEN 512

struct options {
	const char    *opt;
	unsigned char val;
};

static struct options kmod[] = {
	{.opt = "Control_l",		.val = 0x01},
	{.opt = "Control_r",		.val = 0x10},
	{.opt = "Shift_l",		.val = 0x02},
	{.opt = "Shift_r",		.val = 0x20},
	{.opt = "Alt_l",			.val = 0x04},
	{.opt = "Alt_r",		.val = 0x40},
	{.opt = "Meta_l",		.val = 0x08},
	{.opt = "Meta_r",		.val = 0x80},
	{.opt = NULL}
};

/* Commented parts in this section not yet implemented,
 * but are available as {#...} commands in Plover dictionary
 * format. Filling in with hid codes as needed. Codes for
 * each key found in the keyboard section of:
 * www.usb.org/developers/hidpage/Hut1_12v2.pdf
 */

static struct options kval[] = {
	{.opt = "a",			.val = 0x04},
	{.opt = "b",			.val = 0x05},
	{.opt = "c",			.val = 0x06},
	{.opt = "d",			.val = 0x07},
	{.opt = "e",			.val = 0x08},
	{.opt = "f",			.val = 0x09},
	{.opt = "g",			.val = 0x0a},
	{.opt = "h",			.val = 0x0b},
	{.opt = "i",			.val = 0x0c},
	{.opt = "j",			.val = 0x0d},
	{.opt = "k",			.val = 0x0e},
	{.opt = "l",			.val = 0x0f},
	{.opt = "m",			.val = 0x10},
	{.opt = "n",			.val = 0x11},
	{.opt = "o",			.val = 0x12},
	{.opt = "p",			.val = 0x13},
	{.opt = "q",			.val = 0x14},
	{.opt = "r",			.val = 0x15},
	{.opt = "s",			.val = 0x16},
	{.opt = "t",			.val = 0x17},
	{.opt = "u",			.val = 0x18},
	{.opt = "v",			.val = 0x19},
	{.opt = "w",			.val = 0x1a},
	{.opt = "x",			.val = 0x1b},
	{.opt = "y",			.val = 0x1c},
	{.opt = "z",			.val = 0x1d},
	{.opt = "1",			.val = 0x1e},
	{.opt = "2",			.val = 0x1f},
	{.opt = "3",			.val = 0x20},
	{.opt = "4",			.val = 0x21},
	{.opt = "5",			.val = 0x22},
	{.opt = "6",			.val = 0x23},
	{.opt = "7",			.val = 0x24},
	{.opt = "8",			.val = 0x25},
	{.opt = "9",			.val = 0x26},
	{.opt = "0",			.val = 0x27},

	{.opt = "f1",			.val = 0x3a},
	{.opt = "f2",			.val = 0x3b},
	{.opt = "f3",			.val = 0x3c},
	{.opt = "f4",			.val = 0x3d},
	{.opt = "f5",			.val = 0x3e},
	{.opt = "f6",			.val = 0x3f},
	{.opt = "f7",			.val = 0x40},
	{.opt = "f8",			.val = 0x41},
	{.opt = "f9",			.val = 0x42},
	{.opt = "f10",			.val = 0x43},
	{.opt = "f11",			.val = 0x44},
	{.opt = "f12",			.val = 0x45},
	{.opt = "f13",			.val = 0x68},
	{.opt = "f14",			.val = 0x69},
	{.opt = "f15",			.val = 0x6a},
	{.opt = "f16",			.val = 0x6b},
	{.opt = "f17",			.val = 0x6c},
	{.opt = "f18",			.val = 0x6d},
	{.opt = "f19",			.val = 0x6e},
	{.opt = "f20",			.val = 0x6f},
	{.opt = "f21",			.val = 0x70},
	{.opt = "f22",			.val = 0x71},
	{.opt = "f23",			.val = 0x72},
	{.opt = "f24",			.val = 0x73},

	{.opt = "kp_divide",	.val = 0x54},
	{.opt = "kp_multiply",	.val = 0x55},
	{.opt = "kp_minus",		.val = 0x56},
	{.opt = "kp_plus",		.val = 0x57},
	{.opt = "kp_add",		.val = 0x57},
	{.opt = "kp_enter",		.val = 0x58},
	{.opt = "kp_return",	.val = 0x58},
	{.opt = "kp_1",			.val = 0x59},
	{.opt = "kp_2",			.val = 0x5a},
	{.opt = "kp_3",			.val = 0x5b},
	{.opt = "kp_4",			.val = 0x5c},
	{.opt = "kp_5",			.val = 0x5d},
	{.opt = "kp_6",			.val = 0x5e},
	{.opt = "kp_7",			.val = 0x5f},
	{.opt = "kp_8",			.val = 0x60},
	{.opt = "kp_9",			.val = 0x61},
	{.opt = "kp_0",			.val = 0x62},
	{.opt = "kp_decimal",	.val = 0x63},
	{.opt = "kp_period",	.val = 0x63},
	{.opt = "kp_stop",		.val = 0x63},
	{.opt = "kp_equals",	.val = 0x67},
	{.opt = "kp_equal",		.val = 0x67},
    //
    //{.opt = "aacute",   .val = 0x},
    //{.opt = "aacute",   .val = 0x},
    //{.opt = "acircumflex",  .val = 0x},
    //{.opt = "acircumflex",  .val = 0x},
    //{.opt = "acute",    .val = 0x},
    //{.opt = "adiaeresis",   .val = 0x},
    //{.opt = "adiaeresis",   .val = 0x},
    //{.opt = "ae",   .val = 0x},
    //{.opt = "ae",   .val = 0x},
    //{.opt = "agrave",   .val = 0x},
    //{.opt = "agrave",   .val = 0x},
    //{.opt = "ampersand",    .val = 0x}, //complex_chars
    {.opt = "apostrophe",   .val = 0x34},
    //{.opt = "aring",    .val = 0x},
    //{.opt = "aring",    .val = 0x},
    //{.opt = "asciicircum",  .val = 0x},
    //{.opt = "asciitilde",   .val = 0x},
    //{.opt = "asterisk",     .val = 0x}, //complex_chars
    //{.opt = "at",   .val = 0x}, //complex_chars
    //{.opt = "atilde",   .val = 0x},
    //{.opt = "atilde",   .val = 0x},
    //{.opt = "backslash",    .val = 0x31},
    {.opt = "backspace",    .val = 0x2a},
    //{.opt = "bar",  .val = 0x},
    //{.opt = "begin",    .val = 0x},
    //{.opt = "braceleft",    .val = 0x},
    //{.opt = "braceright",   .val = 0x},
    {.opt = "bracketleft",  .val = 0x2f},
    {.opt = "bracketright",     .val = 0x30},
    //{.opt = "break",    .val = 0x},
    //{.opt = "brokenbar",    .val = 0x},
    //{.opt = "cancel",   .val = 0x},
    {.opt = "caps_lock",    .val = 0x39},
    //{.opt = "ccedilla",     .val = 0x},
    //{.opt = "ccedilla",     .val = 0x},
    //{.opt = "cent",     .val = 0x},
    //{.opt = "clear",    .val = 0x},
    //{.opt = "colon",    .val = 0x},
    {.opt = "comma",    .val = 0x36},
    //{.opt = "copyright",    .val = 0x},
    //{.opt = "currency",     .val = 0x},
    //{.opt = "dedilla",  .val = 0x},
    //{.opt = "degree",   .val = 0x},
    {.opt = "delete",   .val = 0x4c},
    //{.opt = "diaeresis",    .val = 0x},
    //{.opt = "division",     .val = 0x},
    //{.opt = "dollar",   .val = 0x}, //complex_chars
    {.opt = "down",     .val = 0x51},
    //{.opt = "eacute",   .val = 0x},
    //{.opt = "eacute",   .val = 0x},
    //{.opt = "ecircumflex",  .val = 0x},
    //{.opt = "ecircumflex",  .val = 0x},
    //{.opt = "ediaeresis",   .val = 0x},
    //{.opt = "ediaeresis",   .val = 0x},
    //{.opt = "egrave",   .val = 0x},
    //{.opt = "egrave",   .val = 0x},
    //{.opt = "eisu_shift",   .val = 0x},
    //{.opt = "eisu_toggle",  .val = 0x},
    {.opt = "end",  .val = 0x4d},
    {.opt = "equal",    .val = 0x2e},
    {.opt = "escape",   .val = 0x29},
    //{.opt = "eth",  .val = 0x},
    //{.opt = "eth",  .val = 0x},
    //{.opt = "eth",  .val = 0x},
    //{.opt = "exclam",   .val = 0x}, //complex_chars
    //{.opt = "exclamdown",   .val = 0x},
    //{.opt = "execute",  .val = 0x},
    //{.opt = "f13",  .val = 0x},
    //{.opt = "f14",  .val = 0x},
    //{.opt = "f15",  .val = 0x},
    //{.opt = "f16",  .val = 0x},
    //{.opt = "f17",  .val = 0x},
    //{.opt = "f18",  .val = 0x},
    //{.opt = "f19",  .val = 0x},
    //{.opt = "f20",  .val = 0x},
    //{.opt = "f21",  .val = 0x},
    //{.opt = "f22",  .val = 0x},
    //{.opt = "f23",  .val = 0x},
    //{.opt = "f24",  .val = 0x},
    //{.opt = "f25",  .val = 0x},
    //{.opt = "f26",  .val = 0x},
    //{.opt = "f27",  .val = 0x},
    //{.opt = "f28",  .val = 0x},
    //{.opt = "f29",  .val = 0x},
    //{.opt = "f30",  .val = 0x},
    //{.opt = "f31",  .val = 0x},
    //{.opt = "f32",  .val = 0x},
    //{.opt = "f33",  .val = 0x},
    //{.opt = "f34",  .val = 0x},
    //{.opt = "f35",  .val = 0x},
    //{.opt = "find",     .val = 0x},
    //{.opt = "grave",    .val = 0x},
    //{.opt = "greater",  .val = 0x},
    //{.opt = "guillemotleft",    .val = 0x},
    //{.opt = "hankaku",  .val = 0x},
    //{.opt = "henkan",   .val = 0x},
    //{.opt = "henkan_mode",  .val = 0x},
    //{.opt = "hiragana_katakana",    .val = 0x},
    {.opt = "home",     .val = 0x4a},
    //{.opt = "hyper_l",  .val = 0x},
    //{.opt = "hyper_r",  .val = 0x},
    {.opt = "hyphen",   .val = 0x2d},
    //{.opt = "iacute",   .val = 0x},
    //{.opt = "iacute",   .val = 0x},
    //{.opt = "icircumflex",  .val = 0x},
    //{.opt = "icircumflex",  .val = 0x},
    //{.opt = "idiaeresis",   .val = 0x},
    //{.opt = "idiaeresis",   .val = 0x},
    //{.opt = "igrave",   .val = 0x},
    //{.opt = "igrave",   .val = 0x},
    {.opt = "insert",   .val = 0x49},
    //{.opt = "kana_lock",    .val = 0x},
    //{.opt = "kana_shift",   .val = 0x},
    //{.opt = "kanji",    .val = 0x},
    //{.opt = "katakana",     .val = 0x},
    //{.opt = "kp_begin",     .val = 0x},
    //{.opt = "kp_delete",    .val = 0x},
    //{.opt = "kp_down",  .val = 0x},
    //{.opt = "kp_end",   .val = 0x},
    //{.opt = "kp_f1",    .val = 0x},
    //{.opt = "kp_f2",    .val = 0x},
    //{.opt = "kp_f3",    .val = 0x},
    //{.opt = "kp_f4",    .val = 0x},
    //{.opt = "kp_home",  .val = 0x},
    //{.opt = "kp_insert",    .val = 0x},
    //{.opt = "kp_left",  .val = 0x},
    //{.opt = "kp_multiply",  .val = 0x},
    //{.opt = "kp_next",  .val = 0x},
    //{.opt = "kp_page_down",     .val = 0x},
    //{.opt = "kp_page_up",   .val = 0x},
    //{.opt = "kp_prior",     .val = 0x},
    //{.opt = "kp_right",     .val = 0x},
    //{.opt = "kp_separator",     .val = 0x},
    //{.opt = "kp_space",     .val = 0x},
    //{.opt = "kp_subtract",  .val = 0x},
    //{.opt = "kp_tab",   .val = 0x},
    //{.opt = "kp_up",    .val = 0x},
    //{.opt = "l1",   .val = 0x},
    //{.opt = "l10",  .val = 0x},
    //{.opt = "l2",   .val = 0x},
    //{.opt = "l3",   .val = 0x},
    //{.opt = "l4",   .val = 0x},
    //{.opt = "l5",   .val = 0x},
    //{.opt = "l6",   .val = 0x},
    //{.opt = "l7",   .val = 0x},
    //{.opt = "l8",   .val = 0x},
    //{.opt = "l9",   .val = 0x},
    {.opt = "left",     .val = 0x50},
    //{.opt = "less",     .val = 0x},
    //{.opt = "linefeed",     .val = 0x},
    //{.opt = "macron",   .val = 0x},
    //{.opt = "mae_koho",     .val = 0x},
    //{.opt = "masculine",    .val = 0x},
    //{.opt = "massyo",   .val = 0x},
    //{.opt = "menu",     .val = 0x},
    {.opt = "minus",    .val = 0x2d},
    //{.opt = "mode_switch",  .val = 0x},
    //{.opt = "mu",   .val = 0x},
    //{.opt = "muhenkan",     .val = 0x},
    //{.opt = "multi_key",    .val = 0x},
    //{.opt = "multiplecandidate",    .val = 0x},
    //{.opt = "multiply",     .val = 0x},
    //{.opt = "next",     .val = 0x},
    //{.opt = "nobreakspace",     .val = 0x},
    //{.opt = "notsign",  .val = 0x},
    //{.opt = "ntilde",   .val = 0x},
    //{.opt = "ntilde",   .val = 0x},
    //{.opt = "numbersign",   .val = 0x},
    {.opt = "num_lock",     .val = 0x53},
    //{.opt = "oacute",   .val = 0x},
    //{.opt = "oacute",   .val = 0x},
    //{.opt = "ocircumflex",  .val = 0x},
    //{.opt = "ocircumflex",  .val = 0x},
    //{.opt = "odiaeresis",   .val = 0x},
    //{.opt = "odiaeresis",   .val = 0x},
    //{.opt = "ograve",   .val = 0x},
    //{.opt = "onehalf",  .val = 0x},
    //{.opt = "onequarter",   .val = 0x},
    //{.opt = "onesuperior",  .val = 0x},
    //{.opt = "ordfeminine",  .val = 0x},
    //{.opt = "oslash",   .val = 0x},
    //{.opt = "otilde",   .val = 0x},
    {.opt = "page_down",    .val = 0x4e},
    {.opt = "page_up",  .val = 0x4b},
    //{.opt = "paragraph",    .val = 0x},
    //{.opt = "parenleft",    .val = 0x}, //complex_chars
    //{.opt = "parenright",   .val = 0x}, //complex_chars
    //{.opt = "pause",    .val = 0x},
    //{.opt = "percent",  .val = 0x}, //complex_chars
    {.opt = "period",   .val = 0x37},
    //{.opt = "periodcentered",   .val = 0x},
    //{.opt = "plus",     .val = 0x}, //complex_chars
    //{.opt = "plusminus",    .val = 0x},
    //{.opt = "previouscandidate",    .val = 0x},
    {.opt = "print",    .val = 0x46},
    //{.opt = "prior",    .val = 0x},
    //{.opt = "question",     .val = 0x}, //complex_chars
    //{.opt = "questiondown",     .val = 0x},
    //{.opt = "quotedbl",     .val = 0x},
    //{.opt = "quoteleft",    .val = 0x},
    //{.opt = "quoteright",   .val = 0x},
    //{.opt = "r1",   .val = 0x},
    //{.opt = "r10",  .val = 0x},
    //{.opt = "r11",  .val = 0x},
    //{.opt = "r12",  .val = 0x},
    //{.opt = "r13",  .val = 0x},
    //{.opt = "r14",  .val = 0x},
    //{.opt = "r15",  .val = 0x},
    //{.opt = "r2",   .val = 0x},
    //{.opt = "r3",   .val = 0x},
    //{.opt = "r4",   .val = 0x},
    //{.opt = "r5",   .val = 0x},
    //{.opt = "r6",   .val = 0x},
    //{.opt = "r7",   .val = 0x},
    //{.opt = "r8",   .val = 0x},
    //{.opt = "r9",   .val = 0x},
    //{.opt = "redo",     .val = 0x},
    //{.opt = "registered",   .val = 0x},
    {.opt = "return",   .val = 0x28},
    {.opt = "right",    .val = 0x51},
    //{.opt = "romaji",   .val = 0x},
    //{.opt = "script_switch",    .val = 0x},
    {.opt = "scroll_lock",  .val = 0x47},
    //{.opt = "section",  .val = 0x},
    //{.opt = "select",   .val = 0x},
    {.opt = "semicolon",    .val = 0x33},
    //{.opt = "shift_lock",   .val = 0x},
    //{.opt = "singlecandidate",  .val = 0x},
    {.opt = "slash",    .val = 0x38},
    {.opt = "space",    .val = 0x2c},
    //{.opt = "ssharp",   .val = 0x},
    //{.opt = "sterling",     .val = 0x},
    //{.opt = "super_l",     .val = 0x},
    //{.opt = "super_r",     .val = 0x},
    //{.opt = "sys_req",     .val = 0x},
    {.opt = "tab",     .val = 0x2b},
    //{.opt = "thorn",     .val = 0x},
    //{.opt = "threequarters 	 ",     .val = 0x},
    //{.opt = "threesuperior 	 ",     .val = 0x},
    //{.opt = "touroku",     .val = 0x},
    //{.opt = "twosuperior",     .val = 0x},
    //{.opt = "uacute",     .val = 0x},
    //{.opt = "ucircumflex",     .val = 0x},
    //{.opt = "udiaeresis",     .val = 0x},
    //{.opt = "ugrave",     .val = 0x},
    //{.opt = "ugrave",     .val = 0x},
    //{.opt = "underscore",     .val = 0x}, //complex_chars
    //{.opt = "undo",     .val = 0x},
    {.opt = "up",     .val = 0x52},
    //{.opt = "yacute",   .val = 0x},
    //{.opt = "yacute",   .val = 0x},
    //{.opt = "ydiaeresis",   .val = 0x},
    //{.opt = "yen",  .val = 0x},
    //{.opt = "zenkaku",  .val = 0x},
    //{.opt = "zenkaku_hankaku",  .val = 0x},
    //{.opt = "zen_koho",     .val = 0x},
	{.opt = NULL}
};

int keyboard_fill_report(char report[8], char buf[BUF_LEN], int *hold)
{
	char *tok = strtok(buf, " ");
	int key = 0;
	int i = 0;

	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		if (strcmp(tok, "Hold") == 0) {
			*hold = 1;
			continue;
		}

		if (key < 6) {
			for (i = 0; kval[i].opt != NULL; i++)
				if (strcmp(tok, kval[i].opt) == 0) {
					report[2 + key++] = kval[i].val;
					break;
				}
			if (kval[i].opt != NULL)
				continue;
		}

		if (key < 6)
			if (islower(tok[0])) {
				report[2 + key++] = (tok[0] - ('a' - 0x04));
				continue;
			}

		for (i = 0; kmod[i].opt != NULL; i++)
			if (strcmp(tok, kmod[i].opt) == 0) {
				report[0] = report[0] | kmod[i].val;
				break;
			}
		if (kmod[i].opt != NULL)
			continue;

		if (key < 6)
			fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 8;
}

static struct options mmod[] = {
	{.opt = "--b1", .val = 0x01},
	{.opt = "--b2", .val = 0x02},
	{.opt = "--b3", .val = 0x04},
	{.opt = NULL}
};

int mouse_fill_report(char report[8], char buf[BUF_LEN], int *hold)
{
	char *tok = strtok(buf, " ");
	int mvt = 0;
	int i = 0;
	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		if (strcmp(tok, "Hold") == 0) {
			*hold = 1;
			continue;
		}

		for (i = 0; mmod[i].opt != NULL; i++)
			if (strcmp(tok, mmod[i].opt) == 0) {
				report[0] = report[0] | mmod[i].val;
				break;
			}
		if (mmod[i].opt != NULL)
			continue;

		if (!(tok[0] == '-' && tok[1] == '-') && mvt < 2) {
			errno = 0;
			report[1 + mvt++] = (char)strtol(tok, NULL, 0);
			if (errno != 0) {
				fprintf(stderr, "Bad value:'%s'\n", tok);
				report[1 + mvt--] = 0;
			}
			continue;
		}

		fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 3;
}

static struct options jmod[] = {
	{.opt = "--b1",		.val = 0x10},
	{.opt = "--b2",		.val = 0x20},
	{.opt = "--b3",		.val = 0x40},
	{.opt = "--b4",		.val = 0x80},
	{.opt = "--hat1",	.val = 0x00},
	{.opt = "--hat2",	.val = 0x01},
	{.opt = "--hat3",	.val = 0x02},
	{.opt = "--hat4",	.val = 0x03},
	{.opt = "--hatneutral",	.val = 0x04},
	{.opt = NULL}
};

int joystick_fill_report(char report[8], char buf[BUF_LEN], int *hold)
{
	char *tok = strtok(buf, " ");
	int mvt = 0;
	int i = 0;

	*hold = 1;

	/* set default hat position: neutral */
	report[3] = 0x04;

	for (; tok != NULL; tok = strtok(NULL, " ")) {

		if (strcmp(tok, "--quit") == 0)
			return -1;

		for (i = 0; jmod[i].opt != NULL; i++)
			if (strcmp(tok, jmod[i].opt) == 0) {
				report[3] = (report[3] & 0xF0) | jmod[i].val;
				break;
			}
		if (jmod[i].opt != NULL)
			continue;

		if (!(tok[0] == '-' && tok[1] == '-') && mvt < 3) {
			errno = 0;
			report[mvt++] = (char)strtol(tok, NULL, 0);
			if (errno != 0) {
				fprintf(stderr, "Bad value:'%s'\n", tok);
				report[mvt--] = 0;
			}
			continue;
		}

		fprintf(stderr, "unknown option: %s\n", tok);
	}
	return 4;
}

void print_options(char c)
{
	int i = 0;

	if (c == 'k') {
		printf("	keyboard options:\n"
		       "		Hold\n");
		for (i = 0; kmod[i].opt != NULL; i++)
			printf("\t\t%s\n", kmod[i].opt);
		printf("\n	keyboard values:\n"
		       "		[a-z] or\n");
		for (i = 0; kval[i].opt != NULL; i++)
			printf("\t\t%-8s%s", kval[i].opt, i % 2 ? "\n" : "");
		printf("\n");
	} else if (c == 'm') {
		printf("	mouse options:\n"
		       "		Hold\n");
		for (i = 0; mmod[i].opt != NULL; i++)
			printf("\t\t%s\n", mmod[i].opt);
		printf("\n	mouse values:\n"
		       "		Two signed numbers\n"
		       "--quit to close\n");
	} else {
		printf("	joystick options:\n");
		for (i = 0; jmod[i].opt != NULL; i++)
			printf("\t\t%s\n", jmod[i].opt);
		printf("\n	joystick values:\n"
		       "		three signed numbers\n"
		       "--quit to close\n");
	}
}

int main(int argc, const char *argv[])
{
	const char *filename = NULL;
	int fd = 0;
	char buf[BUF_LEN];
	int cmd_len;
	char report[8];
	int to_send = 8;
	int hold = 0;
	fd_set rfds;
	int retval, i;
    int f_in;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s devname mouse|keyboard|joystick\n",
			argv[0]);
		return 1;
	}

	if (argv[2][0] != 'k' && argv[2][0] != 'm' && argv[2][0] != 'j')
	  return 2;

	filename = argv[1];

	if ((fd = open(filename, O_RDWR, 0666)) == -1) {
		perror(filename);
		return 3;
	}

	print_options(argv[2][0]);

	while (42) {

		FD_ZERO(&rfds);
		FD_SET(STDIN_FILENO, &rfds);
		FD_SET(fd, &rfds);

		retval = select(fd + 1, &rfds, NULL, NULL, NULL);
		if (retval == -1 && errno == EINTR)
			continue;
		if (retval < 0) {
			perror("select()");
			return 4;
		}

		if (FD_ISSET(fd, &rfds)) {
			cmd_len = read(fd, buf, BUF_LEN - 1);
			printf("recv report:");
			for (i = 0; i < cmd_len; i++)
				printf(" %02x", buf[i]);
			printf("\n");
		}

		if (FD_ISSET(STDIN_FILENO, &rfds)) {
			memset(report, 0x0, sizeof(report));

            //f_in = open("/tmp/plovout", O_RDONLY);
            //read(f_in, str, sizeof(str));
			cmd_len = read(STDIN_FILENO, buf, BUF_LEN - 1);
			//cmd_len = read(f_in, buf, BUF_LEN - 1);
            //close(f_in);

			if (cmd_len == 0)
				break;

			buf[cmd_len - 1] = '\0';
			hold = 0;

			memset(report, 0x0, sizeof(report));
			if (argv[2][0] == 'k')
				to_send = keyboard_fill_report(report, buf, &hold);
			else if (argv[2][0] == 'm')
				to_send = mouse_fill_report(report, buf, &hold);
			else
				to_send = joystick_fill_report(report, buf, &hold);

			if (to_send == -1)
				break;

			if (write(fd, report, to_send) != to_send) {
				perror(filename);
				return 5;
			}
			if (!hold) {
				memset(report, 0x0, sizeof(report));
				if (write(fd, report, to_send) != to_send) {
					perror(filename);
					return 6;
				}
			}
		}
	}

	close(fd);
	return 0;
}
