folder = prova1
login = CRFMGB01R

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	ssh $(login)@ui-studenti.scope.unina.it "rm -fr /homes/DMA/PDC/2024/$(login)/$(folder)"
	scp -r -O $(folder) $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/output $(folder)

mpidocker:
	docker run --rm -it -v $(shell pwd):/project mfisherman/openmpi

mpirun:
	mpicc -o $(folder)/codice/$(file) -lm $(folder)/codice/$(file).c
	mpiexec --hostfile hostfile -np 8 $(folder)/codice/$(file) 3 19 0 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19

#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist