#include "scc.h"

int main()
{
	adj_list_t *adj_list = build_adj_list();
	if(!adj_list) {
		return 1;
	}

	print_adj_list(adj_list);

	destroy_adj_list(adj_list);

	return 0;
}
