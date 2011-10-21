.PHONY: clean All

All:
	@echo ----------Building project:[ Lagom - Debug ]----------
	@"$(MAKE)" -f "Lagom.mk"
clean:
	@echo ----------Cleaning project:[ Lagom - Debug ]----------
	@"$(MAKE)" -f "Lagom.mk" clean
