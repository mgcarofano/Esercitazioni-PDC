folder = ./prova2/codice
file = 1108_esercizio3
login = CRFMGB01R
home=/project
work=$(home)/prova1/codice
prova2=$(home)/esercizio-mpi

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
	mpicc $(prova2)/$(file).c -o $(prova2)/$(file)
	mpiexec -machinefile $(home)/hostfile -np 9 $(prova2)/$(file)
	rm -f $(prova2)/$(file)

#	Da utilizzare nella cartella 'prova1/jobs'
mpirun_prova1:
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
	./$(folder)/$(file) 100 100 1 3 1
	rm -f $(folder)/$(file)

#	clang -Xclang -fopenmp -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -lomp prova2.c -o prova2
#	./prova2 100 100 1 3 1

prova3_workdir=prova3/codice
prova3_output=prova3/output
prova3_bin=prova3/bin
id_test=prova3_028

menu_prova3:
	mkdir -p $(prova3_bin)
	gcc -o $(prova3_bin)/menu -lm $(prova3_workdir)/menu.c
	./$(prova3_bin)/menu
	rm -fr $(prova3_bin)

mpirun_prova3:
	mkdir -p $(prova3_bin)
	rm -fr $(prova3_output)/$(id_test)
	mkdir -p $(prova3_output)/$(id_test)
	mpicc -o $(prova3_bin)/$(id_test) -lm $(prova3_workdir)/prova3.c
	mpiexec -machinefile /project/hostfile -np 4 $(prova3_bin)/$(id_test) 4 4 4 4 2 0 0 28 prova3/input/A_mat.csv prova3/input/B_mat.csv \
		1> $(prova3_output)/$(id_test)/std.out \
		2> $(prova3_output)/$(id_test)/std.err
	rm -fr $(prova3_bin)

test_prova3:
	mkdir -p $(prova3_bin)
	test=1 ; \
	val=$$(((($$test-1)*9))) ; \
	for dim in 36 72 108 ; do \
		for ncpu in 1 4 9 ; do \
			val=$$((val+1)) ; \
			rm -fr $(prova3_output)/prova3_00$$val ; \
			mkdir -p $(prova3_output)/prova3_00$$val ; \
			mpicc -o $(prova3_bin)/prova3_00$$val -lm $(prova3_workdir)/prova3.c ; \
			for i in $(shell seq 10) ; do \
				mpiexec -machinefile /project/hostfile -np $$ncpu $(prova3_bin)/prova3_00$$val \
				$$dim $$dim $$dim $$dim 0 $$test 1 $$val \
				1> $(prova3_output)/prova3_00$$val/std.out \
				2> $(prova3_output)/prova3_00$$val/std.err ; \
			done ; \
		done ; \
	done
	rm -fr $(prova3_bin)

media_prova3:
	mkdir -p $(prova3_bin)
	mpicc -o $(prova3_bin)/media-csv -lm $(prova3_workdir)/media-csv.c
	mpiexec -machinefile /project/hostfile -np 1 $(prova3_bin)/media-csv $(prova3_output)/prova3_time.csv
	rm -fr $(prova3_bin)

#	RIFERIMENTI
#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist
#	https://discussions.apple.com/thread/8336714
#	https://codeforces.com/blog/entry/88063
#	https://stackoverflow.com/questions/71061894/how-to-install-openmp-on-mac-m1
