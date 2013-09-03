void hariman(void)
{
    int i;
	for (i = 0xa0000; i <= 0xaffff; i++) {
        int* p = i;
        *p = i & 0x0f;
	}

fin:
	goto fin;
}
