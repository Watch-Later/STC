#include <stc/cstr.h>
#include <stc/csview.h>
#include <stc/utf8.h>

int main() {
    c_auto (cstr, hello) {
        hello = cstr_new("hell😀 world");
        printf("%s\n", hello.str);

        cstr_replace_sv(
            &hello, 
            utf8_substr(hello.str, 4, 1), 
            c_sv("🐨")
        );
        printf("%s\n", hello.str);

        csview sv = csview_from_s(hello);
        c_foreach (c, csview, sv)
            printf(c_PRIsv ",", c_ARGsv(c.codep));
    }
}
