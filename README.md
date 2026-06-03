Rayanne Maria Lucena de Oliveira
20230047177

Multiplicação de matrizes grandes padrão: mapa por linhas (sem merge).
O programa realiza a multiplicação de duas matrizes quadradas A e B de dimensão n × n, gerando a matriz resultado C. A versão paralela utiliza threads POSIX (pthreads), dividindo as linhas de C entre as threads para que cada uma calcule um bloco independente de linhas, sem necessidade de sincronização durante a escrita. Ao final, os resultados da versão paralela são comparados com os da versão sequencial usando uma tolerância numérica para validar a corretude, e os tempos de execução são medidos para calcular o speedup obtido com a paralelização. 

Para compilar e executar:
gcc -O2 -Wall -Wextra -pthread src/*.c -o multMAT && ./multMAT

Para variar o número de threads, basta identificar a variável 'num_threads' e mudar seu valor.

As medições foram feitas no Intel Core i5-1235U com 10 núcleos, 2 de performance e 8 de eficiência. No sistema operacional Linux, Ubuntu. Compilador, gcc.
