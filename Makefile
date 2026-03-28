# Default target
all: run

# Targets
build:
	@echo "Building Foundry..."
	@echo "Cleaning up old object files..."
	@rm -f main.o mesh.o player.o blocks.o chunks.o rendering.o textures.o

#   Compile source files
	@g++ -c src/main.cpp -o main.o

#   Link object files into final executable
	@g++ main.o \
		-o Foundry \
		-lraylib -lm -ldl -lpthread -no-pie

	@$(MAKE) --no-print-directory clean1

clean1:
	@rm -f main.o

run:
	@$(MAKE) --no-print-directory build
	@./Foundry
	@$(MAKE) --no-print-directory clean1
	@$(MAKE) --no-print-directory clean2

clean2:
	@rm -f Foundry

clean:
	@$(MAKE) --no-print-directory clean1
	@$(MAKE) --no-print-directory clean2

.PHONY: all build clean1 run clean2 clean