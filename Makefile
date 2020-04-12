flags=

optimize:
	$(eval flags+=-O3)

rm: main.c
	gcc -o rm main.c $(flags)

pch.h.gch: pch.h
	gcc $<

build: pch.h.gch rm

run: rm
	@./rm

clean:
	rm -f rm
	rm -f rm.exe