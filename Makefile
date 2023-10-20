folder = prova1
login = CRFMGB01R

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	ssh $(login)@ui-studenti.scope.unina.it "rm -fr /homes/DMA/PDC/2024/$(login)/$(folder)"
#	mkdir -p /homes/DMA/PDC/2024/$(login)/$(folder)/codice
	scp -r -O $(folder) $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/output $(folder)

#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist