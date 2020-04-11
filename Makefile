flags=

optimize:
	$(eval flags+=-O3)

rm: main.c
	gcc -o rm main.c $(flags)

build: rm

run: rm
	@./rm
