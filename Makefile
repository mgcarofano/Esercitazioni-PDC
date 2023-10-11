folder = folder
login = login

scope:
	ssh $(login)@ui-studenti.scope.unina.it

upload:
	scp -r -O $(folder) $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder)

download:
	scp -r -O $(login)@ui-studenti.scope.unina.it:/homes/DMA/PDC/2024/$(login)/$(folder) $(folder)