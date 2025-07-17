#include"id_gens.h"

char *id_gen_e() {
	static char id[1000] = "";
	static int id_index = 0;
	id[id_index++] = 'e';
	return id;
}
