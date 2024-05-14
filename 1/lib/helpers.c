int strlength(char *format) {
    int len = 0;
    while (format[len] != '\0') {
        len++;
    }

    return len;
}

char* intToString(int num, char *str, int base, int isSigned) {
    int i = 0;

    if (num < 0 && isSigned) {
        str[i++] = '-';
        num = -num;
    }

    if (num == 0) {
        str[i++] = '0';
    } else {
        while (num != 0) {
            int rem = num % base;
            str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
            num = num / base;
        }
    }

    str[i] = '\0';

    // Reverse the string
    int start = (str[0] == '-') ? 1 : 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    return str;
} 

char* intToHex(unsigned long dez, char* result) {
    char hex[] = "0123456789ABCDEF";

    int nums = 1;
    int temp = dez;

    while (temp >= 16) {
        temp /= 16;
        nums++;
    }

    

    int i = nums;
    result[i] = '\0';

    while (dez >= 16) {
        int remainder = dez % 16;
        result[--i] = hex[remainder];
        dez /= 16;
    }

    result[--i] = hex[dez];

    return result;
}