folder = folder
login = login
cc = cc
file = file

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	ssh $(login)@ui-studenti.scope.unina.it "rm -r /homes/DMA/PDC/2024/$(login)/$(folder)/codice; mkdir -p /homes/DMA/PDC/2024/$(login)/$(folder)/codice mkdir -p /homes/DMA/PDC/2024/$(login)/$(folder)/output"
	scp -r -O $(folder)/codice $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)/output $(folder)/codice

#	https://unix.stackexchange.com/questions/193368/can-scp-create-a-directory-if-it-doesnt-exist