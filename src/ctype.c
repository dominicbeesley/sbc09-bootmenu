

int toupper(int c) {
	if (c >= 'a' && c <= 'z')
		return c & 0xDF;
	else
		return c;
}

int tolower(int c) {
	if (c >= 'A' && c <= 'Z')
		return c | 0x20;
	else
		return c;
}
