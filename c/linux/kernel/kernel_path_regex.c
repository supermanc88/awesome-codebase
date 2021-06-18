#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
/** #include <linux/glob.h> */


/**
* \brief glob_match1 如果linux内核版本有 <linux/glob.h>文件，就直接使用glob_match函数
* 没有的话，直接把这个实现抄上去
* \param pat
* \param str
* \return 
*/
bool __pure glob_match1(char const *pat, char const *str)
{
	/*
	 * Backtrack to previous * on mismatch and retry starting one
	 * character later in the string.  Because * matches all characters
	 * (no exception for /), it can be easily proved that there's
	 * never a need to backtrack multiple levels.
	 */
	char const *back_pat = NULL, *back_str = back_str;

	/*
	 * Loop over each token (character or class) in pat, matching
	 * it against the remaining unmatched tail of str.  Return false
	 * on mismatch, or true after matching the trailing nul bytes.
	 */
	for (;;) {
		unsigned char c = *str++;
		unsigned char d = *pat++;

		switch (d) {
		case '?':	/* Wildcard: anything but nul */
			if (c == '\0')
				return false;
			break;
		case '*':	/* Any-length wildcard */
			if (*pat == '\0')	/* Optimize trailing * case */
				return true;
			back_pat = pat;
			back_str = --str;	/* Allow zero-length match */
			break;
		case '[': {	/* Character class */
			bool match = false, inverted = (*pat == '!');
			char const *class = pat + inverted;
			unsigned char a = *class++;

			/*
			 * Iterate over each span in the character class.
			 * A span is either a single character a, or a
			 * range a-b.  The first span may begin with ']'.
			 */
			do {
				unsigned char b = a;

				if (a == '\0')	/* Malformed */
					goto literal;

				if (class[0] == '-' && class[1] != ']') {
					b = class[1];

					if (b == '\0')
						goto literal;

					class += 2;
					/* Any special action if a > b? */
				}
				match |= (a <= c && c <= b);
			} while ((a = *class++) != ']');

			if (match == inverted)
				goto backtrack;
			pat = class;
			}
			break;
		case '\\':
			d = *pat++;
			/** fallthrough; */
		default:	/* Literal character */
literal:
			if (c == d) {
				if (d == '\0')
					return true;
				break;
			}
backtrack:
			if (c == '\0' || !back_pat)
				return false;	/* No point continuing */
			/* Try again from last *, one character later in str. */
			pat = back_pat;
			str = ++back_str;
			break;
		}
	}
}

/**
* \require <linux/glob.h>
* \brief test_regex 测试使用 glob_match 函数
*/
void test_regex(void)
{
	char patten[] = "/root/*.txt";
	char str[] = "/root/1.vim";
	char str1[] = "/root/1.txt";
	char str2[] = "/root/1.txt1";


	if (glob_match1(patten, str)) {
		printk("%s success\n", str);
	} else {
		printk("%s failed\n", str);
	}
	if (glob_match1(patten, str1)) {
		printk("%s success\n", str1);
	} else {
		printk("%s failed\n", str1);
	}
	if (glob_match1(patten, str2)) {
		printk("%s success\n", str2);
	} else {
		printk("%s failed\n", str2);
	}
}


static int __init kernel_path_regex_init(void)
{
	test_regex();
	return 0;
}


static void __exit kernel_path_regex_exit(void)
{

}


module_init(kernel_path_regex_init);
module_exit(kernel_path_regex_exit);
MODULE_LICENSE("GPL");
