#include <stdio.h>
#include "compiler.h"
#include "helpers/vector.h"
// #include "helpers/buffer.h"

int main() {
	printf("Compiladores - TURMA A/B - GRUPO 3\n");

	struct vector* vec = vector_create(20);

	int value1 = 1;
	int value2 = 1;
	vector_push(vec, &value1);
	vector_push(vec, &value2);

	vector_pop(vec);

	vector_set_peek_pointer(vec, 1);

	vector_peek(vec);

	compile_file("my_file", "my_out_file", 1);

	printf("Se chegou aqui, os testes rodaram kkk\n");

	return 0;
}
