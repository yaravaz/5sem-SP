#include <stdio.h>

int glob_var_in = 1;
int glob_var_unin;

static int static_glob_var_in = 10;
static int static_glob_var_unin;

void func() {
	printf("output");
}

int main(int argc, char* argv[])
{
	int loc_var_in = 2;
	int loc_var_unin;

	static int static_loc_var_in = 20;
	static int static_loc_var_unin;

    printf("Addresses:\n");
    printf("Address of glob_var_in: %p\n", (void*)&glob_var_in);
    printf("Address of glob_var_unin: %p\n", (void*)&glob_var_unin);
    printf("Address of static_glob_var_in: %p\n", (void*)&static_glob_var_in);
    printf("Address of static_glob_var_unin: %p\n", (void*)&static_glob_var_unin);
    printf("Address of loc_var_in: %p\n", (void*)&loc_var_in);
    printf("Address of loc_var_unin: %p\n", (void*)&loc_var_unin);
    printf("Address of static_loc_var_in: %p\n", (void*)&static_loc_var_in);
    printf("Address of static_loc_var_unin: %p\n", (void*)&static_loc_var_unin);
    printf("Address of func: %p\n", (void*)&func);
    printf("Address of argc: %p\n", (void*)&argc);
    printf("Address of argv: %p\n", (void*)&argv);

    printf("Press any key to continue...\n");
    getchar();

	return 0;
}