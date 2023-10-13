folder = folder
login = login
cc = cc
file = file

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	scp -r -O $(folder)/codice $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/codice

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/codice $(folder)/codice

compile:
	$(cc) $(file).c -o $(file)