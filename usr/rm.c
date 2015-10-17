#include <usys.h>
#include <uio.h>

int main(int argc, char **argv){
    while (argc > 1){
        if (_unlink(argv[argc - 1]) < 0){
            printf("mkdir: can not remove `%s`\n", argv[argc - 1]);
            goto exit;
        }
        argc--;
    }

exit:
    _exit();
}
