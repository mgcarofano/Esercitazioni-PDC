folder = prova1
login = CRFMGB01R
file = prova1
out = prova1_1

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
	mpicc -o $(folder)/codice/$(out) -lm $(folder)/codice/$(file).c
	mpiexec --machinefile hostfile -np 8 $(folder)/codice/$(out) 3 10000 1 1

#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist