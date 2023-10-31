folder = openmp-esempio
file = openmp
login = CRFMGB01R
home=/project
work=$(home)/prova1/codice

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	ssh $(login)@ui-studenti.scope.unina.it "rm -fr /homes/DMA/PDC/2024/$(login)/$(folder)"
	scp -r -O $(folder) $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/output $(folder)

mpidocker:
	docker run --rm -it -v $(shell pwd):/project mfisherman/openmpi

#	Da utilizzare nella cartella 'prova1/jobs'
mpirun:
	mkdir -p $(home)/prova1/output
	for strategia in $(shell seq 3) ; do \
		for num in 10000 100000 1000000 ; do \
			for ncpu in 1 2 4 7 8 ; do \
				val=$$((val+1)) ; \
				mpicc -o $(work)/prova1_00$$val -lm $(work)/prova1.c ; \
				for i in $(shell seq 10) ; do \
					mpiexec -machinefile $(home)/hostfile -np $$ncpu $(work)/prova1_00$$val \
					$$strategia $$num 3 1 \
					1> $(home)/prova1/output/prova1_00$$val.out \
					2> $(home)/prova1/output/prova1_00$$val.err ; \
				done ; \
			done ; \
		done ; \
	done

clang-openmp-run:
	clang -Xclang -fopenmp \
		-L/opt/homebrew/opt/libomp/lib \
		-I/opt/homebrew/opt/libomp/include -lomp \
		$(folder)/$(file).c -o $(folder)/$(file)
	./$(folder)/$(file)
	rm -f $(folder)/$(file)

#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist
#	https://discussions.apple.com/thread/8336714
#	https://codeforces.com/blog/entry/88063
#	https://stackoverflow.com/questions/71061894/how-to-install-openmp-on-mac-m1