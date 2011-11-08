#include <libterm.h>

int main(int argc, char *argv[])
{
	term_t handle;
	term_create(80, 25, 0, &handle);

	term_free(handle);
}
